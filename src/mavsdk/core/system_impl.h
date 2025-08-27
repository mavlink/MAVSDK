#pragma once

#include "autopilot.h"
#include "callback_list.h"
#include "mavlink_component_metadata.h"
#include "call_every_handler.h"
#include "flight_mode.h"
#include "mavlink_address.h"
#include "mavlink_include.h"
#include "mavlink_parameter_client.h"
#include "mavlink_command_sender.h"
#include "mavlink_ftp_client.h"
#include "mavlink_message_handler.h"
#include "mavlink_mission_transfer_client.h"
#include "mavlink_request_message.h"
#include "mavlink_statustext_handler.h"
#include "ardupilot_custom_mode.h"
#include "ping.h"
#include "timeout_handler.h"
#include "timesync.h"
#include "system.h"
#include "vehicle.h"
#include "libmav_receiver.h"
#include <cstdint>
#include <functional>
#include <atomic>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <future>

// Forward declarations
namespace mav {
class MessageSet;
}

namespace mavsdk {

class MavsdkImpl;
class PluginImplBase;
class Connection;

// This class is the impl of System. This is to hide the private methods
// and functionality from the public library API.
class SystemImpl {
public:
    explicit SystemImpl(MavsdkImpl& parent);
    ~SystemImpl();

    void init(uint8_t system_id, uint8_t comp_id);

    void enable_timesync();

    System::IsConnectedHandle subscribe_is_connected(const System::IsConnectedCallback& callback);
    void unsubscribe_is_connected(System::IsConnectedHandle handle);

    void process_mavlink_message(mavlink_message_t& message);

    void register_mavlink_message_handler(
        uint16_t msg_id, const MavlinkMessageHandler::Callback& callback, const void* cookie);
    void register_mavlink_message_handler_with_compid(
        uint16_t msg_id,
        uint8_t cmp_id,
        const MavlinkMessageHandler::Callback& callback,
        const void* cookie);

    void unregister_mavlink_message_handler(uint16_t msg_id, const void* cookie);
    void unregister_all_mavlink_message_handlers(const void* cookie);

    void
    update_component_id_messages_handler(uint16_t msg_id, uint8_t component_id, const void* cookie);

    // Libmav message handling
    using LibmavMessageCallback = std::function<void(const Mavsdk::MavlinkMessage&)>;

    void process_libmav_message(const Mavsdk::MavlinkMessage& message);

    void register_libmav_message_handler(
        const std::string& message_name, const LibmavMessageCallback& callback, const void* cookie);
    void register_libmav_message_handler_with_compid(
        const std::string& message_name,
        uint8_t cmp_id,
        const LibmavMessageCallback& callback,
        const void* cookie);

    void unregister_libmav_message_handler(const std::string& message_name, const void* cookie);
    void unregister_all_libmav_message_handlers(const void* cookie);

    // Get connections for sending messages
    std::vector<Connection*> get_connections() const;

    // Get MessageSet for message creation and parsing
    mav::MessageSet& get_message_set() const;

    // Thread-safe MessageSet operations
    bool load_custom_xml_to_message_set(const std::string& xml_content);

    TimeoutHandler::Cookie
    register_timeout_handler(const std::function<void()>& callback, double duration_s);
    void refresh_timeout_handler(TimeoutHandler::Cookie cookie);
    void unregister_timeout_handler(TimeoutHandler::Cookie cookie);

    CallEveryHandler::Cookie add_call_every(std::function<void()> callback, float interval_s);
    void change_call_every(float interval_s, CallEveryHandler::Cookie cookie);
    void reset_call_every(CallEveryHandler::Cookie cookie);
    void remove_call_every(CallEveryHandler::Cookie cookie);

    void register_statustext_handler(
        std::function<void(const MavlinkStatustextHandler::Statustext&)>, void* cookie);
    void unregister_statustext_handler(void* cookie);

    bool send_message(mavlink_message_t& message);
    bool queue_message(
        std::function<mavlink_message_t(MavlinkAddress mavlink_address, uint8_t channel)> fun);

    Autopilot autopilot() const { return _autopilot; };

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

    uint8_t get_system_id() const;
    std::vector<uint8_t> component_ids() const;

    void set_system_id(uint8_t system_id);

    uint8_t get_own_system_id() const;
    uint8_t get_own_component_id() const;
    uint8_t get_own_mav_type() const;
    MAV_TYPE get_vehicle_type() const;
    Vehicle vehicle() const;

    bool is_armed() const { return _armed; }

    MavlinkParameterClient* param_sender(uint8_t component_id, bool extended);

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
    AutopilotTime& get_autopilot_time() { return _autopilot_time; }

    double get_ping_time_s() const { return _ping.last_ping_time_s(); }

    void register_plugin(PluginImplBase* plugin_impl);
    void unregister_plugin(PluginImplBase* plugin_impl);

    void call_user_callback_located(
        const std::string& filename, int linenumber, const std::function<void()>& func);

    void send_autopilot_version_request();

    MavlinkMissionTransferClient& mission_transfer_client() { return _mission_transfer_client; }

    MavlinkFtpClient& mavlink_ftp_client() { return _mavlink_ftp_client; }

    MavlinkComponentMetadata& component_metadata() { return _mavlink_component_metadata; }

    MavlinkRequestMessage& mavlink_request_message() { return _mavlink_request_message; }

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

    static std::string component_name(uint8_t component_id);
    static ComponentType component_type(uint8_t component_id);

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

    CallbackList<ComponentType> _component_discovered_callbacks{};
    CallbackList<ComponentType, uint8_t> _component_discovered_id_callbacks{};

    MavlinkAddress _target_address{};

    AutopilotTime _autopilot_time{};

    MavlinkMessageHandler _mavlink_message_handler{};

    // Libmav message handling
    struct LibmavMessageHandler {
        std::string message_name;
        LibmavMessageCallback callback;
        const void* cookie;
        std::optional<uint8_t> component_id; // If specified, only messages from this component
    };
    std::mutex _libmav_message_handlers_mutex{};
    std::vector<LibmavMessageHandler> _libmav_message_handlers{};

    bool _message_debugging = false;

    MavlinkStatustextHandler _statustext_handler{};

    struct StatustextCallback {
        std::function<void(const MavlinkStatustextHandler::Statustext&)> callback;
        void* cookie;
    };
    std::vector<StatustextCallback> _statustext_handler_callbacks;

    std::atomic<bool> _armed{false};
    std::atomic<bool> _hitl_enabled{false};

    std::atomic<Autopilot> _autopilot{Autopilot::Unknown};

    MavsdkImpl& _mavsdk_impl;

    std::thread* _system_thread{nullptr};
    std::atomic<bool> _should_exit{false};

    static constexpr double HEARTBEAT_TIMEOUT_S = 3.0;

    std::atomic<bool> _connected{false};
    CallbackList<bool> _is_connected_callbacks{};
    TimeoutHandler::Cookie _heartbeat_timeout_cookie{};

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

    MavlinkMissionTransferClient _mission_transfer_client;
    MavlinkRequestMessage _mavlink_request_message;
    MavlinkFtpClient _mavlink_ftp_client;
    MavlinkComponentMetadata _mavlink_component_metadata;

    std::mutex _plugin_impls_mutex{};
    std::vector<PluginImplBase*> _plugin_impls{};

    // We used set to maintain unique component ids
    mutable std::mutex _components_mutex{};
    std::unordered_set<uint8_t> _components{};

    std::unordered_map<const void*, ParamChangedCallback> _param_changed_callbacks{};

    MAV_TYPE _vehicle_type{MAV_TYPE::MAV_TYPE_GENERIC};

    std::atomic<FlightMode> _flight_mode{FlightMode::Unknown};

    std::mutex _mavlink_ftp_files_mutex{};
    std::unordered_map<std::string, std::string> _mavlink_ftp_files{};
};

} // namespace mavsdk
