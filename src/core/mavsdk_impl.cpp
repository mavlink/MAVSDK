#include "mavsdk_impl.h"

#include <algorithm>
#include <mutex>
#include <utility>

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

MavsdkImpl::MavsdkImpl() : timeout_handler(_time), call_every_handler(_time)
{
    LogInfo() << "MAVSDK version: " << mavsdk_version;

    if (const char* env_p = std::getenv("MAVSDK_CALLBACK_DEBUGGING")) {
        if (env_p && std::string("1").compare(env_p) == 0) {
            LogDebug() << "Callback debugging is on.";
            _callback_debugging = true;
        }
    }

    if (const char* env_p = std::getenv("MAVSDK_MESSAGE_DEBUGGING")) {
        if (env_p && std::string("1").compare(env_p) == 0) {
            LogDebug() << "Message debugging is on.";
            _message_logging_on = true;
        }
    }

    _work_thread = new std::thread(&MavsdkImpl::work_thread, this);

    _process_user_callbacks_thread =
        new std::thread(&MavsdkImpl::process_user_callbacks_thread, this);
}

MavsdkImpl::~MavsdkImpl()
{
    call_every_handler.remove(_heartbeat_send_cookie);

    _should_exit = true;

    if (_process_user_callbacks_thread != nullptr) {
        _user_callback_queue.stop();
        _process_user_callbacks_thread->join();
        delete _process_user_callbacks_thread;
        _process_user_callbacks_thread = nullptr;
    }

    if (_work_thread != nullptr) {
        _work_thread->join();
        delete _work_thread;
        _work_thread = nullptr;
    }

    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

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

std::vector<std::shared_ptr<System>> MavsdkImpl::systems() const
{
    std::vector<std::shared_ptr<System>> systems_result{};

    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
    for (auto& system : _systems) {
        // We ignore the 0 entry because it's just a null system.
        // It's only created because the older, deprecated API needs a
        // reference.
        if (system.first == 0) {
            continue;
        }
        systems_result.push_back(system.second);
    }

    return systems_result;
}

void MavsdkImpl::forward_message(mavlink_message_t& message, Connection* connection)
{
    // Forward_message Function implementing Mavlink routing rules.
    // See https://mavlink.io/en/guide/routing.html

    bool forward_heartbeats_enabled = true;
    const uint8_t target_system_id = get_target_system_id(message);
    const uint8_t target_component_id = get_target_component_id(message);

    // If it's a message only for us, we keep it, otherwise, we forward it.
    const bool targeted_only_at_us =
        (target_system_id == get_own_system_id() && target_component_id == get_own_component_id());

    // We don't forward heartbeats unless it's specifically enabled.
    const bool heartbeat_check_ok =
        (message.msgid != MAVLINK_MSG_ID_HEARTBEAT || forward_heartbeats_enabled);

    if (!targeted_only_at_us && heartbeat_check_ok) {
        std::lock_guard<std::mutex> lock(_connections_mutex);

        unsigned successful_emissions = 0;
        for (auto it = _connections.begin(); it != _connections.end(); ++it) {
            // Check whether the connection is not the one from which we received the message.
            // And also check if the connection was set to forward messages.
            if ((*it).get() == connection || !(**it).should_forward_messages()) {
                continue;
            }
            if ((**it).send_message(message)) {
                successful_emissions++;
            }
        }
        if (successful_emissions == 0) {
            LogErr() << "Message forwarding failed";
        }
    }
}

void MavsdkImpl::receive_message(mavlink_message_t& message, Connection* connection)
{
    if (_message_logging_on) {
        LogDebug() << "Processing message " << message.msgid << " from "
                   << static_cast<int>(message.sysid) << "/" << static_cast<int>(message.compid);
    }

    /** @note: Forward message if option is enabled and multiple interfaces are connected.
     *  Performs message forwarding checks for every messages if message forwarding
     *  is enabled on at least one connection, and in case of a single forwarding connection,
     *  we check that it is not the one which received the current message.
     *
     * Conditions:
     * 1. At least 2 connections.
     * 2. At least 1 forwarding connection.
     * 3. At least 2 forwarding connections or current connection is not forwarding.
     */
    if (_connections.size() > 1 && connection->forwarding_connections_count() > 0 &&
        (connection->forwarding_connections_count() > 1 ||
         !connection->should_forward_messages())) {
        if (_message_logging_on) {
            LogDebug() << "Forwarding message " << message.msgid << " from "
                       << static_cast<int>(message.sysid) << "/"
                       << static_cast<int>(message.compid);
        }
        forward_message(message, connection);
    }

    // Don't ever create a system with sysid 0.
    if (message.sysid == 0) {
        if (_message_logging_on) {
            LogDebug() << "Ignoring message with sysid == 0";
        }
        return;
    }

    // Filter out messages by QGroundControl, however, only do that if MAVSDK
    // is also implementing a ground station and not if it is used in another
    // configuration, e.g. on a companion.
    //
    // This is a workaround because PX4 started forwarding messages between
    // mavlink instances which leads to existing implementations (including
    // examples and integration tests) to connect to QGroundControl by accident
    // instead of PX4 because the check `has_autopilot()` is not used.
    if (_configuration.get_usage_type() == Mavsdk::Configuration::UsageType::GroundStation &&
        message.sysid == 255 && message.compid == MAV_COMP_ID_MISSIONPLANNER) {
        if (_message_logging_on) {
            LogDebug() << "Ignoring messages from QGC as we are also a ground station";
        }
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (_systems.size() == 1 && _systems[0].first == 0) {
        _systems[0].first = message.sysid;
        _systems[0].second->system_impl()->set_system_id(message.sysid);
    }

    bool found_system = false;
    for (auto& system : _systems) {
        if (system.first == message.sysid) {
            system.second->system_impl()->add_new_component(message.compid);
            found_system = true;
            break;
        }
    }

    if (!found_system) {
        make_system_with_component(message.sysid, message.compid);
    }

    if (_should_exit) {
        // Don't try to call at() if systems have already been destroyed
        // in descructor.
        return;
    }

    for (auto& system : _systems) {
        if (system.first == message.sysid) {
            system.second->system_impl()->process_mavlink_message(message);
            break;
        }
    }
}

bool MavsdkImpl::send_message(mavlink_message_t& message)
{
    if (_message_logging_on) {
        LogDebug() << "Sending message " << message.msgid << " from "
                   << static_cast<int>(message.sysid) << "/" << static_cast<int>(message.compid);
    }

    std::lock_guard<std::mutex> lock(_connections_mutex);

    uint8_t successful_emissions = 0;
    for (auto it = _connections.begin(); it != _connections.end(); ++it) {
        const uint8_t target_system_id = get_target_system_id(message);

        if (target_system_id != 0 && !(**it).has_system_id(target_system_id)) {
            continue;
        }

        if ((**it).send_message(message)) {
            successful_emissions++;
        }
    }

    if (successful_emissions == 0) {
        LogErr() << "Sending message failed";
        return false;
    }

    return true;
}

ConnectionResult MavsdkImpl::add_any_connection(
    const std::string& connection_url, ForwardingOption forwarding_option)
{
    CliArg cli_arg;
    if (!cli_arg.parse(connection_url)) {
        return ConnectionResult::ConnectionUrlInvalid;
    }

    switch (cli_arg.get_protocol()) {
        case CliArg::Protocol::Udp: {
            int port = cli_arg.get_port() ? cli_arg.get_port() : Mavsdk::DEFAULT_UDP_PORT;

            if (cli_arg.get_path().empty() || cli_arg.get_path() == Mavsdk::DEFAULT_UDP_BIND_IP) {
                std::string path = Mavsdk::DEFAULT_UDP_BIND_IP;
                return add_udp_connection(path, port, forwarding_option);
            } else {
                std::string path = cli_arg.get_path();
                return setup_udp_remote(path, port, forwarding_option);
            }
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
            return add_tcp_connection(path, port, forwarding_option);
        }

        case CliArg::Protocol::Serial: {
            int baudrate = Mavsdk::DEFAULT_SERIAL_BAUDRATE;
            if (cli_arg.get_baudrate()) {
                baudrate = cli_arg.get_baudrate();
            }
            bool flow_control = cli_arg.get_flow_control();
            return add_serial_connection(
                cli_arg.get_path(), baudrate, flow_control, forwarding_option);
        }

        default:
            return ConnectionResult::ConnectionError;
    }
}

ConnectionResult MavsdkImpl::add_udp_connection(
    const std::string& local_ip, const int local_port, ForwardingOption forwarding_option)
{
    auto new_conn = std::make_shared<UdpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1, std::placeholders::_2),
        local_ip,
        local_port,
        forwarding_option);
    if (!new_conn) {
        return ConnectionResult::ConnectionError;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::Success) {
        add_connection(new_conn);
    }
    return ret;
}

ConnectionResult MavsdkImpl::setup_udp_remote(
    const std::string& remote_ip, int remote_port, ForwardingOption forwarding_option)
{
    auto new_conn = std::make_shared<UdpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1, std::placeholders::_2),
        "0.0.0.0",
        0,
        forwarding_option);
    if (!new_conn) {
        return ConnectionResult::ConnectionError;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::Success) {
        new_conn->add_remote(remote_ip, remote_port);
        add_connection(new_conn);
        make_system_with_component(0, 0, true);
    }
    return ret;
}

ConnectionResult MavsdkImpl::add_tcp_connection(
    const std::string& remote_ip, int remote_port, ForwardingOption forwarding_option)
{
    auto new_conn = std::make_shared<TcpConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1, std::placeholders::_2),
        remote_ip,
        remote_port,
        forwarding_option);
    if (!new_conn) {
        return ConnectionResult::ConnectionError;
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::Success) {
        add_connection(new_conn);
    }
    return ret;
}

ConnectionResult MavsdkImpl::add_serial_connection(
    const std::string& dev_path,
    int baudrate,
    bool flow_control,
    ForwardingOption forwarding_option)
{
    auto new_conn = std::make_shared<SerialConnection>(
        std::bind(&MavsdkImpl::receive_message, this, std::placeholders::_1, std::placeholders::_2),
        dev_path,
        baudrate,
        flow_control,
        forwarding_option);
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

void MavsdkImpl::set_configuration(Mavsdk::Configuration new_configuration)
{
    if (new_configuration.get_always_send_heartbeats() &&
        !_configuration.get_always_send_heartbeats()) {
        _configuration = new_configuration;
        start_sending_heartbeats();
    } else if (
        !new_configuration.get_always_send_heartbeats() &&
        _configuration.get_always_send_heartbeats() && !is_connected()) {
        _configuration = new_configuration;
        stop_sending_heartbeats();
    }
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
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    // In get_system without uuid, we expect to have only
    // one system connected.

    if (_systems.size() == 1) {
        // Expected case.

    } else if (_systems.size() > 1) {
        LogWarn()
            << "More than one system found. You should be using `get_system(uuid)` instead of `get_system()`!";
        // Just return first system instead of failing.

    } else {
        uint8_t system_id = 0, comp_id = 0;
        make_system_with_component(system_id, comp_id);
    }

    return *(_systems[0].second);
}

System& MavsdkImpl::get_system(const uint64_t uuid)
{
    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
        // TODO: make a cache map for this.
        for (auto& system : _systems) {
            if (system.second->_system_impl->get_uuid() == uuid) {
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

    return *_systems[0].second;
}

uint8_t MavsdkImpl::get_own_system_id() const
{
    return _configuration.get_system_id();
}

uint8_t MavsdkImpl::get_own_component_id() const
{
    return _configuration.get_component_id();
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
        if (it->second->_system_impl->get_uuid() == uuid) {
            return it->second->is_connected();
        }
    }
    return false;
}

void MavsdkImpl::make_system_with_component(
    uint8_t system_id, uint8_t comp_id, bool always_connected)
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    if (_should_exit) {
        // When the system got destroyed in the destructor, we have to give up.
        return;
    }

    LogDebug() << "New: System ID: " << int(system_id) << " Comp ID: " << int(comp_id);
    // Make a system with its first component
    auto new_system = std::make_shared<System>(*this);
    new_system->init(system_id, comp_id, always_connected);

    _systems.push_back(std::pair<uint8_t, std::shared_ptr<System>>(system_id, new_system));
}

void MavsdkImpl::notify_on_discover(const uint64_t uuid)
{
    if (_on_discover_callback) {
        _on_discover_callback(uuid);
    }

    std::lock_guard<std::mutex> lock(_new_system_callback_mutex);
    if (_new_system_callback) {
        auto temp_callback = _new_system_callback;
        call_user_callback([temp_callback]() { temp_callback(); });
    }
}

void MavsdkImpl::notify_on_timeout(const uint64_t uuid)
{
    LogDebug() << "Lost " << uuid;
    if (_on_timeout_callback) {
        _on_timeout_callback(uuid);
    }

    std::lock_guard<std::mutex> lock(_new_system_callback_mutex);
    if (_new_system_callback) {
        auto temp_callback = _new_system_callback;
        call_user_callback([temp_callback]() { temp_callback(); });
    }
}

void MavsdkImpl::subscribe_on_new_system(Mavsdk::NewSystemCallback callback)
{
    std::lock_guard<std::mutex> lock(_new_system_callback_mutex);
    _new_system_callback = callback;

    const auto is_any_system_connected = [this]() {
        std::vector<std::shared_ptr<System>> connected_systems = systems();
        return std::any_of(connected_systems.cbegin(), connected_systems.cend(), [](auto& system) {
            return system->is_connected();
        });
    };

    if (_new_system_callback != nullptr && is_any_system_connected()) {
        _new_system_callback();
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
            if (connected_system.second->_system_impl->get_uuid() == 0) {
                continue;
            }
            callback(connected_system.second->_system_impl->get_uuid());
        }
    }

    _on_discover_callback = callback;
}

void MavsdkImpl::register_on_timeout(const Mavsdk::event_callback_t callback)
{
    _on_timeout_callback = callback;
}

void MavsdkImpl::work_thread()
{
    while (!_should_exit) {
        timeout_handler.run_once();
        call_every_handler.run_once();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void MavsdkImpl::call_user_callback_located(
    const std::string& filename, const int linenumber, const std::function<void()>& func)
{
    auto callback_size = _user_callback_queue.size();
    if (callback_size == 10) {
        LogWarn()
            << "User callback queue too slow.\n"
               "See: https://mavsdk.mavlink.io/main/en/cpp/troubleshooting.html#user_callbacks";

    } else if (callback_size == 99) {
        LogErr()
            << "User callback queue overflown\n"
               "See: https://mavsdk.mavlink.io/main/en/cpp/troubleshooting.html#user_callbacks";

    } else if (callback_size == 100) {
        return;
    }

    // We only need to keep track of filename and linenumber if we're actually debugging this.
    UserCallback user_callback =
        _callback_debugging ? UserCallback{func, filename, linenumber} : UserCallback{func};

    _user_callback_queue.enqueue(user_callback);
}

void MavsdkImpl::process_user_callbacks_thread()
{
    while (!_should_exit) {
        auto callback = _user_callback_queue.dequeue();
        if (!callback) {
            continue;
        }

        void* cookie{nullptr};

        const double timeout_s = 1.0;
        timeout_handler.add(
            [&]() {
                if (_callback_debugging) {
                    LogWarn() << "Callback called from " << callback.value().filename << ":"
                              << callback.value().linenumber << " took more than " << timeout_s
                              << " second to run.";
                    fflush(stdout);
                    fflush(stderr);
                    abort();
                } else {
                    LogWarn()
                        << "Callback took more than " << timeout_s << " second to run.\n"
                        << "See: https://mavsdk.mavlink.io/main/en/cpp/troubleshooting.html#user_callbacks";
                }
            },
            timeout_s,
            &cookie);
        callback.value().func();
        timeout_handler.remove(cookie);
    }
}

void MavsdkImpl::start_sending_heartbeats()
{
    if (_heartbeat_send_cookie == nullptr) {
        call_every_handler.add(
            [this]() { send_heartbeat(); }, _HEARTBEAT_SEND_INTERVAL_S, &_heartbeat_send_cookie);
    }
}

void MavsdkImpl::stop_sending_heartbeats()
{
    if (!_configuration.get_always_send_heartbeats()) {
        call_every_handler.remove(_heartbeat_send_cookie);
        _heartbeat_send_cookie = nullptr;
    }
}

void MavsdkImpl::send_heartbeat()
{
    mavlink_message_t message;
    mavlink_msg_heartbeat_pack(
        get_own_system_id(),
        get_own_component_id(),
        &message,
        get_mav_type(),
        get_own_component_id() == MAV_COMP_ID_AUTOPILOT1 ? MAV_AUTOPILOT_GENERIC :
                                                           MAV_AUTOPILOT_INVALID,
        0,
        0,
        0);
    send_message(message);
}

uint8_t MavsdkImpl::get_target_system_id(const mavlink_message_t& message)
{
    // Checks whether connection knows target system ID by extracting target system if set.
    const mavlink_msg_entry_t* meta = mavlink_get_msg_entry(message.msgid);

    if (meta == nullptr || !(meta->flags & MAV_MSG_ENTRY_FLAG_HAVE_TARGET_SYSTEM)) {
        return 0;
    }

    // Don't look at the target system offset if it is outside of the payload length.
    // This can happen if the fields are trimmed.
    if (meta->target_system_ofs >= message.len) {
        return 0;
    }

    return (_MAV_PAYLOAD(&message))[meta->target_system_ofs];
}

uint8_t MavsdkImpl::get_target_component_id(const mavlink_message_t& message)
{
    // Checks whether connection knows target system ID by extracting target system if set.
    const mavlink_msg_entry_t* meta = mavlink_get_msg_entry(message.msgid);

    if (meta == nullptr || !(meta->flags & MAV_MSG_ENTRY_FLAG_HAVE_TARGET_COMPONENT)) {
        return 0;
    }

    // Don't look at the target component offset if it is outside of the payload length.
    // This can happen if the fields are trimmed.
    if (meta->target_component_ofs >= message.len) {
        return 0;
    }

    return (_MAV_PAYLOAD(&message))[meta->target_system_ofs];
}

} // namespace mavsdk
