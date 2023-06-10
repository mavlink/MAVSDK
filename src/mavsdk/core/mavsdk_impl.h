#pragma once

#include <cstdint>
#include <mutex>
#include <utility>
#include <vector>
#include <atomic>
#include <thread>

#include "call_every_handler.h"
#include "connection.h"
#include "handle.h"
#include "mavsdk.h"
#include "mavlink_include.h"
#include "mavlink_address.h"
#include "mavlink_message_handler.h"
#include "mavlink_command_receiver.h"
#include "safe_queue.h"
#include "server_component.h"
#include "system.h"
#include "timeout_handler.h"
#include "callback_list.h"

namespace mavsdk {

class MavsdkImpl {
public:
    /** @brief Default System ID for GCS configuration type. */
    static constexpr int DEFAULT_SYSTEM_ID_GCS = 245;
    /** @brief Default Component ID for GCS configuration type. */
    static constexpr int DEFAULT_COMPONENT_ID_GCS = MAV_COMP_ID_MISSIONPLANNER;
    /** @brief Default System ID for CompanionComputer configuration type. */
    static constexpr int DEFAULT_SYSTEM_ID_CC = 1;
    /** @brief Default Component ID for CompanionComputer configuration type. */
    static constexpr int DEFAULT_COMPONENT_ID_CC = MAV_COMP_ID_PATHPLANNER;
    /** @brief Default System ID for Autopilot configuration type. */
    static constexpr int DEFAULT_SYSTEM_ID_AUTOPILOT = 1;
    /** @brief Default Component ID for Autopilot configuration type. */
    static constexpr int DEFAULT_COMPONENT_ID_AUTOPILOT = MAV_COMP_ID_AUTOPILOT1;
    /** @brief Default System ID for Camera configuration type. */
    static constexpr int DEFAULT_SYSTEM_ID_CAMERA = 1;
    /** @brief Default Component ID for Camera configuration type. */
    static constexpr int DEFAULT_COMPONENT_ID_CAMERA = MAV_COMP_ID_CAMERA;

    MavsdkImpl();
    ~MavsdkImpl();
    MavsdkImpl(const MavsdkImpl&) = delete;
    void operator=(const MavsdkImpl&) = delete;

    static std::string version();

    void forward_message(mavlink_message_t& message, Connection* connection);
    void receive_message(mavlink_message_t& message, Connection* connection);
    bool send_message(mavlink_message_t& message);

    std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
    add_any_connection(const std::string& connection_url, ForwardingOption forwarding_option);
    std::pair<ConnectionResult, Mavsdk::ConnectionHandle> add_udp_connection(
        const std::string& local_ip, int local_port_number, ForwardingOption forwarding_option);
    std::pair<ConnectionResult, Mavsdk::ConnectionHandle> add_tcp_connection(
        const std::string& remote_ip, int remote_port, ForwardingOption forwarding_option);
    std::pair<ConnectionResult, Mavsdk::ConnectionHandle> add_serial_connection(
        const std::string& dev_path,
        int baudrate,
        bool flow_control,
        ForwardingOption forwarding_option);
    std::pair<ConnectionResult, Mavsdk::ConnectionHandle> setup_udp_remote(
        const std::string& remote_ip, int remote_port, ForwardingOption forwarding_option);

    void remove_connection(Mavsdk::ConnectionHandle handle);

    std::vector<std::shared_ptr<System>> systems() const;

    std::optional<std::shared_ptr<System>> first_autopilot(double timeout_s);

    void set_configuration(Mavsdk::Configuration new_configuration);
    Mavsdk::Configuration get_configuration() const;

    uint8_t get_own_system_id() const;
    uint8_t get_own_component_id() const;
    uint8_t get_mav_type() const;

    Mavsdk::NewSystemHandle subscribe_on_new_system(const Mavsdk::NewSystemCallback& callback);
    void unsubscribe_on_new_system(Mavsdk::NewSystemHandle handle);

    void notify_on_discover();
    void notify_on_timeout();

    void start_sending_heartbeats();
    void stop_sending_heartbeats();

    void intercept_incoming_messages_async(std::function<bool(mavlink_message_t&)> callback);
    void intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback);

    std::shared_ptr<ServerComponent> server_component_by_type(
        Mavsdk::ServerComponentType server_component_type, unsigned instance = 0);
    std::shared_ptr<ServerComponent> server_component_by_id(uint8_t component_id);

    TimeoutHandler timeout_handler;
    CallEveryHandler call_every_handler;

    void call_user_callback_located(
        const std::string& filename, int linenumber, const std::function<void()>& func);

    void set_timeout_s(double timeout_s) { _timeout_s = timeout_s; }

    double timeout_s() const { return _timeout_s; };

    MavlinkMessageHandler mavlink_message_handler{};
    Time time{};

private:
    Mavsdk::ConnectionHandle add_connection(const std::shared_ptr<Connection>&);
    void make_system_with_component(uint8_t system_id, uint8_t component_id);

    void work_thread();
    void process_user_callbacks_thread();

    void send_heartbeat();
    bool is_any_system_connected() const;

    static uint8_t get_target_system_id(const mavlink_message_t& message);
    static uint8_t get_target_component_id(const mavlink_message_t& message);

    std::mutex _connections_mutex{};
    uint64_t _connections_handle_id{1};
    struct ConnectionEntry {
        std::shared_ptr<Connection> connection;
        Handle<> handle;
    };
    std::vector<ConnectionEntry> _connections{};

    mutable std::recursive_mutex _systems_mutex{};
    std::vector<std::pair<uint8_t, std::shared_ptr<System>>> _systems{};

    mutable std::mutex _server_components_mutex{};
    std::vector<std::pair<uint8_t, std::shared_ptr<ServerComponent>>> _server_components{};
    std::shared_ptr<ServerComponent> _default_server_component{nullptr};

    CallbackList<> _new_system_callbacks{};

    Time _time{};

    Mavsdk::Configuration _configuration{Mavsdk::Configuration::UsageType::GroundStation};

    struct UserCallback {
        UserCallback() = default;
        explicit UserCallback(std::function<void()> func_) : func(std::move(func_)) {}
        UserCallback(std::function<void()> func_, std::string filename_, const int linenumber_) :
            func(std::move(func_)),
            filename(std::move(filename_)),
            linenumber(linenumber_)
        {}

        std::function<void()> func{};
        std::string filename{};
        int linenumber{};
    };

    std::thread* _work_thread{nullptr};
    std::thread* _process_user_callbacks_thread{nullptr};
    SafeQueue<UserCallback> _user_callback_queue{};

    bool _message_logging_on{false};
    bool _callback_debugging{false};

    mutable std::mutex _intercept_callback_mutex{};
    std::function<bool(mavlink_message_t&)> _intercept_incoming_messages_callback{nullptr};
    std::function<bool(mavlink_message_t&)> _intercept_outgoing_messages_callback{nullptr};

    std::atomic<double> _timeout_s{Mavsdk::DEFAULT_TIMEOUT_S};

    static constexpr double HEARTBEAT_SEND_INTERVAL_S = 1.0;
    void* _heartbeat_send_cookie{nullptr};

    std::atomic<bool> _should_exit = {false};
};

} // namespace mavsdk
