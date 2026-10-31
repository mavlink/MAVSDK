#pragma once

#include "plugins/modes_server/modes_server.hpp"
#include "callback_list.hpp"
#include "mavlink_include.hpp"
#include "mavlink_request_message_handler.hpp"
#include "server_plugin_impl_base.hpp"

#include <cstdint>
#include <mutex>
#include <optional>
#include <utility>
#include <vector>

namespace mavsdk {

// Announces the available modes and the current mode. Once modes are set, it also handles mode
// change requests, which ActionServer then leaves alone.
class ModesServerImpl : public ServerPluginImplBase {
public:
    explicit ModesServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~ModesServerImpl() override;

    void init() override;
    void deinit() override;

    ModesServer::Result set_available_modes(const std::vector<ModesServer::Mode>& modes);

    ModesServer::ModeRequestedHandle
    subscribe_mode_requested(const ModesServer::ModeRequestedCallback& callback);
    void unsubscribe_mode_requested(ModesServer::ModeRequestedHandle handle);

    ModesServer::Result set_current_mode(const std::string& name);

private:
    struct Entry {
        ModesServer::Mode mode;
        uint8_t standard_mode;
    };

    std::optional<MAV_RESULT>
    process_available_modes_request(const MavlinkRequestMessageHandler::Params& params);
    std::optional<MAV_RESULT> process_set_standard_mode(float param1);
    std::optional<MAV_RESULT> process_set_mode(uint8_t base_mode, uint32_t custom_mode);
    void process_set_mode_message(const mavlink_message_t& message);
    void report_mode_requested(const ModesServer::Mode& mode);
    uint32_t custom_mode_from_command(const MavlinkCommandReceiver::CommandLong& command) const;

    void check_current_mode();

    void send_available_mode(size_t index);
    void send_available_modes_monitor();
    void send_current_mode(uint32_t custom_mode, uint32_t intended_custom_mode);

    std::mutex _mutex{};
    std::vector<Entry> _entries{};
    bool _modes_set{false};
    uint8_t _seq{0};
    std::optional<uint32_t> _intended_custom_mode{};
    // The current and intended custom mode last sent in CURRENT_MODE.
    std::optional<std::pair<uint32_t, uint32_t>> _last_sent{};
    unsigned _ticks{0};

    std::mutex _callback_mutex{};
    CallbackList<ModesServer::Mode> _mode_requested_callbacks{
        _server_component_impl->io_context()};

    CallEveryHandler::Cookie _call_every_cookie{};
};

} // namespace mavsdk
