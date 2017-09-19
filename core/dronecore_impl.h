#pragma once

#include "dronecore.h"
#include "connection.h"
#include "device.h"
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

    const std::vector<uint8_t> &get_device_system_ids() const;
    Device &get_device();
    Device &get_device(uint8_t system_id);

    void register_on_discover(DroneCore::event_callback_t callback);
    void register_on_timeout(DroneCore::event_callback_t callback);

    void notify_on_discover(uint8_t system_id);
    void notify_on_timeout(uint8_t system_id);

private:
    void create_device_if_not_existing(uint8_t system_id);

    std::mutex _connections_mutex;
    std::vector<Connection *> _connections;

    std::recursive_mutex _devices_mutex;
    std::map<uint8_t, Device *> _devices;
    std::map<uint8_t, DeviceImpl *> _device_impls;

    DroneCore::event_callback_t _on_discover_callback;
    DroneCore::event_callback_t _on_timeout_callback;

    std::atomic<bool> _should_exit = {false};
};

} // namespace dronecore
