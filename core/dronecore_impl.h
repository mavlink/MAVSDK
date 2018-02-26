#pragma once

#include <map>
#include <mutex>
#include <vector>

#include "connection.h"
#include "device.h"
#include "dronecore.h"
#include "mavlink_include.h"

namespace dronecore {

class DroneCoreImpl
{
public:
    DroneCoreImpl();
    ~DroneCoreImpl();

    void receive_message(const mavlink_message_t &message);
    bool send_message(const mavlink_message_t &message);
    DroneCore::ConnectionResult add_any_connection(const std::string &connection_url);
    DroneCore::ConnectionResult add_link_connection(const std::string &protocol, const std::string &ip,
                                                    int port);
    DroneCore::ConnectionResult add_udp_connection(int local_port_number);
    void add_connection(Connection *connection);
    DroneCore::ConnectionResult add_tcp_connection(const std::string &remote_ip, int remote_port);
    DroneCore::ConnectionResult add_serial_connection(const std::string &dev_path, int baudrate);

    const std::vector<uint64_t> &get_device_uuids() const;
    Device &get_device();
    Device &get_device(uint64_t uuid);

    bool is_connected() const;
    bool is_connected(uint64_t uuid) const;

    void register_on_discover(DroneCore::event_callback_t callback);
    void register_on_timeout(DroneCore::event_callback_t callback);

    void notify_on_discover(uint64_t uuid);
    void notify_on_timeout(uint64_t uuid);

private:
    void create_device_if_not_existing(uint8_t system_id);

    std::mutex _connections_mutex;
    std::vector<Connection *> _connections;

    mutable std::recursive_mutex _devices_mutex;
    std::map<uint8_t, Device *> _devices;

    DroneCore::event_callback_t _on_discover_callback;
    DroneCore::event_callback_t _on_timeout_callback;

    std::atomic<bool> _should_exit = {false};
};

} // namespace dronecore
