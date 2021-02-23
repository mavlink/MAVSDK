#pragma once

#include <unordered_map>
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

    ConnectionResult add_any_connection(const std::string& connection_url, bool forward_messages);
    ConnectionResult
    add_link_connection(const std::string& protocol, const std::string& ip, int port);
    ConnectionResult
    add_udp_connection(const std::string& local_ip, int local_port_number, bool forward_messages);
    ConnectionResult
    add_tcp_connection(const std::string& remote_ip, int remote_port, bool forward_messages);
    ConnectionResult add_serial_connection(
        const std::string& dev_path, int baudrate, bool flow_control, bool forward_messages);
    ConnectionResult
    setup_udp_remote(const std::string& remote_ip, int remote_port, bool forward_messages);

    std::vector<std::shared_ptr<System>> systems() const;

    void set_configuration(Mavsdk::Configuration configuration);

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

    void start_sending_heartbeat();

    TimeoutHandler timeout_handler;
    CallEveryHandler call_every_handler;

    void call_user_callback_located(
        const std::string& filename, const int linenumber, const std::function<void()>& func);

    MAVLinkAddress own_address{};

private:
    void add_connection(std::shared_ptr<Connection>);
    void make_system_with_component(uint8_t system_id, uint8_t component_id);
    bool does_system_exist(uint8_t system_id);

    void work_thread();
    void process_user_callbacks_thread();

    void send_heartbeat();

    using system_entry_t = std::pair<uint8_t, std::shared_ptr<System>>;

    std::mutex _connections_mutex{};
    std::vector<std::shared_ptr<Connection>> _connections{};

    mutable std::recursive_mutex _systems_mutex{};
    std::unordered_map<uint8_t, std::shared_ptr<System>> _systems{};

    std::mutex _new_system_callback_mutex{};
    Mavsdk::NewSystemCallback _new_system_callback{nullptr};

    Mavsdk::event_callback_t _on_discover_callback{nullptr};
    Mavsdk::event_callback_t _on_timeout_callback{nullptr};

    Time _time{};

    Mavsdk::Configuration _configuration{Mavsdk::Configuration::UsageType::GroundStation};
    bool _is_single_system{false};

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
    bool _callback_debugging{false};

    static constexpr double _HEARTBEAT_SEND_INTERVAL_S = 1.0;
    std::atomic<bool> _sending_heartbeats{false};
    void* _heartbeat_send_cookie = nullptr;

    std::atomic<bool> _should_exit = {false};
};

} // namespace mavsdk
