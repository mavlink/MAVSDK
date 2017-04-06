#include "dronelink_impl.h"
#include "global_include.h"
#include <mutex>

namespace dronelink {

DroneLinkImpl::DroneLinkImpl() :
    _connections_mutex(),
    _connections(),
    _devices_mutex(),
    _devices(),
    _device_impls(),
    _on_discover_callback(nullptr),
    _on_timeout_callback(nullptr)
{}

DroneLinkImpl::~DroneLinkImpl()
{
    {
        std::lock_guard<std::mutex> lock(_devices_mutex);
        _should_exit = true;

        for (auto it = _devices.begin(); it != _devices.end(); ++it) {
            delete it->second;
        }

        for (auto it = _device_impls.begin(); it != _device_impls.end(); ++it) {
            delete it->second;
        }
        _devices.clear();
        _device_impls.clear();
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

void DroneLinkImpl::receive_message(const mavlink_message_t &message)
{
    // Don't ever create a device with sysid 0.
    if (message.sysid == 0) {
        return;
    }

    create_device_if_not_existing(message.sysid, message.compid);

    remove_empty_devices();

    if (_should_exit) {
        // Don't try to call at() if devices have already been destroyed
        // in descructor.
        return;
    }

    _devices_mutex.lock();

    if (message.sysid != 1) {
        Debug() << "sysid: " << int(message.sysid);
    }
    auto &device_impl = _device_impls.at(message.sysid);

    // We need to release the lock, otherwise we can't send
    // messages inside process_mavlink_message.
    _devices_mutex.unlock();

    device_impl->process_mavlink_message(message);
}

bool DroneLinkImpl::send_message(const mavlink_message_t &message)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);

    for (auto it = _connections.begin(); it != _connections.end(); ++it) {
        if (!(**it).send_message(message)) {
            Debug() << "send fail";
            return false;
        }
    }

    return true;
}

void DroneLinkImpl::add_connection(Connection *new_connection)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);
    _connections.push_back(new_connection);
}

const std::vector<uint64_t> &DroneLinkImpl::get_device_uuids() const
{
    // This needs to survive the scope but we need to clean it up.
    static std::vector<uint64_t> uuids = {};
    uuids.clear();

    for (auto it = _device_impls.begin(); it != _device_impls.end(); ++it) {
        uint64_t uuid = it->second->get_target_uuid();
        if (uuid != 0) {
            uuids.push_back(uuid);
        }
    }

    return uuids;
}

Device &DroneLinkImpl::get_device()
{
    {
        std::lock_guard<std::mutex> lock(_devices_mutex);
        // In get_device withoiut uuid, we expect to have only
        // one device conneted.
        if (_device_impls.size() == 1) {
            return *(_devices.at(_device_impls.begin()->first));
        }

        if (_device_impls.size() > 1) {
            Debug() << "Error: more than one device found:";

            int i = 0;
            for (auto it = _device_impls.begin(); it != _device_impls.end(); ++it) {
                Debug() << "strange: " << i << ": " << int(it->first) << ", " << it->second;
                ++i;
            }

        } else {
            Debug() << "Error: no device found.";
        }
    }

    // In both failure cases, return a dummy.
    uint8_t system_id = 0;
    uint8_t component_id = 0;
    create_device_if_not_existing(system_id, component_id);

    return *_devices[system_id];
}

Device &DroneLinkImpl::get_device(uint64_t uuid)
{
    {
        std::lock_guard<std::mutex> lock(_devices_mutex);
        // TODO: make a cache map for this.
        for (auto it = _device_impls.begin(); it != _device_impls.end(); ++it) {
            if (it->second->get_target_uuid() == uuid) {
                return *(_devices.at(it->first));
            }
        }
    }

    // We have not found a device with this UUID.
    // TODO: this is an error condition that we ought to handle properly.
    Debug() << "device with UUID: " << uuid << " not found";

    // Create a dummy
    uint8_t system_id = 0;
    uint8_t component_id = 0;
    create_device_if_not_existing(system_id, component_id);

    return *_devices[system_id];
}

void DroneLinkImpl::create_device_if_not_existing(uint8_t system_id, uint8_t component_id)
{
    std::lock_guard<std::mutex> lock(_devices_mutex);

    if (_should_exit) {
        // When the device_impl got destroyed in the destructor, we have to give up.
        return;
    }

    // existing already.
    if (_devices.find(system_id) != _devices.end()) {
        //Debug() << "ID: " << int(system_id) << " exists already.";
        return;
    }

    // Create both lists in parallel
    DeviceImpl *new_device_impl = new DeviceImpl(this, system_id, component_id);
    _device_impls.insert(std::pair<uint8_t, DeviceImpl *>(system_id, new_device_impl));

    Device *new_device = new Device(new_device_impl);
    _devices.insert(std::pair<uint8_t, Device *>(system_id, new_device));
}

void DroneLinkImpl::remove_empty_devices()
{
    std::lock_guard<std::mutex> lock(_devices_mutex);

    // Remove entries with system and component ID 0.
    if (_devices.find(0) != _devices.end()) {
        _devices.erase(0);
    }
    if (_device_impls.find(0) != _device_impls.end()) {
        _device_impls.erase(0);
    }
}

void DroneLinkImpl::notify_on_discover(uint64_t uuid)
{
    if (_on_discover_callback != nullptr) {
        _on_discover_callback(uuid);
    }
}

void DroneLinkImpl::notify_on_timeout(uint64_t uuid)
{
    if (_on_timeout_callback != nullptr) {
        _on_timeout_callback(uuid);
    }
}

void DroneLinkImpl::register_on_discover(DroneLink::event_callback_t callback)
{
    _on_discover_callback = callback;
}

void DroneLinkImpl::register_on_timeout(DroneLink::event_callback_t callback)
{
    _on_timeout_callback = callback;
}

} // namespace dronelink
