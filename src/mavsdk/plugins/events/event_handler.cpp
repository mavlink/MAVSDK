
#include "event_handler.h"

#include <utility>

namespace mavsdk {

EventHandler::EventHandler(
    const std::string& profile,
    handle_event_f handle_event_cb,
    health_and_arming_checks_updated_f health_and_arming_checks_updated_cb,
    SystemImpl& system_impl,
    uint8_t system_id,
    uint8_t component_id) :
    _handle_event_cb(std::move(handle_event_cb)),
    _health_and_arming_checks_updated_cb(std::move(health_and_arming_checks_updated_cb)),
    _compid(component_id),
    _system_impl(system_impl)
{
    auto error_cb = [component_id, this](int num_events_lost) {
        _health_and_arming_checks.reset();
        LogWarn() << "Events got lost:" << num_events_lost << "comp_id:" << component_id;
    };

    const auto timeout_cb = [this](int timeout_ms) {
        if (_timer_cookie != 0) {
            _system_impl.unregister_timeout_handler(_timer_cookie);
            _timer_cookie = 0;
        }
        if (timeout_ms > 0) {
            _timer_cookie = _system_impl.register_timeout_handler(
                [this]() {
                    const std::lock_guard lg{_protocol_mutex};
                    _protocol->timerEvent();
                },
                timeout_ms / 1000.0);
        }
    };

    const auto send_request_cb = [this](const mavlink_request_event_t& msg) {
        mavlink_message_t message;
        mavlink_msg_request_event_encode(
            _system_impl.get_own_system_id(), _system_impl.get_own_component_id(), &message, &msg);
        _system_impl.send_message(message);
    };

    _parser.setProfile(profile);

    _parser.formatters().url = [](const std::string& /*content*/, const std::string& link) {
        return link;
    };

    events::ReceiveProtocol::Callbacks callbacks{
        error_cb,
        send_request_cb,
        std::bind(&EventHandler::got_event, this, std::placeholders::_1),
        timeout_cb};
    _protocol = std::make_unique<events::ReceiveProtocol>(
        callbacks,
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        system_id,
        component_id);

    _system_impl.register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_EVENT,
        _compid,
        std::bind(&EventHandler::handle_mavlink_message, this, std::placeholders::_1),
        &_message_handler_cookies[0]);
    _system_impl.register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_CURRENT_EVENT_SEQUENCE,
        _compid,
        std::bind(&EventHandler::handle_mavlink_message, this, std::placeholders::_1),
        &_message_handler_cookies[1]);
    _system_impl.register_mavlink_message_handler_with_compid(
        MAVLINK_MSG_ID_RESPONSE_EVENT_ERROR,
        _compid,
        std::bind(&EventHandler::handle_mavlink_message, this, std::placeholders::_1),
        &_message_handler_cookies[2]);
}
EventHandler::~EventHandler()
{
    if (_timer_cookie != 0) {
        _system_impl.unregister_timeout_handler(_timer_cookie);
    }
    for (const auto& cookie : _message_handler_cookies) {
        _system_impl.unregister_all_mavlink_message_handlers(cookie);
    }
}
void EventHandler::set_metadata(const std::string& metadata_json)
{
    if (_parser.loadDefinitions(metadata_json)) {
        if (_parser.hasDefinitions()) {
            // do we have queued events?
            for (const auto& event : _pending_events) {
                got_event(event);
            }
            _pending_events.clear();
        }
    } else {
        LogErr() << "Failed to load events JSON metadata file";
    }
}

std::optional<int> EventHandler::get_mode_group(uint32_t custom_mode) const
{
    events::parser::Parser::NavigationModeGroups groups = _parser.navigationModeGroups(_compid);
    for (auto group_iter : groups.groups) {
        if (group_iter.second.find(custom_mode) != group_iter.second.end()) {
            return group_iter.first;
        }
    }
    return std::nullopt;
}
void EventHandler::handle_mavlink_message(const mavlink_message_t& message)
{
    const std::lock_guard lg{_protocol_mutex};
    _protocol->processMessage(message);
}
void EventHandler::got_event(const mavlink_event_t& event)
{
    if (!_parser.hasDefinitions()) {
        if (_pending_events.size() > 50) { // Limit size (not expected to hit that limit)
            _pending_events.clear();
        }
        if (_pending_events.empty()) { // Print only for the first to avoid spamming
            LogDebug() << "No metadata, queuing event, ID: " << event.id
                       << ", num pending: " << _pending_events.size();
        }
        _pending_events.push_back(event);
        return;
    }

    std::unique_ptr<events::parser::ParsedEvent> parsed_event =
        _parser.parse(events::EventType(event));
    if (parsed_event == nullptr) {
        LogWarn() << "Got Event without known metadata: ID:" << event.id << "comp id:" << _compid;
        return;
    }

    //    LogDebug() << "Got Event: ID: " << parsed_event->id() << " namespace: " <<
    //    parsed_event->eventNamespace().c_str() <<
    //        " name: " << parsed_event->name().c_str() << " msg: " <<
    //        parsed_event->message().c_str();

    if (_health_and_arming_checks.handleEvent(*parsed_event)) {
        _health_and_arming_checks_valid = true;
        _health_and_arming_checks_updated_cb();
    }
    _handle_event_cb(std::move(parsed_event));
}
} // namespace mavsdk
