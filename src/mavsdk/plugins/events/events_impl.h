#pragma once

#include "plugins/events/events.h"
#include "plugin_impl_base.h"
#include "callback_list.h"

#include "event_handler.h"

namespace mavsdk {

class EventsImpl : public PluginImplBase {
public:
    explicit EventsImpl(System& system);
    explicit EventsImpl(std::shared_ptr<System> system);
    ~EventsImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Events::EventsHandle subscribe_events(const Events::EventsCallback& callback);
    void unsubscribe_events(Events::EventsHandle handle);

    Events::HealthAndArmingChecksHandle
    subscribe_health_and_arming_checks(const Events::HealthAndArmingChecksCallback& callback);
    void unsubscribe_health_and_arming_checks(Events::HealthAndArmingChecksHandle handle);

    std::pair<Events::Result, Events::HealthAndArmingCheckReport>
    get_health_and_arming_checks_report();

    void set_metadata(uint8_t compid, const std::string& metadata_json);

private:
    void handle_event(uint8_t compid, std::unique_ptr<events::parser::ParsedEvent> event);
    void on_new_displayable_event(
        uint8_t compid,
        std::unique_ptr<events::parser::ParsedEvent> event,
        Events::LogLevel log_level,
        const std::string& message,
        const std::string& description);
    void handle_health_and_arming_check_update();
    EventHandler& get_or_create_event_handler(uint8_t compid);
    void handle_current_mode(const mavlink_message_t& message);
    void handle_heartbeat(const mavlink_message_t& message);

    std::string mode_name_from_custom_mode(uint32_t custom_mode) const;

    static std::string get_health_component_from_index(
        const events::HealthAndArmingChecks::Results& results, int health_component_index);

    static std::optional<Events::LogLevel> external_log_level(uint8_t log_levels);

    std::unordered_map<uint8_t, std::shared_ptr<EventHandler>>
        _events; ///< One protocol handler for each component ID

    std::recursive_mutex _mutex{};
    CallbackList<Events::Event> _events_callbacks;
    CallbackList<Events::HealthAndArmingCheckReport> _health_and_arming_checks_callbacks;

    std::optional<uint32_t>
        _maybe_custom_mode_user_intention; ///< current user intention if available
    uint32_t _custom_mode{}; ///< current custom mode
    void* _current_mode_cookie{};
    void* _heartbeat_mode_cookie{};
};

} // namespace mavsdk
