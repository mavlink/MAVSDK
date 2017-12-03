#pragma once

#include "dronecore.h"
#include "connection.h"
#include "device_impl.h"
#include "mavlink_include.h"
#include <vector>
#include <map>
#include <mutex>


namespace dronecore {

class DroneCoreImpl
{
public:
    DroneCoreImpl();
    ~DroneCoreImpl();

    void receive_message(const mavlink_message_t &message);
    bool send_message(const mavlink_message_t &message);
    void add_connection(Connection *connection);

    const std::vector<uint64_t> &get_device_uuids() const;
    DeviceImpl &get_device();
    DeviceImpl &get_device(uint64_t uuid);

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
    std::map<uint8_t, DeviceImpl *> _device_impls;

    DroneCore::event_callback_t _on_discover_callback;
    DroneCore::event_callback_t _on_timeout_callback;

    std::atomic<bool> _should_exit = {false};
};

} // namespace dronecore
