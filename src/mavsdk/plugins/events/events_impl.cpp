
#include "events_impl.h"
#include "callback_list.tpp"
#include "unused.h"

namespace mavsdk {

EventsImpl::EventsImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

EventsImpl::EventsImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

EventsImpl::~EventsImpl()
{
    _system_impl->unregister_plugin(this);
}

void EventsImpl::init()
{
    _system_impl->register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_HEARTBEAT,
        MAV_COMP_ID_AUTOPILOT1,
        std::bind(&EventsImpl::handle_heartbeat, this, std::placeholders::_1),
        &_heartbeat_mode_cookie);
#ifdef MAVLINK_MSG_ID_CURRENT_MODE // Defined in development.xml MAVLink dialect
    _system_impl->register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_CURRENT_MODE,
        MAV_COMP_ID_AUTOPILOT1,
        std::bind(&EventsImpl::handle_current_mode, this, std::placeholders::_1),
        &_current_mode_cookie);
#endif

    const std::lock_guard lg{_mutex};
    // Instantiate the autopilot component. It's not strictly required, it just makes sure to buffer
    // events until we get the metadata
    get_or_create_event_handler(MAV_COMP_ID_AUTOPILOT1);

    // Request metadata
    _system_impl->component_metadata().subscribe_metadata_available(
        [this](MavlinkComponentMetadata::MetadataUpdate update) {
            if (update.type == MavlinkComponentMetadata::MetadataType::Events) {
                set_metadata(update.compid, update.json_metadata);
            }
        });
    _system_impl->component_metadata().request_autopilot_component();
}

void EventsImpl::deinit()
{
    if (_current_mode_cookie) {
        _system_impl->unregister_all_mavlink_message_handlers(_current_mode_cookie);
        _current_mode_cookie = nullptr;
    }
    if (_heartbeat_mode_cookie) {
        _system_impl->unregister_all_mavlink_message_handlers(_heartbeat_mode_cookie);
        _heartbeat_mode_cookie = nullptr;
    }
}

void EventsImpl::enable() {}

void EventsImpl::disable() {}

EventHandler& EventsImpl::get_or_create_event_handler(uint8_t compid)
{
    auto event_data = _events.find(compid);
    if (event_data == _events.end()) {
        // Add new component
        const std::string profile = "dev"; // Could also be "normal"

        auto event_handler = std::make_shared<EventHandler>(
            profile,
            std::bind(&EventsImpl::handle_event, this, compid, std::placeholders::_1),
            std::bind(&EventsImpl::handle_health_and_arming_check_update, this),
            *_system_impl,
            _system_impl->get_system_id(),
            compid);
        event_data = _events.insert(std::make_pair(compid, event_handler)).first;
    }
    return *event_data->second;
}

void EventsImpl::set_metadata(uint8_t compid, const std::string& metadata_json)
{
    const std::lock_guard lg{_mutex};
    auto& event_handler = get_or_create_event_handler(compid);
    event_handler.set_metadata(metadata_json);

    if (!event_handler.health_and_arming_check_results_valid() &&
        compid == MAV_COMP_ID_AUTOPILOT1) {
        // Request arming checks to be reported
        MavlinkCommandSender::CommandLong command{};
        command.command = MAV_CMD_RUN_PREARM_CHECKS;
        command.target_component_id = _system_impl->get_autopilot_id();

        _system_impl->send_command_async(
            command, [this](MavlinkCommandSender::Result result, float) {
                if (result != MavlinkCommandSender::Result::Success) {
                    LogWarn() << "command MAV_CMD_RUN_PREARM_CHECKS failed";
                }
            });
    }
}

void EventsImpl::handle_event(uint8_t compid, std::unique_ptr<events::parser::ParsedEvent> event)
{
    std::optional<Events::LogLevel> maybe_log_level =
        external_log_level(event->eventData().log_levels);

    // Handle special groups & protocols
    if (event->group() == "health" || event->group() == "arming_check") {
        // These are displayed separately
        return;
    }

    // Show message according to the log level, don't show unknown event groups (might be part of a
    // new protocol)
    if (event->group() == "default" && maybe_log_level) {
        std::string message = event->message();
        std::string description = event->description();

        if (event->type() == "append_health_and_arming_messages" && event->numArguments() > 0) {
            const uint32_t custom_mode = event->argumentValue(0).value.val_uint32_t;
            const std::shared_ptr<EventHandler>& event_handler = _events[compid];
            const auto maybe_mode_group = event_handler->get_mode_group(custom_mode);
            if (maybe_mode_group) {
                const int mode_group = maybe_mode_group.value();
                std::vector<events::HealthAndArmingChecks::Check> checks =
                    event_handler->health_and_arming_check_results().checks(mode_group);
                std::vector<std::string> message_checks;
                for (const auto& check : checks) {
                    if (external_log_level(check.log_levels).value_or(Events::LogLevel::Debug) <=
                        Events::LogLevel::Warning) {
                        message_checks.push_back(check.message);
                    }
                }
                if (message_checks.empty()) {
                    // Add all
                    for (const auto& check : checks) {
                        message_checks.push_back(check.message);
                    }
                }
                if (!message.empty() && !message_checks.empty()) {
                    message += "\n";
                }
                if (message_checks.size() == 1) {
                    message += message_checks[0];
                } else {
                    for (const auto& message_check : message_checks) {
                        message += "- " + message_check + "\n";
                    }
                }
            }
        }

        if (!message.empty()) {
            on_new_displayable_event(
                compid, std::move(event), maybe_log_level.value(), message, description);
        }
    }
}

void EventsImpl::on_new_displayable_event(
    uint8_t compid,
    std::unique_ptr<events::parser::ParsedEvent> event,
    Events::LogLevel log_level,
    const std::string& message,
    const std::string& description)
{
    // Notify subscribers
    const std::lock_guard lg{_mutex};
    _events_callbacks.queue(
        Events::Event{
            compid, message, description, log_level, event->eventNamespace(), event->name()},
        [this](const auto& func) { _system_impl->call_user_callback(func); });
}

void EventsImpl::handle_health_and_arming_check_update()
{
    // Notify subscribers
    const std::lock_guard lg{_mutex};
    const auto report = get_health_and_arming_checks_report();
    if (report.first == Events::Result::Success) {
        _health_and_arming_checks_callbacks.queue(
            report.second, [this](const auto& func) { _system_impl->call_user_callback(func); });
    }
}
std::optional<Events::LogLevel> EventsImpl::external_log_level(uint8_t log_levels)
{
    const int external_log_level = log_levels & 0xF;
    switch (external_log_level) {
        case 0:
            return Events::LogLevel::Emergency;
        case 1:
            return Events::LogLevel::Alert;
        case 2:
            return Events::LogLevel::Critical;
        case 3:
            return Events::LogLevel::Error;
        case 4:
            return Events::LogLevel::Warning;
        case 5:
            return Events::LogLevel::Notice;
        case 6:
            return Events::LogLevel::Info;
        case 7:
            return Events::LogLevel::Debug;
            // 8: Protocol
            // 9: Disabled
        default:
            break;
    }
    return std::nullopt;
}
Events::EventsHandle EventsImpl::subscribe_events(const Events::EventsCallback& callback)
{
    const std::lock_guard lg{_mutex};
    return _events_callbacks.subscribe(callback);
}
void EventsImpl::unsubscribe_events(Events::EventsHandle handle)
{
    const std::lock_guard lg{_mutex};
    _events_callbacks.unsubscribe(handle);
}
Events::HealthAndArmingChecksHandle EventsImpl::subscribe_health_and_arming_checks(
    const Events::HealthAndArmingChecksCallback& callback)
{
    const std::lock_guard lg{_mutex};
    // Run callback immediately if the report is available
    const auto report = get_health_and_arming_checks_report();
    if (report.first == Events::Result::Success) {
        _system_impl->call_user_callback([temp_callback = callback, temp_report = report.second] {
            temp_callback(temp_report);
        });
    }
    return _health_and_arming_checks_callbacks.subscribe(callback);
}
void EventsImpl::unsubscribe_health_and_arming_checks(Events::HealthAndArmingChecksHandle handle)
{
    const std::lock_guard lg{_mutex};
    _health_and_arming_checks_callbacks.unsubscribe(handle);
}
std::pair<Events::Result, Events::HealthAndArmingCheckReport>
EventsImpl::get_health_and_arming_checks_report()
{
    const auto& event_handler = get_or_create_event_handler(MAV_COMP_ID_AUTOPILOT1);
    if (!event_handler.health_and_arming_check_results_valid()) {
        return std::make_pair(Events::Result::NotAvailable, Events::HealthAndArmingCheckReport{});
    }
    const uint32_t custom_mode = _maybe_custom_mode_user_intention.value_or(_custom_mode);
    const auto maybe_current_mode_group = event_handler.get_mode_group(custom_mode);
    if (!maybe_current_mode_group) {
        LogDebug() << "Current mode not available (yet)";
        return std::make_pair(Events::Result::NotAvailable, Events::HealthAndArmingCheckReport{});
    }

    const auto& event_handler_results = event_handler.health_and_arming_check_results();
    Events::HealthAndArmingCheckReport report{};

    auto get_problems =
        [&event_handler_results](const std::vector<events::HealthAndArmingChecks::Check>& checks) {
            std::vector<Events::HealthAndArmingCheckProblem> problems;
            for (const auto& check : checks) {
                const auto maybe_log_level = external_log_level(check.log_levels);
                if (maybe_log_level) {
                    problems.push_back(Events::HealthAndArmingCheckProblem{
                        check.message,
                        check.description,
                        maybe_log_level.value(),
                        get_health_component_from_index(
                            event_handler_results, check.affected_health_component_index)});
                }
            }
            return problems;
        };

    // All problems
    report.all_problems = get_problems(event_handler_results.checks());

    // Current mode
    const int current_mode_group = maybe_current_mode_group.value();
    report.current_mode_intention.problems =
        get_problems(event_handler_results.checks(current_mode_group));
    report.current_mode_intention.can_arm_or_run =
        _system_impl->is_armed() ? event_handler_results.canRun(current_mode_group) :
                                   event_handler_results.canArm(current_mode_group);
    report.current_mode_intention.mode_name = mode_name_from_custom_mode(custom_mode);
    // We could add the results for other modes, like Takeoff or Mission, with allows to check if
    // arming is possible for that mode independent of the current mode.
    // For that, we need to get the custom_mode for these.

    // Health components
    for (const auto& [component_name, component] :
         event_handler_results.healthComponents().health_components) {
        const Events::HealthComponentReport health_component_report{
            component_name,
            component.label,
            component.health.is_present,
            component.health.error || component.arming_check.error,
            component.health.warning || component.arming_check.warning};
        report.health_components.push_back(health_component_report);
    }

    return std::make_pair(Events::Result::Success, report);
}
std::string EventsImpl::get_health_component_from_index(
    const events::HealthAndArmingChecks::Results& results, int health_component_index)
{
    if (health_component_index != 0) { // 0 == none
        for (const auto& [component_name, component] :
             results.healthComponents().health_components) {
            if (component.bitmask == (1ull << health_component_index)) {
                return component_name;
            }
        }
    }
    return "";
}
void EventsImpl::handle_current_mode(const mavlink_message_t& message)
{
    UNUSED(message);
#ifdef MAVLINK_MSG_ID_CURRENT_MODE
    mavlink_current_mode_t current_mode;
    mavlink_msg_current_mode_decode(&message, &current_mode);
    if (current_mode.intended_custom_mode != 0) { // 0 == unknown/not supplied
        const bool changed =
            _maybe_custom_mode_user_intention.value_or(0) != current_mode.intended_custom_mode;
        _maybe_custom_mode_user_intention = current_mode.intended_custom_mode;
        if (changed) {
            handle_health_and_arming_check_update();
        }
    }
#endif
}
void EventsImpl::handle_heartbeat(const mavlink_message_t& message)
{
    mavlink_heartbeat_t heartbeat;
    mavlink_msg_heartbeat_decode(&message, &heartbeat);
    if (_custom_mode != heartbeat.custom_mode) {
        _custom_mode = heartbeat.custom_mode;
        if (!_maybe_custom_mode_user_intention) {
            handle_health_and_arming_check_update();
        }
    }
}
std::string EventsImpl::mode_name_from_custom_mode(uint32_t custom_mode) const
{
    // TODO: use Standard Modes MAVLink interface
    switch (to_flight_mode_from_custom_mode(
        _system_impl->autopilot(), _system_impl->get_vehicle_type(), custom_mode)) {
        case FlightMode::FBWA:
            return "FBWA";
        case FlightMode::Autotune:
            return "Autotune";
        case FlightMode::Guided:
            return "Guided";
        case FlightMode::Ready:
            return "Ready";
        case FlightMode::Takeoff:
            return "Takeoff";
        case FlightMode::Hold:
            return "Hold";
        case FlightMode::Mission:
            return "Mission";
        case FlightMode::ReturnToLaunch:
            return "RTL";
        case FlightMode::Land:
            return "Land";
        case FlightMode::Offboard:
            return "Offboard";
        case FlightMode::FollowMe:
            return "Follow Me";
        case FlightMode::Manual:
            return "Manual";
        case FlightMode::Altctl:
            return "Altitude";
        case FlightMode::Posctl:
            return "Position";
        case FlightMode::Acro:
            return "Acro";
        case FlightMode::Rattitude:
            return "Rattitude";
        case FlightMode::Stabilized:
            return "Stabilized";
        case FlightMode::Unknown:
            break;
    }
    return "Unknown";
}
} // namespace mavsdk
