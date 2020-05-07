#pragma once

#include <map>
#include <mutex>
#include <vector>
#include <atomic>

#include "connection.h"
#include "mavsdk.h"
#include "system.h"
#include "mavlink_include.h"
#include "mavlink_address.h"

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

    std::string version() const;

    void receive_message(mavlink_message_t& message);
    bool send_message(mavlink_message_t& message);

    ConnectionResult add_any_connection(const std::string& connection_url);
    ConnectionResult
    add_link_connection(const std::string& protocol, const std::string& ip, int port);
    ConnectionResult add_udp_connection(const std::string& local_ip, int local_port_number);
    ConnectionResult add_tcp_connection(const std::string& remote_ip, int remote_port);
    ConnectionResult add_serial_connection(const std::string& dev_path, int baudrate);
    ConnectionResult setup_udp_remote(const std::string& remote_ip, int remote_port);

    void set_configuration(Mavsdk::Configuration configuration);

    std::vector<uint64_t> get_system_uuids() const;
    System& get_system();
    System& get_system(uint64_t uuid);

    uint8_t get_own_system_id() const;
    uint8_t get_own_component_id() const;
    uint8_t get_mav_type() const;

    bool is_connected() const;
    bool is_connected(uint64_t uuid) const;

    void register_on_discover(Mavsdk::event_callback_t callback);
    void register_on_timeout(Mavsdk::event_callback_t callback);

    void notify_on_discover(uint64_t uuid);
    void notify_on_timeout(uint64_t uuid);

    MAVLinkAddress own_address{};

private:
    void add_connection(std::shared_ptr<Connection>);
    void make_system_with_component(uint8_t system_id, uint8_t component_id);
    bool does_system_exist(uint8_t system_id);

    using system_entry_t = std::pair<uint8_t, std::shared_ptr<System>>;

    std::mutex _connections_mutex;
    std::vector<std::shared_ptr<Connection>> _connections;

    mutable std::recursive_mutex _systems_mutex;
    std::map<uint8_t, std::shared_ptr<System>> _systems;

    Mavsdk::event_callback_t _on_discover_callback;
    Mavsdk::event_callback_t _on_timeout_callback;

    Mavsdk::Configuration _configuration;
    bool _is_single_system{false};

    std::atomic<bool> _should_exit = {false};
};

} // namespace mavsdk
