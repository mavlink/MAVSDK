#pragma once

#include "mavlink_include.h"
#include "mavlink_command_receiver.h"
#include "mavlink_mission_transfer.h"
#include "mavlink_parameter_server.h"
#include "mavlink_request_message_handler.h"
#include "mavsdk_time.h"
#include "flight_mode.h"
#include "log.h"

#include <atomic>
#include <mutex>
#include <cstdint>

namespace mavsdk {

class MavsdkImpl;
class ServerPluginImplBase;

class ServerComponentImpl {
public:
    ServerComponentImpl(MavsdkImpl& mavsdk_impl, uint8_t component_id);
    ~ServerComponentImpl();

    void register_plugin(ServerPluginImplBase* server_plugin_impl);
    void unregister_plugin(ServerPluginImplBase* server_plugin_impl);

    class OurSender : public Sender {
    public:
        OurSender(MavsdkImpl& mavsdk_impl, ServerComponentImpl& server_component_impl);
        virtual ~OurSender() = default;
        bool send_message(mavlink_message_t& message) override;
        [[nodiscard]] uint8_t get_own_system_id() const override;
        [[nodiscard]] uint8_t get_own_component_id() const override;
        [[nodiscard]] uint8_t get_system_id() const override;
        [[nodiscard]] Autopilot autopilot() const override;

        uint8_t current_target_system_id{0};

    private:
        MavsdkImpl& _mavsdk_impl;
        ServerComponentImpl& _server_component_impl;
    };

    // FIXME: remove this hack again by writing the proper mission transfer server part
    void set_our_current_target_system_id(uint8_t id) { _our_sender.current_target_system_id = id; }

    struct AutopilotVersion {
        /** @brief MAVLink autopilot_version capabilities. */
        uint64_t capabilities{};
        /** @brief MAVLink autopilot_version flight_sw_version */
        uint32_t flight_sw_version{0};
        /** @brief MAVLink autopilot_version middleware_sw_version */
        uint32_t middleware_sw_version{0};
        /** @brief MAVLink autopilot_version os_sw_version */
        uint32_t os_sw_version{0};
        /** @brief MAVLink autopilot_version board_version */
        uint32_t board_version{0};
        /** @brief MAVLink autopilot_version vendor_id */
        uint16_t vendor_id{0};
        /** @brief MAVLink autopilot_version product_id */
        uint16_t product_id{0};
        /** @brief MAVLink autopilot_version uid2 */
        std::array<uint8_t, 18> uid2{0};
    };

    void register_mavlink_command_handler(
        uint16_t cmd_id,
        const MavlinkCommandReceiver::MavlinkCommandIntHandler& callback,
        const void* cookie);
    void register_mavlink_command_handler(
        uint16_t cmd_id,
        const MavlinkCommandReceiver::MavlinkCommandLongHandler& callback,
        const void* cookie);
    void unregister_mavlink_command_handler(uint16_t cmd_id, const void* cookie);
    void unregister_all_mavlink_command_handlers(const void* cookie);

    using MavlinkMessageHandler = std::function<void(const mavlink_message_t&)>;

    void register_mavlink_message_handler(
        uint16_t msg_id, const MavlinkMessageHandler& callback, const void* cookie);
    void register_mavlink_message_handler(
        uint16_t msg_id, uint8_t cmp_id, const MavlinkMessageHandler& callback, const void* cookie);

    void unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie);
    void unregister_all_mavlink_message_handlers(const void* cookie);

    void register_timeout_handler(
        const std::function<void()>& callback, double duration_s, void** cookie);
    void refresh_timeout_handler(const void* cookie);
    void unregister_timeout_handler(const void* cookie);

    [[nodiscard]] uint8_t get_own_system_id() const;

    void set_own_component_id(uint8_t own_component_id);
    [[nodiscard]] uint8_t get_own_component_id() const;

    Time& get_time();

    bool send_message(mavlink_message_t& message);

    void add_call_every(std::function<void()> callback, float interval_s, void** cookie);
    void change_call_every(float interval_s, const void* cookie);
    void reset_call_every(const void* cookie);
    void remove_call_every(const void* cookie);

    mavlink_message_t
    make_command_ack_message(const MavlinkCommandReceiver::CommandLong& command, MAV_RESULT result);
    mavlink_message_t
    make_command_ack_message(const MavlinkCommandReceiver::CommandInt& command, MAV_RESULT result);

    void send_heartbeat();

    void set_system_status(uint8_t system_status);
    [[nodiscard]] uint8_t get_system_status() const;
    void set_base_mode(uint8_t base_mode);
    [[nodiscard]] uint8_t get_base_mode() const;
    void set_custom_mode(uint32_t custom_mode);
    [[nodiscard]] uint32_t get_custom_mode() const;

    void call_user_callback_located(
        const std::string& filename, const int linenumber, const std::function<void()>& func);

    // Autopilot version data
    void add_capabilities(uint64_t capabilities);
    void set_flight_sw_version(uint32_t flight_sw_version);
    void set_middleware_sw_version(uint32_t middleware_sw_version);
    void set_os_sw_version(uint32_t os_sw_version);
    void set_board_version(uint32_t board_version);
    void set_vendor_id(uint16_t vendor_id);
    void set_product_id(uint16_t product_id);
    bool set_uid2(std::string uid2);
    void send_autopilot_version();

    MavlinkMissionTransfer& mission_transfer() { return _mission_transfer; }
    MavlinkParameterServer& mavlink_parameter_server() { return _mavlink_parameter_server; }
    MavlinkRequestMessageHandler& mavlink_request_message_handler()
    {
        return _mavlink_request_message_handler;
    }

    void do_work();

private:
    MavsdkImpl& _mavsdk_impl;
    uint8_t _own_component_id{MAV_COMP_ID_AUTOPILOT1};

    std::atomic<MAV_STATE> _system_status{MAV_STATE_UNINIT};
    std::atomic<uint8_t> _base_mode{0};
    std::atomic<uint32_t> _custom_mode{0};

    std::mutex _autopilot_version_mutex{};
    AutopilotVersion _autopilot_version{};

    OurSender _our_sender;
    MavlinkCommandReceiver _mavlink_command_receiver;
    MavlinkMissionTransfer _mission_transfer;
    MavlinkParameterServer _mavlink_parameter_server;
    MavlinkRequestMessageHandler _mavlink_request_message_handler;
};

} // namespace mavsdk
