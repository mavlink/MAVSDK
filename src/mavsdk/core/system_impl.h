#pragma once

#include "callback_list.h"
#include "flight_mode.h"
#include "mavlink_address.h"
#include "mavlink_include.h"
#include "mavlink_parameter_client.h"
#include "mavlink_command_sender.h"
#include "mavlink_ftp.h"
#include "mavlink_message_handler.h"
#include "mavlink_mission_transfer.h"
#include "mavlink_request_message_handler.h"
#include "mavlink_statustext_handler.h"
#include "request_message.h"
#include "ardupilot_custom_mode.h"
#include "ping.h"
#include "timeout_handler.h"
#include "safe_queue.h"
#include "timesync.h"
#include "system.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <future>

namespace mavsdk {

class MavsdkImpl;
class PluginImplBase;

// This class is the impl of System. This is to hide the private methods
// and functionality from the public library API.
class SystemImpl : public Sender {
public:
    explicit SystemImpl(MavsdkImpl& parent);
    ~SystemImpl() override;

    void init(uint8_t system_id, uint8_t comp_id);

    void enable_timesync();

    System::IsConnectedHandle subscribe_is_connected(const System::IsConnectedCallback& callback);
    void unsubscribe_is_connected(System::IsConnectedHandle handle);

    // void process_mavlink_message(mavlink_message_t& message);

    using MavlinkMessageHandler = std::function<void(const mavlink_message_t&)>;

    void register_mavlink_message_handler(
        uint16_t msg_id, const MavlinkMessageHandler& callback, const void* cookie);
    void register_mavlink_message_handler(
        uint16_t msg_id, uint8_t cmp_id, const MavlinkMessageHandler& callback, const void* cookie);

    void unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie);
    void unregister_all_mavlink_message_handlers(const void* cookie);

    void update_componentid_messages_handler(uint16_t msg_id, uint8_t cmp_id, const void* cookie);

    void register_timeout_handler(
        const std::function<void()>& callback, double duration_s, void** cookie);
    void refresh_timeout_handler(const void* cookie);
    void unregister_timeout_handler(const void* cookie);

    void add_call_every(std::function<void()> callback, float interval_s, void** cookie);
    void change_call_every(float interval_s, const void* cookie);
    void reset_call_every(const void* cookie);
    void remove_call_every(const void* cookie);

    void register_statustext_handler(
        std::function<void(const MavlinkStatustextHandler::Statustext&)>, void* cookie);
    void unregister_statustext_handler(void* cookie);

    bool send_message(mavlink_message_t& message) override;

    Autopilot autopilot() const override { return _autopilot; };

    using CommandResultCallback = MavlinkCommandSender::CommandResultCallback;

    MavlinkCommandSender::Result send_command(MavlinkCommandSender::CommandLong& command);
    MavlinkCommandSender::Result send_command(MavlinkCommandSender::CommandInt& command);

    void send_command_async(
        MavlinkCommandSender::CommandLong command, const CommandResultCallback& callback);
    void send_command_async(
        MavlinkCommandSender::CommandInt command, const CommandResultCallback& callback);

    MavlinkCommandSender::Result set_msg_rate(
        uint16_t message_id, double rate_hz, uint8_t maybe_component_id = MAV_COMP_ID_AUTOPILOT1);

    void set_msg_rate_async(
        uint16_t message_id,
        double rate_hz,
        const CommandResultCallback& callback,
        uint8_t maybe_component_id = MAV_COMP_ID_AUTOPILOT1);

    // Adds unique component ids
    void add_new_component(uint8_t component_id);
    size_t total_components() const;

    System::ComponentDiscoveredHandle
    subscribe_component_discovered(const System::ComponentDiscoveredCallback& callback);
    void unsubscribe_component_discovered(System::ComponentDiscoveredHandle handle);

    System::ComponentDiscoveredIdHandle
    subscribe_component_discovered_id(const System::ComponentDiscoveredIdCallback& callback);
    void unsubscribe_component_discovered_id(System::ComponentDiscoveredIdHandle handle);

    uint8_t get_autopilot_id() const;
    std::vector<uint8_t> get_camera_ids() const;
    uint8_t get_gimbal_id() const;

    bool is_standalone() const;
    bool has_autopilot() const;
    bool has_camera(int camera_id = -1) const;
    bool has_gimbal() const;

    uint8_t get_system_id() const override;
    std::vector<uint8_t> component_ids() const;

    void set_system_id(uint8_t system_id);

    uint8_t get_own_system_id() const override;
    uint8_t get_own_component_id() const override;
    uint8_t get_own_mav_type() const;
    MAV_TYPE get_vehicle_type() const;

    bool is_armed() const { return _armed; }

    MavlinkParameterClient::Result set_param(
        const std::string& name,
        ParamValue value,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    MavlinkParameterClient::Result set_param_float(
        const std::string& name,
        float value,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    MavlinkParameterClient::Result set_param_int(
        const std::string& name,
        int32_t value,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    std::pair<MavlinkParameterClient::Result, std::map<std::string, ParamValue>>
    get_all_params(std::optional<uint8_t> maybe_component_id = {}, bool extended = false);

    MavlinkParameterClient::Result set_param_custom(
        const std::string& name,
        const std::string& value,
        std::optional<uint8_t> maybe_component_id = {});

    using SetParamCallback = std::function<void(MavlinkParameterClient::Result result)>;

    void set_param_float_async(
        const std::string& name,
        float value,
        const SetParamCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    void set_param_int_async(
        const std::string& name,
        int32_t value,
        const SetParamCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    FlightMode get_flight_mode() const;

    MavlinkCommandSender::Result
    set_flight_mode(FlightMode mode, uint8_t component_id = MAV_COMP_ID_AUTOPILOT1);
    void set_flight_mode_async(
        FlightMode mode,
        const CommandResultCallback& callback,
        uint8_t component_id = MAV_COMP_ID_AUTOPILOT1);

    using GetParamAnyCallback =
        std::function<void(MavlinkParameterClient::Result result, ParamValue value)>;
    using GetParamFloatCallback =
        std::function<void(MavlinkParameterClient::Result result, float value)>;
    using GetParamIntCallback =
        std::function<void(MavlinkParameterClient::Result result, int32_t value)>;
    using GetParamCustomCallback =
        std::function<void(MavlinkParameterClient::Result result, const std::string& value)>;

    std::pair<MavlinkParameterClient::Result, float> get_param_float(
        const std::string& name,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);
    std::pair<MavlinkParameterClient::Result, int> get_param_int(
        const std::string& name,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);
    std::pair<MavlinkParameterClient::Result, std::string>
    get_param_custom(const std::string& name, std::optional<uint8_t> maybe_component_id = {});

    // These methods can be used to cache a parameter when a system connects. For that
    // the callback can just be set to nullptr.
    void get_param_async(
        const std::string& name,
        ParamValue value,
        const GetParamAnyCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    void get_param_float_async(
        const std::string& name,
        const GetParamFloatCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    void get_param_int_async(
        const std::string& name,
        const GetParamIntCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);
    void get_param_custom_async(
        const std::string& name, const GetParamCustomCallback& callback, const void* cookie);

    void set_param_async(
        const std::string& name,
        ParamValue value,
        const SetParamCallback& callback,
        const void* cookie,
        std::optional<uint8_t> maybe_component_id = {},
        bool extended = false);

    void subscribe_param_float(
        const std::string& name,
        const MavlinkParameterClient::ParamFloatChangedCallback& callback,
        const void* cookie);

    void subscribe_param_int(
        const std::string& name,
        const MavlinkParameterClient::ParamIntChangedCallback& callback,
        const void* cookie);

    void subscribe_param_custom(
        const std::string& name,
        const MavlinkParameterClient::ParamCustomChangedCallback& callback,
        const void* cookie);

    void cancel_all_param(const void* cookie);

    void param_changed(const std::string& name);

    using ParamChangedCallback = std::function<void(const std::string& name)>;
    void register_param_changed_handler(const ParamChangedCallback& callback, const void* cookie);
    void unregister_param_changed_handler(const void* cookie);

    bool is_connected() const;

    Time& get_time();
    AutopilotTime& get_autopilot_time() { return _autopilot_time; };

    double get_ping_time_s() const { return _ping.last_ping_time_s(); }

    void register_plugin(PluginImplBase* plugin_impl);
    void unregister_plugin(PluginImplBase* plugin_impl);

    void call_user_callback_located(
        const std::string& filename, int linenumber, const std::function<void()>& func);

    void send_autopilot_version_request();
    void send_autopilot_version_request_async(
        const MavlinkCommandSender::CommandResultCallback& callback);
    void send_flight_information_request();

    MavlinkMissionTransfer& mission_transfer() { return _mission_transfer; };

    MavlinkFtp& mavlink_ftp() { return _mavlink_ftp; };

    RequestMessage& request_message() { return _request_message; };

    // Non-copyable
    SystemImpl(const SystemImpl&) = delete;
    const SystemImpl& operator=(const SystemImpl&) = delete;

    double timeout_s() const;

private:
    static bool is_autopilot(uint8_t comp_id);
    static bool is_camera(uint8_t comp_id);

    void process_heartbeat(const mavlink_message_t& message);
    void process_autopilot_version(const mavlink_message_t& message);
    void process_statustext(const mavlink_message_t& message);
    void heartbeats_timed_out();
    void set_connected();
    void set_disconnected();

    std::optional<mavlink_message_t>
    process_autopilot_version_request(const MavlinkCommandReceiver::CommandLong& command);

    static std::string component_name(uint8_t component_id);
    static System::ComponentType component_type(uint8_t component_id);

    void system_thread();

    std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong>
    make_command_flight_mode(FlightMode mode, uint8_t component_id);

    // We use std::pair instead of a std::optional.
    std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong>
    make_command_px4_mode(FlightMode mode, uint8_t component_id);

    std::pair<MavlinkCommandSender::Result, MavlinkCommandSender::CommandLong>
    make_command_ardupilot_mode(FlightMode flight_mode, uint8_t component_id);

    static ardupilot::RoverMode flight_mode_to_ardupilot_rover_mode(FlightMode flight_mode);
    static ardupilot::CopterMode flight_mode_to_ardupilot_copter_mode(FlightMode flight_mode);
    static ardupilot::PlaneMode flight_mode_to_ardupilot_plane_mode(FlightMode flight_mode);

    MavlinkCommandSender::CommandLong
    make_command_msg_rate(uint16_t message_id, double rate_hz, uint8_t component_id);

    static void receive_float_param(
        MavlinkParameterClient::Result result,
        ParamValue value,
        const GetParamFloatCallback& callback);
    static void receive_int_param(
        MavlinkParameterClient::Result result,
        ParamValue value,
        const GetParamIntCallback& callback);

    std::mutex _component_discovered_callback_mutex{};
    CallbackList<System::ComponentType> _component_discovered_callbacks{};
    CallbackList<System::ComponentType, uint8_t> _component_discovered_id_callbacks{};

    MAVLinkAddress _target_address{};

    AutopilotTime _autopilot_time{};

    MavlinkStatustextHandler _statustext_handler{};

    MavlinkParameterClient* param_sender(uint8_t component_id, bool extended);

    struct StatustextCallback {
        std::function<void(const MavlinkStatustextHandler::Statustext&)> callback;
        void* cookie;
    };
    std::mutex _statustext_handler_callbacks_mutex{};
    std::vector<StatustextCallback> _statustext_handler_callbacks;

    std::atomic<bool> _armed{false};
    std::atomic<bool> _hitl_enabled{false};

    std::atomic<Autopilot> _autopilot{Autopilot::Unknown};

    MavsdkImpl& _mavsdk_impl;

    std::thread* _system_thread{nullptr};
    std::atomic<bool> _should_exit{false};

    static constexpr double HEARTBEAT_TIMEOUT_S = 3.0;

    std::mutex _connection_mutex{};
    std::atomic<bool> _connected{false};
    CallbackList<bool> _is_connected_callbacks{};
    void* _heartbeat_timeout_cookie = nullptr;

    std::atomic<bool> _autopilot_version_pending{false};

    static constexpr double _ping_interval_s = 5.0;

    struct ParamSenderEntry {
        std::unique_ptr<MavlinkParameterClient> parameter_client;
        uint8_t component_id;
        bool extended;
    };
    std::mutex _mavlink_parameter_clients_mutex{};
    std::vector<ParamSenderEntry> _mavlink_parameter_clients;
    MavlinkCommandSender _command_sender;

    Timesync _timesync;
    Ping _ping;

    MavlinkMissionTransfer _mission_transfer;
    RequestMessage _request_message;
    MavlinkFtp _mavlink_ftp;

    std::mutex _plugin_impls_mutex{};
    std::vector<PluginImplBase*> _plugin_impls{};

    // We used set to maintain unique component ids
    std::unordered_set<uint8_t> _components{};

    std::mutex _param_changed_callbacks_mutex{};
    std::unordered_map<const void*, ParamChangedCallback> _param_changed_callbacks{};

    MAV_TYPE _vehicle_type{MAV_TYPE::MAV_TYPE_GENERIC};

    std::atomic<FlightMode> _flight_mode{FlightMode::Unknown};

    std::mutex _mavlink_ftp_files_mutex{};
    std::unordered_map<std::string, std::string> _mavlink_ftp_files{};

    bool _old_message_520_supported{true};
    bool _old_message_528_supported{true};
};

} // namespace mavsdk
