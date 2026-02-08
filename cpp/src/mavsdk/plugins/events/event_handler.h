#pragma once

#include "system_impl.h"
#include <vector>
#include <array>
#include <mutex>

#include "mavlink_include.h"
#include <libevents/protocol/receive.h>
#include <libevents/parse/parser.h>
#include <libevents/parse/health_and_arming_checks.h>

namespace mavsdk {

/*
 * Handles the events protocol for a single mavlink component
 */
class EventHandler {
public:
    using handle_event_f = std::function<void(std::unique_ptr<events::parser::ParsedEvent>)>;
    using health_and_arming_checks_updated_f = std::function<void()>;

    EventHandler(
        const std::string& profile,
        handle_event_f handle_event_cb,
        health_and_arming_checks_updated_f health_and_arming_checks_updated_cb,
        SystemImpl& system_impl,
        uint8_t system_id,
        uint8_t component_id);
    ~EventHandler();

    void set_metadata(const std::string& metadata_json);

    const events::HealthAndArmingChecks::Results& health_and_arming_check_results() const
    {
        return _health_and_arming_checks.results();
    }
    bool health_and_arming_check_results_valid() const { return _health_and_arming_checks_valid; }

    /**
     * Get the mode group (used for the health and arming checks) from a custom_mode mavlink mode.
     */
    std::optional<int> get_mode_group(uint32_t custom_mode) const;

private:
    void got_event(const mavlink_event_t& event);
    void handle_mavlink_message(const mavlink_message_t& message);

    std::mutex _protocol_mutex{};
    std::unique_ptr<events::ReceiveProtocol> _protocol{nullptr};
    TimeoutHandler::Cookie _timer_cookie{0};
    events::parser::Parser _parser;
    events::HealthAndArmingChecks _health_and_arming_checks;
    bool _health_and_arming_checks_valid{false};
    const handle_event_f _handle_event_cb;
    const health_and_arming_checks_updated_f _health_and_arming_checks_updated_cb;
    const uint8_t _compid;
    SystemImpl& _system_impl;

    std::vector<mavlink_event_t>
        _pending_events; ///< stores incoming events until we have the metadata loaded

    std::array<void*, 3> _message_handler_cookies{};
};

} // namespace mavsdk
