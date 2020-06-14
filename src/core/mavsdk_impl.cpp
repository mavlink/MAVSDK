#include "mavsdk_impl.h"

#include <mutex>

#include "connection.h"
#include "global_include.h"
#include "tcp_connection.h"
#include "udp_connection.h"
#include "system.h"
#include "system_impl.h"
#include "node.h"
#include "node_impl.h"
#include "autopilot_interface.h"
#include "autopilot_interface_impl.h"
#include "serial_connection.h"
#include "cli_arg.h"
#include "version.h"

namespace mavsdk {

MavsdkImpl::MavsdkImpl() :
    _connections_mutex(),
    _connections(),
    _systems_mutex(),
    _systems(),
    _nodes_mutex(),
    _nodes(),
    _on_discover_callback(nullptr),
    _on_timeout_callback(nullptr),
    _configuration(Mavsdk::Configuration::UsageType::GroundStation)
{
    LogInfo() << "MAVSDK version: " << mavsdk_version;
    set_configuration(_configuration);

    _system_thread = new std::thread(&MavsdkImpl::system_thread, this);
}

MavsdkImpl::~MavsdkImpl()
{
    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
        _should_exit = true;

        _systems.clear();
        _nodes.clear();
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
    // TODO kalyan - should we do something with this instead of just dropping it?
    if (message.sysid == 0) {
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    // TODO kalyan - I'm assuming this is how we create systems, and should be checked
    // Change system id of null system

    if (message.msgid == MAVLINK_MSG_ID_HEARTBEAT) {
        process_heartbeat(message);
    }

    //if (_systems.find(0) != _systems.end()) {
        //auto null_system = _systems[0];
        //_systems.erase(0);
        //null_system->system_impl()->set_system_id(message.sysid);
        //_systems.insert(system_entry_t(message.sysid, null_system));
    //} else if (_is_single_system) {
        //auto sys = _systems.begin();
        //if (sys->first != message.sysid) {
            //sys->second->system_impl()->set_system_id(message.sysid);
            //_systems.insert(system_entry_t(message.sysid, sys->second));
            //_systems.erase(sys->first);
        //}
    //}

    //if (!does_system_exist(message.sysid)) {
        //make_system_with_component(message.sysid, message.compid);
    //} else {
        //_systems.at(message.sysid)->system_impl()->add_new_component(message.compid);
    //}

    if (_should_exit) {
        // Don't try to call at() if systems have already been destroyed
        // in descructor.
        return;
    }

    //if (_systems.find(message.sysid) != _systems.end()) {
        //_systems.at(message.sysid)->system_impl()->process_mavlink_message(message);
    //}

    uint16_t node_id = (message.sysid << 8) | message.compid;
    if (_nodes.find(node_id) != _nodes.end()) {
        _nodes.at(node_id)->node_impl()->process_mavlink_message(message);
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

void MavsdkImpl::process_heartbeat(const mavlink_message_t& message)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);

    uint16_t node_id = (message.sysid << 8) | message.compid;
    //std::cout << "HB Sysid: " << int(message.sysid) << " compid: " << int(message.compid) << std::endl;

    auto node = _nodes.find(node_id);
    //if (node != _nodes.end()) {
        //node->process_heartbeat(message);
    //} else {
    if (node == _nodes.end()) {
        LogDebug() << "Creating node";
        make_node_with_id(message.sysid, message.compid);
    }
}

ConnectionResult MavsdkImpl::add_any_connection(const std::string& connection_url)
{
    CliArg cli_arg;
    if (!cli_arg.parse(connection_url)) {
        return ConnectionResult::ConnectionUrlInvalid;
    }

    switch (cli_arg.get_protocol()) {
        case CliArg::Protocol::Udp: {
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

        case CliArg::Protocol::Tcp: {
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

        case CliArg::Protocol::Serial: {
            int baudrate = Mavsdk::DEFAULT_SERIAL_BAUDRATE;
            if (cli_arg.get_baudrate()) {
                baudrate = cli_arg.get_baudrate();
            }
            return add_serial_connection(cli_arg.get_path(), baudrate);
        }

        default:
            return ConnectionResult::ConnectionError;
    }
}

ConnectionResult MavsdkImpl::add_udp_connection(const std::string& local_ip, const int local_port)
{
    auto new_conn = std::make_shared<UdpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1), local_ip, local_port);
    if (!new_conn) {
        return ConnectionResult::ConnectionError;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::Success) {
        add_connection(new_conn);
    }
    return ret;
}

ConnectionResult MavsdkImpl::setup_udp_remote(const std::string& remote_ip, int remote_port)
{
    auto new_conn = std::make_shared<UdpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1), "0.0.0.0", 0);
    if (!new_conn) {
        return ConnectionResult::ConnectionError;
    }
    ConnectionResult ret = new_conn->start();
    _is_single_system = true;
    if (ret == ConnectionResult::Success) {
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
        return ConnectionResult::ConnectionError;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::Success) {
        add_connection(new_conn);
    }
    return ret;
}

ConnectionResult MavsdkImpl::add_serial_connection(const std::string& dev_path, int baudrate)
{
    auto new_conn = std::make_shared<SerialConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1), dev_path, baudrate);
    if (!new_conn) {
        return ConnectionResult::ConnectionError;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::Success) {
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
    // TODO kalyan - should probably be replaced with 2 methods, one for getting system/comp ids
    // and another for uid2
    std::vector<uint64_t> uuids = {};

    for (auto it = _systems.begin(); it != _systems.end(); ++it) {
        uint64_t uuid = it->second->_system_impl->get_uuid();
        if (uuid != 0) {
            uuids.push_back(uuid);
        }
    }

    return uuids;
}

AutopilotInterface* MavsdkImpl::get_autopilot()
{
    { 
        std::lock_guard<std::recursive_mutex> lock(_nodes_mutex);

        // in get_autopilot without uuid, we expect to have only one autopilot connected
        for (auto const& connected_node : _nodes) {
            if (connected_node.second->node_impl()->is_autopilot()) {
                return new AutopilotInterface(*connected_node.second);
            }
        }
    }

    return nullptr;
}

System& MavsdkImpl::get_system()
{
    // TODO kalyan - probably should be scrapped
    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
        // In get_system without uuid, we expect to have only
        // one system connected.
        if (_systems.size() == 1) {
            return *(_systems.at(_systems.begin()->first));
        }

        if (_systems.size() > 1) {
            LogWarn()
                << "More than one system found. You should be using `get_system(uuid)` instead of `get_system()`!";

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
    // TODO kalyan - do we even need this? Why not just discover?
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
    return own_address.system_id;
}

uint8_t MavsdkImpl::get_own_component_id() const
{
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
            // TODO kalyan - we should probably allow user to set custom mav type
            return MAV_TYPE_GENERIC;

        default:
            LogErr() << "Unknown configuration";
            return 0;
    }
}

bool MavsdkImpl::is_connected() const
{
    // TODO kalyan - don't really like this, should be looked at in detail
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (_systems.empty()) {
        return false;
    }

    return _systems.begin()->second->is_connected();
}

bool MavsdkImpl::is_connected(const uint64_t uuid) const
{
    // TODO kalyan - don't really like this, should be looked at in detail
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

void MavsdkImpl::make_node_with_id(uint8_t system_id, uint8_t component_id) {
    std::lock_guard<std::recursive_mutex> lock(_nodes_mutex);

    if (_should_exit) {
        // When the node got destroyed in the destructor, we have to give up.
        // TODO kalyan - wait why?
        return;
    }

    LogDebug() << "Init new Node: System ID: " << int(system_id) << "Comp Id: " << int(component_id);
    // Make a node with this id pair
    uint16_t node_id = (system_id << 8) | (component_id);
    auto new_node = std::make_shared<Node>(*this, system_id, component_id);

    _nodes.insert(node_entry_t(node_id, new_node));

    notify_on_discover(system_id, component_id);
}

bool MavsdkImpl::does_system_exist(uint8_t system_id)
{
    // TODO kalyan - system ID is not enough to determine connection?
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (!_should_exit) {
        return (_systems.find(system_id) != _systems.end());
    }
    // When the system got destroyed in the destructor, we have to give up.
    return false;
}

void MavsdkImpl::notify_on_discover(const uint8_t system_id, const uint8_t component_id)
{
    // TODO kalyan - where is this called?
    if (_on_discover_callback != nullptr) {
        _on_discover_callback(system_id, component_id);
    }
}

void MavsdkImpl::notify_on_timeout(const uint8_t system_id, const uint8_t component_id)
{
    // TODO kalyan - where is this called?
    LogDebug() << "Lost " << system_id << " " << component_id;
    if (_on_timeout_callback != nullptr) {
        _on_timeout_callback(system_id, component_id);
    }
}

void MavsdkImpl::register_on_discover(const Mavsdk::event_callback_t callback)
{
    // TODO kalyan - where is this called?
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (callback) {
        for (auto const& connected_node : _nodes) {
            callback(connected_node.second->get_system_id(), connected_node.second->get_component_id());
        }
        //for (auto const& connected_system : _systems) {
            //// Ignore dummy system with system ID 0.
            //if (connected_system.first == 0) {
                //continue;
            //}
            //// TODO where will this be called? We shouldn't have a scenario where the uuid isn't init
            //// Ignore system if UUID is not initialized yet.
            //if (connected_system.second->get_uuid() == 0) {
                //continue;
            //}
            //callback(connected_system.second->get_uuid());
        //}
    }

    _on_discover_callback = callback;
}

void MavsdkImpl::register_on_timeout(const Mavsdk::event_callback_t callback)
{
    // TODO kalyan - where is this called?
    _on_timeout_callback = callback;
}

void MavsdkImpl::system_thread()
{
    dl_time_t last_time{};

    while (!_should_exit) {
        if (_time.elapsed_since_s(last_time) >= MavsdkImpl::_HEARTBEAT_SEND_INTERVAL_S) {
            send_heartbeat();
        }
        last_time = _time.steady_time();

        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // in case we want to do anything else in this thread
    }
}

void MavsdkImpl::send_heartbeat()
{
    std::cout << "Sending heartbeat" << std::endl;
    mavlink_message_t message;

    mavlink_msg_heartbeat_pack(
            get_own_system_id(),
            get_own_component_id(),
            &message,
            get_mav_type(),
            MAV_AUTOPILOT_INVALID,
            0,
            0,
            0);
    send_message(message);
}

} // namespace mavsdk
