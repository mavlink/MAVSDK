#include "dronecore_impl.h"

#include <mutex>

#include "connection.h"
#include "global_include.h"
#include "tcp_connection.h"
#include "udp_connection.h"

#ifndef WINDOWS
#include "serial_connection.h"
#endif

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

        _devices.clear();
    }

    {
        std::lock_guard<std::mutex> lock(_connections_mutex);
        _connections.clear();
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
        _devices.insert(system_entry_t(message.sysid, null_device));
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

ConnectionResult DroneCoreImpl::add_any_connection(const std::string &connection_url)
{
    std::string delimiter = "://";
    std::string conn_url(connection_url);
    std::vector<std::string> connection_str;
    size_t pos = 0;
    /*Parse the connection url to get Protocol, IP or Serial dev node
      and port number or baudrate as per the URL definition
    */
    for (int i = 0; i < 2; i++) {
        pos = conn_url.find(delimiter);
        if (pos != std::string::npos) {
            connection_str.push_back(conn_url.substr(0, pos));
            // Erase the string which is parsed already
            conn_url.erase(0, pos + delimiter.length());
            if (conn_url == "") {
                break;
            }
            delimiter = ":";
        }
    }
    connection_str.push_back(conn_url);
    /* check if the protocol is Network protocol or Serial */
    if (connection_str.at(0) != "serial") {
        int port = 0;
        if (connection_str.at(2) != "") {
            port = std::stoi(connection_str.at(2));
        }
        return add_link_connection(connection_str.at(0), connection_str.at(1),
                                   port);
    } else {
        if (connection_str.at(1) == "") {
            return add_serial_connection(DroneCore::DEFAULT_SERIAL_DEV_PATH,
                                         DroneCore::DEFAULT_SERIAL_BAUDRATE);
        } else {
            return add_serial_connection(connection_str.at(1), std::stoi(connection_str.at(2)));
        }
    }
}

ConnectionResult DroneCoreImpl::add_link_connection(const std::string &protocol,
                                                    const std::string &ip, const int port)
{
    int local_port_number = 0;
    std::string local_ip = ip;
    if (port == 0) {
        if (ip != "") {
            local_port_number = std::stoi(ip);
            /* default ip for tcp if only port number is specified */
            local_ip = "127.0.0.1";
        }
    } else {
        local_port_number = port;
    }

    if (protocol == "udp") {
        return add_udp_connection(local_port_number);
    } else { //TCP connection
        return add_tcp_connection(local_ip, local_port_number);
    }
}

ConnectionResult DroneCoreImpl::add_udp_connection(const int local_port_number)
{
    std::shared_ptr<Connection> new_conn = std::make_shared<UdpConnection>(*this, local_port_number);

    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

void DroneCoreImpl::add_connection(std::shared_ptr<Connection> new_connection)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);
    _connections.push_back(new_connection);
}

ConnectionResult DroneCoreImpl::add_tcp_connection(const std::string &remote_ip,
                                                   const int remote_port)
{

    std::shared_ptr<Connection> new_conn = std::make_shared<TcpConnection>(*this, remote_ip,
                                                                           remote_port);

    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

ConnectionResult DroneCoreImpl::add_serial_connection(const std::string &dev_path,
                                                      const int baudrate)
{
#if !defined(WINDOWS) && !defined(APPLE)
    std::shared_ptr<Connection> new_conn = std::make_shared<SerialConnection>(*this,
                                                                              dev_path, baudrate);
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
#else
    UNUSED(dev_path);
    UNUSED(baudrate);
    return ConnectionResult::NOT_IMPLEMENTED;
#endif
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

Device &DroneCoreImpl::get_device(const uint64_t uuid)
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

bool DroneCoreImpl::is_connected(const uint64_t uuid) const
{
    std::lock_guard<std::recursive_mutex> lock(_devices_mutex);

    for (auto it = _devices.begin(); it != _devices.end(); ++it) {
        if (it->second->get_target_uuid() == uuid) {
            return it->second->is_connected();
        }
    }
    return false;
}

void DroneCoreImpl::create_device_if_not_existing(const uint8_t system_id)
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

    // Make new device
    auto new_device = std::make_shared<Device>(*this, system_id);
    _devices.insert(system_entry_t(system_id, new_device));
}

void DroneCoreImpl::notify_on_discover(const uint64_t uuid)
{
    LogDebug() << "Discovered " << uuid;
    if (_on_discover_callback != nullptr) {
        _on_discover_callback(uuid);
    }
}

void DroneCoreImpl::notify_on_timeout(const uint64_t uuid)
{
    LogDebug() << "Lost " << uuid;
    if (_on_timeout_callback != nullptr) {
        _on_timeout_callback(uuid);
    }
}

void DroneCoreImpl::register_on_discover(const DroneCore::event_callback_t callback)
{
    _on_discover_callback = callback;
}

void DroneCoreImpl::register_on_timeout(const DroneCore::event_callback_t callback)
{
    _on_timeout_callback = callback;
}

} // namespace dronecore
