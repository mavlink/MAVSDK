#pragma once

#include "plugins/modes_server/modes_server.hpp"
#include "mavlink_include.hpp"
#include "mavlink_request_message_handler.hpp"
#include "server_plugin_impl_base.hpp"

#include <cstdint>
#include <mutex>
#include <optional>
#include <vector>

namespace mavsdk {

// Announces the available modes and the current mode. Changing modes is left to ActionServer,
// which handles MAV_CMD_DO_SET_MODE and SET_MODE and sets the custom mode of the component.
class ModesServerImpl : public ServerPluginImplBase {
public:
    explicit ModesServerImpl(std::shared_ptr<ServerComponent> server_component);
    ~ModesServerImpl() override;

    void init() override;
    void deinit() override;

    ModesServer::Result set_available_modes(const std::vector<ModesServer::Mode>& modes);

private:
    struct Entry {
        ModesServer::Mode mode;
        uint8_t standard_mode;
    };

    std::optional<MAV_RESULT> process_available_modes_request(const MavlinkRequestMessageHandler::Params& params);
    void check_current_mode();

    void send_available_mode(size_t index);
    void send_available_modes_monitor();
    void send_current_mode(uint32_t custom_mode);

    std::mutex _mutex{};
    std::vector<Entry> _entries{};
    bool _modes_set{false};
    uint8_t _seq{0};
    std::optional<uint32_t> _last_custom_mode{};
    unsigned _ticks{0};

    CallEveryHandler::Cookie _call_every_cookie{};
};

} // namespace mavsdk
