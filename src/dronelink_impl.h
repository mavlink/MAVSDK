#pragma once

#include "dronelink.h"
#include "connection.h"
#include "device.h"
#include "device_impl.h"
#include "mavlink_include.h"
#include <vector>
#include <map>
#include <mutex>


namespace dronelink {

class DroneLinkImpl
{
public:
    DroneLinkImpl();
    ~DroneLinkImpl();

    void receive_message(const mavlink_message_t &message);
    Result send_message(const mavlink_message_t &message);
    void add_connection(Connection *connection);

    const std::vector<uint64_t> &get_device_uuids() const;
    Device &get_device(uint64_t uuid);

    static constexpr int DEFAULT_UDP_RECEIVE_PORT = 14540;

private:
    void create_device_if_not_existing(uint8_t system_id);

    std::mutex _connections_mutex;
    std::vector<Connection *> _connections;

    std::mutex _devices_mutex;
    std::map<uint8_t, Device *> _devices;
    std::map<uint8_t, DeviceImpl *> _device_impls;
};

} // namespace dronelink
