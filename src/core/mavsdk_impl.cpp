#include "mavsdk_impl.h"

#include <mutex>

#include "connection.h"
#include "global_include.h"
#include "tcp_connection.h"
#include "udp_connection.h"
#include "system.h"
#include "system_impl.h"
#include "serial_connection.h"
#include "cli_arg.h"
#include "version.h"

namespace mavsdk {

MavsdkImpl::MavsdkImpl() :
    _connections_mutex(),
    _connections(),
    _systems_mutex(),
    _systems(),
    _on_discover_callback(nullptr),
    _on_timeout_callback(nullptr),
    _configuration(Mavsdk::Configuration::UsageType::GroundStation)
{
    LogInfo() << "MAVSDK version: " << mavsdk_version;
    set_configuration(_configuration);
}

MavsdkImpl::~MavsdkImpl()
{
    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
        _should_exit = true;

        _systems.clear();
    }

    {
        std::lock_guard<std::mutex> lock(_connections_mutex);
        _connections.clear();
    }
}

std::string MavsdkImpl::version() const
{
    static unsigned version_counter = 0;

    ++version_counter;

    switch (version_counter) {
        case 10:
            return "You were wondering about the name of this library?";
        case 11:
            return "Let's look at the history:";
        case 12:
            return "DroneLink";
        case 13:
            return "DroneCore";
        case 14:
            return "DronecodeSDK";
        case 15:
            return "MAVSDK";
        case 16:
            return "And that's it...";
        case 17:
            return "At least for now ¯\\_(ツ)_/¯.";
        default:
            return mavsdk_version;
    }
}

void MavsdkImpl::receive_message(mavlink_message_t& message)
{
    // Don't ever create a system with sysid 0.
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

    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    // Change system id of null system
    if (_systems.find(0) != _systems.end()) {
        auto null_system = _systems[0];
        _systems.erase(0);
        null_system->system_impl()->set_system_id(message.sysid);
        _systems.insert(system_entry_t(message.sysid, null_system));
    } else if (_is_single_system) {
        auto sys = _systems.begin();
        if (sys->first != message.sysid) {
            sys->second->system_impl()->set_system_id(message.sysid);
            _systems.insert(system_entry_t(message.sysid, sys->second));
            _systems.erase(sys->first);
        }
    }

    if (!does_system_exist(message.sysid)) {
        make_system_with_component(message.sysid, message.compid);
    } else {
        _systems.at(message.sysid)->system_impl()->add_new_component(message.compid);
    }

    if (_should_exit) {
        // Don't try to call at() if systems have already been destroyed
        // in descructor.
        return;
    }

    if (_systems.find(message.sysid) != _systems.end()) {
        _systems.at(message.sysid)->system_impl()->process_mavlink_message(message);
    }
}

bool MavsdkImpl::send_message(mavlink_message_t& message)
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

ConnectionResult MavsdkImpl::add_any_connection(const std::string& connection_url)
{
    CliArg cli_arg;
    if (!cli_arg.parse(connection_url)) {
        return ConnectionResult::CONNECTION_URL_INVALID;
    }

    switch (cli_arg.get_protocol()) {
        case CliArg::Protocol::UDP: {
            std::string path = Mavsdk::DEFAULT_UDP_BIND_IP;
            int port = Mavsdk::DEFAULT_UDP_PORT;
            if (!cli_arg.get_path().empty()) {
                path = cli_arg.get_path();
            }
            if (cli_arg.get_port()) {
                port = cli_arg.get_port();
            }
            return add_udp_connection(path, port);
        }

        case CliArg::Protocol::TCP: {
            std::string path = Mavsdk::DEFAULT_TCP_REMOTE_IP;
            int port = Mavsdk::DEFAULT_TCP_REMOTE_PORT;
            if (!cli_arg.get_path().empty()) {
                path = cli_arg.get_path();
            }
            if (cli_arg.get_port()) {
                port = cli_arg.get_port();
            }
            return add_tcp_connection(path, port);
        }

        case CliArg::Protocol::SERIAL: {
            int baudrate = Mavsdk::DEFAULT_SERIAL_BAUDRATE;
            if (cli_arg.get_baudrate()) {
                baudrate = cli_arg.get_baudrate();
            }
            return add_serial_connection(cli_arg.get_path(), baudrate);
        }

        default:
            return ConnectionResult::CONNECTION_ERROR;
    }
}

ConnectionResult MavsdkImpl::add_udp_connection(const std::string& local_ip, const int local_port)
{
    auto new_conn = std::make_shared<UdpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1), local_ip, local_port);
    if (!new_conn) {
        return ConnectionResult::CONNECTION_ERROR;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

ConnectionResult MavsdkImpl::setup_udp_remote(const std::string& remote_ip, int remote_port)
{
    auto new_conn = std::make_shared<UdpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1), "0.0.0.0", 0);
    if (!new_conn) {
        return ConnectionResult::CONNECTION_ERROR;
    }
    ConnectionResult ret = new_conn->start();
    _is_single_system = true;
    if (ret == ConnectionResult::SUCCESS) {
        new_conn->add_remote(remote_ip, remote_port);
        add_connection(new_conn);
        make_system_with_component(get_own_system_id(), get_own_component_id());
    }
    return ret;
}

ConnectionResult MavsdkImpl::add_tcp_connection(const std::string& remote_ip, int remote_port)
{
    auto new_conn = std::make_shared<TcpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1),
        remote_ip,
        remote_port);
    if (!new_conn) {
        return ConnectionResult::CONNECTION_ERROR;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

ConnectionResult MavsdkImpl::add_serial_connection(const std::string& dev_path, int baudrate)
{
    auto new_conn = std::make_shared<SerialConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1), dev_path, baudrate);
    if (!new_conn) {
        return ConnectionResult::CONNECTION_ERROR;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::SUCCESS) {
        add_connection(new_conn);
    }
    return ret;
}

void MavsdkImpl::add_connection(std::shared_ptr<Connection> new_connection)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);
    _connections.push_back(new_connection);
}

void MavsdkImpl::set_configuration(Mavsdk::Configuration configuration)
{
    own_address.system_id = configuration.get_system_id();
    own_address.component_id = configuration.get_component_id();
}

std::vector<uint64_t> MavsdkImpl::get_system_uuids() const
{
    std::vector<uint64_t> uuids = {};

    for (auto it = _systems.begin(); it != _systems.end(); ++it) {
        uint64_t uuid = it->second->_system_impl->get_uuid();
        if (uuid != 0) {
            uuids.push_back(uuid);
        }
    }

    return uuids;
}

System& MavsdkImpl::get_system()
{
    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
        // In get_system without uuid, we expect to have only
        // one system connected.
        if (_systems.size() == 1) {
            return *(_systems.at(_systems.begin()->first));
        }

        if (_systems.size() > 1) {
            LogErr() << "More than one system found:";

            // Just return first system instead of failing.
            return *_systems.begin()->second;
        } else {
            uint8_t system_id = 0, comp_id = 0;
            make_system_with_component(system_id, comp_id);
            return *_systems[system_id];
        }
    }
}

System& MavsdkImpl::get_system(const uint64_t uuid)
{
    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
        // TODO: make a cache map for this.
        for (auto system : _systems) {
            if (system.second->get_uuid() == uuid) {
                return *system.second;
            }
        }
    }

    // We have not found a system with this UUID.
    // TODO: this is an error condition that we ought to handle properly.
    LogErr() << "System with UUID: " << uuid << " not found";

    // Create a dummy
    uint8_t system_id = 0, comp_id = 0;
    make_system_with_component(system_id, comp_id);

    return *_systems[system_id];
}

uint8_t MavsdkImpl::get_own_system_id() const
{
    // TODO: To be deprecated.
    return own_address.system_id;
}

uint8_t MavsdkImpl::get_own_component_id() const
{
    // TODO: To be deprecated.
    return own_address.component_id;
}

uint8_t MavsdkImpl::get_mav_type() const
{
    switch (_configuration.get_usage_type()) {
        case Mavsdk::Configuration::UsageType::Autopilot:
            return MAV_TYPE_GENERIC;

        case Mavsdk::Configuration::UsageType::GroundStation:
            return MAV_TYPE_GCS;

        case Mavsdk::Configuration::UsageType::CompanionComputer:
            return MAV_TYPE_ONBOARD_CONTROLLER;

        case Mavsdk::Configuration::UsageType::Custom:
            return MAV_TYPE_GENERIC;

        default:
            LogErr() << "Unknown configuration";
            return 0;
    }
}

bool MavsdkImpl::is_connected() const
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (_systems.empty()) {
        return false;
    }

    return _systems.begin()->second->is_connected();
}

bool MavsdkImpl::is_connected(const uint64_t uuid) const
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    for (auto it = _systems.begin(); it != _systems.end(); ++it) {
        if (it->second->get_uuid() == uuid) {
            return it->second->is_connected();
        }
    }
    return false;
}

void MavsdkImpl::make_system_with_component(uint8_t system_id, uint8_t comp_id)
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (_should_exit) {
        // When the system got destroyed in the destructor, we have to give up.
        return;
    }

    LogDebug() << "New: System ID: " << int(system_id) << " Comp ID: " << int(comp_id);
    // Make a system with its first component
    auto new_system = std::make_shared<System>(*this, system_id, comp_id, _is_single_system);

    _systems.insert(system_entry_t(system_id, new_system));
}

bool MavsdkImpl::does_system_exist(uint8_t system_id)
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (!_should_exit) {
        return (_systems.find(system_id) != _systems.end());
    }
    // When the system got destroyed in the destructor, we have to give up.
    return false;
}

void MavsdkImpl::notify_on_discover(const uint64_t uuid)
{
    if (_on_discover_callback != nullptr) {
        _on_discover_callback(uuid);
    }
}

void MavsdkImpl::notify_on_timeout(const uint64_t uuid)
{
    LogDebug() << "Lost " << uuid;
    if (_on_timeout_callback != nullptr) {
        _on_timeout_callback(uuid);
    }
}

void MavsdkImpl::register_on_discover(const Mavsdk::event_callback_t callback)
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (callback) {
        for (auto const& connected_system : _systems) {
            // Ignore dummy system with system ID 0.
            if (connected_system.first == 0) {
                continue;
            }
            // Ignore system if UUID is not initialized yet.
            if (connected_system.second->get_uuid() == 0) {
                continue;
            }
            callback(connected_system.second->get_uuid());
        }
    }

    _on_discover_callback = callback;
}

void MavsdkImpl::register_on_timeout(const Mavsdk::event_callback_t callback)
{
    _on_timeout_callback = callback;
}

} // namespace mavsdk
