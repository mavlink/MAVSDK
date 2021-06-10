#pragma once

#include <mutex>
#include <vector>
#include <atomic>

#include "call_every_handler.h"
#include "connection.h"
#include "mavsdk.h"
#include "mavlink_include.h"
#include "mavlink_address.h"
#include "safe_queue.h"
#include "system.h"
#include "timeout_handler.h"

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

    MavsdkImpl();
    ~MavsdkImpl();
    MavsdkImpl(const MavsdkImpl&) = delete;
    void operator=(const MavsdkImpl&) = delete;

    std::string version() const;

    void forward_message(mavlink_message_t& message, Connection* connection);
    void receive_message(mavlink_message_t& message, Connection* connection);
    bool send_message(mavlink_message_t& message);

    ConnectionResult
    add_any_connection(const std::string& connection_url, ForwardingOption forwarding_option);
    ConnectionResult
    add_link_connection(const std::string& protocol, const std::string& ip, int port);
    ConnectionResult add_udp_connection(
        const std::string& local_ip, int local_port_number, ForwardingOption forwarding_option);
    ConnectionResult add_tcp_connection(
        const std::string& remote_ip, int remote_port, ForwardingOption forwarding_option);
    ConnectionResult add_serial_connection(
        const std::string& dev_path,
        int baudrate,
        bool flow_control,
        ForwardingOption forwarding_option);
    ConnectionResult setup_udp_remote(
        const std::string& remote_ip, int remote_port, ForwardingOption forwarding_option);

    std::vector<std::shared_ptr<System>> systems() const;

    void set_configuration(Mavsdk::Configuration new_configuration);

    std::vector<uint64_t> get_system_uuids() const;
    System& get_system();
    System& get_system(uint64_t uuid);

    uint8_t get_own_system_id() const;
    uint8_t get_own_component_id() const;
    uint8_t get_mav_type() const;

    bool is_connected() const;
    bool is_connected(uint64_t uuid) const;

    void subscribe_on_new_system(Mavsdk::NewSystemCallback callback);
    void register_on_discover(Mavsdk::event_callback_t callback);
    void register_on_timeout(Mavsdk::event_callback_t callback);

    void notify_on_discover(uint64_t uuid);
    void notify_on_timeout(uint64_t uuid);

    void start_sending_heartbeats();
    void stop_sending_heartbeats();

    TimeoutHandler timeout_handler;
    CallEveryHandler call_every_handler;

    void call_user_callback_located(
        const std::string& filename, const int linenumber, const std::function<void()>& func);

    void set_timeout_s(double timeout_s) { _timeout_s = timeout_s; }

    double timeout_s() const { return _timeout_s; };

    MAVLinkAddress own_address{};

private:
    void add_connection(std::shared_ptr<Connection>);
    void make_system_with_component(
        uint8_t system_id, uint8_t component_id, bool always_connected = false);

    void work_thread();
    void process_user_callbacks_thread();

    void send_heartbeat();

    static uint8_t get_target_system_id(const mavlink_message_t& message);
    static uint8_t get_target_component_id(const mavlink_message_t& message);

    std::mutex _connections_mutex{};
    std::vector<std::shared_ptr<Connection>> _connections{};

    mutable std::recursive_mutex _systems_mutex{};

    std::vector<std::pair<uint8_t, std::shared_ptr<System>>> _systems{};

    std::mutex _new_system_callback_mutex{};
    Mavsdk::NewSystemCallback _new_system_callback{nullptr};

    Mavsdk::event_callback_t _on_discover_callback{nullptr};
    Mavsdk::event_callback_t _on_timeout_callback{nullptr};

    Time _time{};

    Mavsdk::Configuration _configuration{Mavsdk::Configuration::UsageType::GroundStation};

    struct UserCallback {
        UserCallback() {}
        UserCallback(const std::function<void()>& func_) : func(func_) {}
        UserCallback(
            const std::function<void()>& func_,
            const std::string& filename_,
            const int linenumber_) :
            func(func_),
            filename(filename_),
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

    std::atomic<double> _timeout_s{Mavsdk::DEFAULT_TIMEOUT_S};

    static constexpr double _HEARTBEAT_SEND_INTERVAL_S = 1.0;
    void* _heartbeat_send_cookie{nullptr};

    std::atomic<bool> _should_exit = {false};
};

} // namespace mavsdk
