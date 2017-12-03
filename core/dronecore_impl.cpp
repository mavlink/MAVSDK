#include "dronecore_impl.h"
#include "global_include.h"
#include <mutex>

namespace dronecore {

DroneCoreImpl::DroneCoreImpl() :
    _connections_mutex(),
    _connections(),
    _devices_mutex(),
    _devices(),
    _on_discover_callback(nullptr),
    _on_timeout_callback(nullptr)
{}

DroneCoreImpl::~DroneCoreImpl()
{
    {
        std::lock_guard<std::recursive_mutex> lock(_devices_mutex);
        _should_exit = true;

        for (auto it = _devices.begin(); it != _devices.end(); ++it) {
            delete it->second;
        }
        _devices.clear();
    }

    std::vector<Connection *> tmp_connections;
    {
        std::lock_guard<std::mutex> lock(_connections_mutex);

        // We need to copy the connections to a temporary vector. This way they won't
        // get used anymore while they are cleaned up.
        tmp_connections = _connections;
        _connections.clear();
    }

    for (auto connection : tmp_connections) {
        delete connection;
    }

}

void DroneCoreImpl::receive_message(const mavlink_message_t &message)
{
    // Don't ever create a device with sysid 0.
    if (message.sysid == 0) {
        return;
    }

    // FIXME: Ignore messages from QGroundControl for now. Usually QGC identifies
    //        itself with sysid 255.
    //        A better way would probably be to parse the heartbeat message and
    //        look at type and check if it is MAV_TYPE_GCS.
    if (message.sysid == 255) {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(_devices_mutex);

    // Change system id of null device
    if (_devices.find(0) != _devices.end()) {
        auto null_device = _devices[0];
        _devices.erase(0);
        null_device->set_target_system_id(message.sysid);
        _devices.insert(std::pair<uint8_t, Device *>(message.sysid, null_device));
    }

    create_device_if_not_existing(message.sysid);

    if (_should_exit) {
        // Don't try to call at() if devices have already been destroyed
        // in descructor.
        return;
    }

    if (message.sysid != 1) {
        LogDebug() << "sysid: " << int(message.sysid);
    }

    if (_devices.find(message.sysid) != _devices.end()) {
        _devices.at(message.sysid)->process_mavlink_message(message);
    }
}

bool DroneCoreImpl::send_message(const mavlink_message_t &message)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);

    for (auto it = _connections.begin(); it != _connections.end(); ++it) {
        if (!(**it).send_message(message)) {
            LogErr() << "send fail";
            return false;
        }
    }

    return true;
}

void DroneCoreImpl::add_connection(Connection *new_connection)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);
    _connections.push_back(new_connection);
}

const std::vector<uint64_t> &DroneCoreImpl::get_device_uuids() const
{
    // This needs to survive the scope but we need to clean it up.
    static std::vector<uint64_t> uuids = {};
    uuids.clear();

    for (auto it = _devices.begin(); it != _devices.end(); ++it) {
        uint64_t uuid = it->second->get_target_uuid();
        if (uuid != 0) {
            uuids.push_back(uuid);
        }
    }

    return uuids;
}

Device &DroneCoreImpl::get_device()
{
    {
        std::lock_guard<std::recursive_mutex> lock(_devices_mutex);
        // In get_device withoiut uuid, we expect to have only
        // one device conneted.
        if (_devices.size() == 1) {
            return *(_devices.at(_devices.begin()->first));
        }

        if (_devices.size() > 1) {
            LogErr() << "Error: more than one device found:";

            int i = 0;
            for (auto it = _devices.begin(); it != _devices.end(); ++it) {
                LogWarn() << "strange: " << i << ": " << int(it->first) << ", " << it->second;
                ++i;
            }

            // Just return first device instead of failing.
            return *_devices.begin()->second;
        } else {
            LogErr() << "Error: no device found.";
            uint8_t system_id = 0;
            create_device_if_not_existing(system_id);
            return *_devices[system_id];
        }
    }
}

Device &DroneCoreImpl::get_device(uint64_t uuid)
{
    {
        std::lock_guard<std::recursive_mutex> lock(_devices_mutex);
        // TODO: make a cache map for this.
        for (auto device : _devices) {
            if (device.second->get_target_uuid() == uuid) {
                return *device.second;
            }
        }
    }

    // We have not found a device with this UUID.
    // TODO: this is an error condition that we ought to handle properly.
    LogErr() << "device with UUID: " << uuid << " not found";

    // Create a dummy
    uint8_t system_id = 0;
    create_device_if_not_existing(system_id);

    return *_devices[system_id];
}

bool DroneCoreImpl::is_connected() const
{
    std::lock_guard<std::recursive_mutex> lock(_devices_mutex);

    if (_devices.size() == 1) {
        return _devices.begin()->second->is_connected();
    }
    return false;
}

bool DroneCoreImpl::is_connected(uint64_t uuid) const
{
    std::lock_guard<std::recursive_mutex> lock(_devices_mutex);

    for (auto it = _devices.begin(); it != _devices.end(); ++it) {
        if (it->second->get_target_uuid() == uuid) {
            return it->second->is_connected();
        }
    }
    return false;
}

void DroneCoreImpl::create_device_if_not_existing(uint8_t system_id)
{
    std::lock_guard<std::recursive_mutex> lock(_devices_mutex);

    if (_should_exit) {
        // When the device got destroyed in the destructor, we have to give up.
        return;
    }

    // existing already.
    if (_devices.find(system_id) != _devices.end()) {
        //LogDebug() << "ID: " << int(system_id) << " exists already.";
        return;
    }

    // Create both lists in parallel
    Device *new_device = new Device(this, system_id);
    _devices.insert(std::pair<uint8_t, Device *>(system_id, new_device));
}

void DroneCoreImpl::notify_on_discover(uint64_t uuid)
{
    LogDebug() << "Discovered " << uuid;
    if (_on_discover_callback != nullptr) {
        _on_discover_callback(uuid);
    }
}

void DroneCoreImpl::notify_on_timeout(uint64_t uuid)
{
    LogDebug() << "Lost " << uuid;
    if (_on_timeout_callback != nullptr) {
        _on_timeout_callback(uuid);
    }
}

void DroneCoreImpl::register_on_discover(DroneCore::event_callback_t callback)
{
    _on_discover_callback = callback;
}

void DroneCoreImpl::register_on_timeout(DroneCore::event_callback_t callback)
{
    _on_timeout_callback = callback;
}

} // namespace dronecore
