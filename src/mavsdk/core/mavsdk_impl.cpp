#include "mavsdk_impl.h"

#include <algorithm>
#include <mutex>
#include <tcp_server_connection.h>

#include "connection.h"
#include "tcp_client_connection.h"
#include "tcp_server_connection.h"
#include "udp_connection.h"
#include "system.h"
#include "system_impl.h"
#include "serial_connection.h"
#include "version.h"
#include "server_component_impl.h"
#include "plugin_base.h"
#include "mavlink_channels.h"
#include "callback_list.tpp"

namespace mavsdk {

template class CallbackList<>;

MavsdkImpl::MavsdkImpl(const Mavsdk::Configuration& configuration) :
    timeout_handler(time),
    call_every_handler(time)
{
    LogInfo() << "MAVSDK version: " << mavsdk_version;

    if (const char* env_p = std::getenv("MAVSDK_CALLBACK_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Callback debugging is on.";
            _callback_debugging = true;
        }
    }

    if (const char* env_p = std::getenv("MAVSDK_MESSAGE_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Message debugging is on.";
            _message_logging_on = true;
        }
    }

    set_configuration(configuration);

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

std::string MavsdkImpl::version()
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

std::optional<std::shared_ptr<System>> MavsdkImpl::first_autopilot(double timeout_s)
{
    {
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
        for (auto system : _systems) {
            if (system.second->is_connected() && system.second->has_autopilot()) {
                return system.second;
            }
        }
    }

    if (timeout_s == 0.0) {
        // Don't wait at all.
        return {};
    }

    auto prom = std::promise<std::shared_ptr<System>>();

    std::once_flag flag;
    auto handle = subscribe_on_new_system([this, &prom, &flag]() {
        const auto system = systems().at(0);
        if (system->is_connected() && system->has_autopilot()) {
            std::call_once(flag, [&prom, &system]() { prom.set_value(system); });
        }
    });

    auto fut = prom.get_future();

    if (timeout_s > 0.0) {
        if (fut.wait_for(std::chrono::milliseconds(int64_t(timeout_s * 1e3))) ==
            std::future_status::ready) {
            unsubscribe_on_new_system(handle);
            return fut.get();

        } else {
            unsubscribe_on_new_system(handle);
            return std::nullopt;
        }
    } else {
        fut.wait();
        unsubscribe_on_new_system(handle);
        return std::optional(fut.get());
    }
}

std::shared_ptr<ServerComponent> MavsdkImpl::server_component(unsigned instance)
{
    auto component_type = _configuration.get_component_type();
    switch (component_type) {
        case Mavsdk::ComponentType::Autopilot:
        case Mavsdk::ComponentType::GroundStation:
        case Mavsdk::ComponentType::CompanionComputer:
        case Mavsdk::ComponentType::Camera:
        case Mavsdk::ComponentType::Custom:
            return server_component_by_type(component_type, instance);
        default:
            LogErr() << "Unknown component type";
            return {};
    }
}

std::shared_ptr<ServerComponent>
MavsdkImpl::server_component_by_type(Mavsdk::ComponentType server_component_type, unsigned instance)
{
    switch (server_component_type) {
        case Mavsdk::ComponentType::Autopilot:
            if (instance == 0) {
                return server_component_by_id(MAV_COMP_ID_AUTOPILOT1);
            } else {
                LogErr() << "Only autopilot instance 0 is valid";
                return {};
            }

        case Mavsdk::ComponentType::GroundStation:
            if (instance == 0) {
                return server_component_by_id(MAV_COMP_ID_MISSIONPLANNER);
            } else {
                LogErr() << "Only one ground station supported at this time";
                return {};
            }

        case Mavsdk::ComponentType::CompanionComputer:
            if (instance == 0) {
                return server_component_by_id(MAV_COMP_ID_ONBOARD_COMPUTER);
            } else if (instance == 1) {
                return server_component_by_id(MAV_COMP_ID_ONBOARD_COMPUTER2);
            } else if (instance == 2) {
                return server_component_by_id(MAV_COMP_ID_ONBOARD_COMPUTER3);
            } else if (instance == 3) {
                return server_component_by_id(MAV_COMP_ID_ONBOARD_COMPUTER4);
            } else {
                LogErr() << "Only companion computer 0..3 are supported";
                return {};
            }

        case Mavsdk::ComponentType::Camera:
            if (instance == 0) {
                return server_component_by_id(MAV_COMP_ID_CAMERA);
            } else if (instance == 1) {
                return server_component_by_id(MAV_COMP_ID_CAMERA2);
            } else if (instance == 2) {
                return server_component_by_id(MAV_COMP_ID_CAMERA3);
            } else if (instance == 3) {
                return server_component_by_id(MAV_COMP_ID_CAMERA4);
            } else if (instance == 4) {
                return server_component_by_id(MAV_COMP_ID_CAMERA5);
            } else if (instance == 5) {
                return server_component_by_id(MAV_COMP_ID_CAMERA6);
            } else {
                LogErr() << "Only camera 0..5 are supported";
                return {};
            }

        default:
            LogErr() << "Unknown server component type";
            return {};
    }
}

std::shared_ptr<ServerComponent> MavsdkImpl::server_component_by_id(uint8_t component_id)
{
    if (component_id == 0) {
        LogErr() << "Server component with component ID 0 not allowed";
        return nullptr;
    }

    std::lock_guard<std::mutex> lock(_server_components_mutex);

    for (auto& it : _server_components) {
        if (it.first == component_id) {
            if (it.second != nullptr) {
                return it.second;
            } else {
                it.second = std::make_shared<ServerComponent>(*this, component_id);
            }
        }
    }

    _server_components.emplace_back(std::pair<uint8_t, std::shared_ptr<ServerComponent>>(
        component_id, std::make_shared<ServerComponent>(*this, component_id)));

    return _server_components.back().second;
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
        for (auto& entry : _connections) {
            // Check whether the connection is not the one from which we received the message.
            // And also check if the connection was set to forward messages.
            if (entry.connection.get() == connection ||
                !entry.connection->should_forward_messages()) {
                continue;
            }
            if ((*entry.connection).send_message(message)) {
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

    // This is a low level interface where incoming messages can be tampered
    // with or even dropped.
    {
        std::lock_guard<std::mutex> lock(_intercept_callback_mutex);
        if (_intercept_incoming_messages_callback != nullptr) {
            bool keep = _intercept_incoming_messages_callback(message);
            if (!keep) {
                LogDebug() << "Dropped incoming message: " << int(message.msgid);
                return;
            }
        }
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
    if (_connections.size() > 1 && mavsdk::Connection::forwarding_connections_count() > 0 &&
        (mavsdk::Connection::forwarding_connections_count() > 1 ||
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
    if (_configuration.get_component_type() == Mavsdk::ComponentType::GroundStation &&
        message.sysid == 255 && message.compid == MAV_COMP_ID_MISSIONPLANNER) {
        if (_message_logging_on) {
            LogDebug() << "Ignoring messages from QGC as we are also a ground station";
        }
        return;
    }

    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    bool found_system = false;
    for (auto& system : _systems) {
        if (system.first == message.sysid) {
            system.second->system_impl()->add_new_component(message.compid);
            found_system = true;
            break;
        }
    }

    if (!found_system && message.compid == MAV_COMP_ID_TELEMETRY_RADIO) {
        if (_message_logging_on) {
            LogDebug() << "Don't create new system just for telemetry radio";
        }
        return;
    }

    if (!found_system) {
        make_system_with_component(message.sysid, message.compid);
    }

    if (_should_exit) {
        // Don't try to call at() if systems have already been destroyed
        // in destructor.
        return;
    }

    mavlink_message_handler.process_message(message);

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
                   << static_cast<int>(message.sysid) << "/" << static_cast<int>(message.compid)
                   << " to " << static_cast<int>(get_target_system_id(message)) << "/"
                   << static_cast<int>(get_target_component_id(message));
    }

    // This is a low level interface where outgoing messages can be tampered
    // with or even dropped.
    if (_intercept_outgoing_messages_callback != nullptr) {
        const bool keep = _intercept_outgoing_messages_callback(message);
        if (!keep) {
            // We fake that everything was sent as instructed because
            // a potential loss would happen later, and we would not be informed
            // about it.
            LogDebug() << "Dropped outgoing message: " << int(message.msgid);
            return true;
        }
    }

    std::lock_guard<std::mutex> lock(_connections_mutex);

    if (_connections.empty()) {
        // We obviously can't send any messages without a connection added, so
        // we silently ignore this.
        return true;
    }

    uint8_t successful_emissions = 0;
    for (auto& _connection : _connections) {
        const uint8_t target_system_id = get_target_system_id(message);

        if (target_system_id != 0 && !(*_connection.connection).has_system_id(target_system_id)) {
            continue;
        }

        if ((*_connection.connection).send_message(message)) {
            successful_emissions++;
        }
    }

    if (successful_emissions == 0) {
        LogErr() << "Sending message failed";
        return false;
    }

    return true;
}

std::pair<ConnectionResult, Mavsdk::ConnectionHandle> MavsdkImpl::add_any_connection(
    const std::string& connection_url, ForwardingOption forwarding_option)
{
    CliArg cli_arg;
    if (!cli_arg.parse(connection_url)) {
        return {ConnectionResult::ConnectionUrlInvalid, Mavsdk::ConnectionHandle{}};
    }

    return std::visit(
        overloaded{
            [](std::monostate) {
                // Should not happen anyway.
                return std::pair<ConnectionResult, Mavsdk::ConnectionHandle>{
                    ConnectionResult::ConnectionUrlInvalid, Mavsdk::ConnectionHandle()};
            },
            [this, forwarding_option](const CliArg::Udp& udp) {
                return add_udp_connection(udp, forwarding_option);
            },
            [this, forwarding_option](const CliArg::Tcp& tcp) {
                return add_tcp_connection(tcp, forwarding_option);
            },
            [this, forwarding_option](const CliArg::Serial& serial) {
                return add_serial_connection(
                    serial.path, serial.baudrate, serial.flow_control_enabled, forwarding_option);
            }},
        cli_arg.protocol);
}

std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
MavsdkImpl::add_udp_connection(const CliArg::Udp& udp, ForwardingOption forwarding_option)
{
    auto new_conn = std::make_shared<UdpConnection>(
        [this](mavlink_message_t& message, Connection* connection) {
            receive_message(message, connection);
        },
        udp.mode == CliArg::Udp::Mode::In ? udp.host : "0.0.0.0",
        udp.mode == CliArg::Udp::Mode::In ? udp.port : 0,
        forwarding_option);

    if (!new_conn) {
        return {ConnectionResult::ConnectionError, Mavsdk::ConnectionHandle{}};
    }

    ConnectionResult ret = new_conn->start();

    if (ret != ConnectionResult::Success) {
        return {ret, Mavsdk::ConnectionHandle{}};
    }

    auto handle = add_connection(new_conn);

    if (udp.mode == CliArg::Udp::Mode::Out) {
        new_conn->add_remote(udp.host, udp.port);
        std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

        // With a UDP remote, we need to initiate the connection by sending heartbeats.
        auto new_configuration = get_configuration();
        new_configuration.set_always_send_heartbeats(true);
        set_configuration(new_configuration);
    }
    return {ConnectionResult::Success, handle};
}

std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
MavsdkImpl::add_tcp_connection(const CliArg::Tcp& tcp, ForwardingOption forwarding_option)
{
    if (tcp.mode == CliArg::Tcp::Mode::Out) {
        auto new_conn = std::make_shared<TcpClientConnection>(
            [this](mavlink_message_t& message, Connection* connection) {
                receive_message(message, connection);
            },
            tcp.host,
            tcp.port,
            forwarding_option);
        if (!new_conn) {
            return {ConnectionResult::ConnectionError, Mavsdk::ConnectionHandle{}};
        }
        ConnectionResult ret = new_conn->start();
        if (ret == ConnectionResult::Success) {
            return {ret, add_connection(new_conn)};
        } else {
            return {ret, Mavsdk::ConnectionHandle{}};
        }
    } else {
        auto new_conn = std::make_shared<TcpServerConnection>(
            [this](mavlink_message_t& message, Connection* connection) {
                receive_message(message, connection);
            },
            tcp.host,
            tcp.port,
            forwarding_option);
        if (!new_conn) {
            return {ConnectionResult::ConnectionError, Mavsdk::ConnectionHandle{}};
        }
        ConnectionResult ret = new_conn->start();
        if (ret == ConnectionResult::Success) {
            return {ret, add_connection(new_conn)};
        } else {
            return {ret, Mavsdk::ConnectionHandle{}};
        }
    }
}

std::pair<ConnectionResult, Mavsdk::ConnectionHandle> MavsdkImpl::add_serial_connection(
    const std::string& dev_path,
    int baudrate,
    bool flow_control,
    ForwardingOption forwarding_option)
{
    auto new_conn = std::make_shared<SerialConnection>(
        [this](mavlink_message_t& message, Connection* connection) {
            receive_message(message, connection);
        },
        dev_path,
        baudrate,
        flow_control,
        forwarding_option);
    if (!new_conn) {
        return {ConnectionResult::ConnectionError, Mavsdk::ConnectionHandle{}};
    }
    ConnectionResult ret = new_conn->start();
    if (ret == ConnectionResult::Success) {
        auto handle = add_connection(new_conn);

        auto new_configuration = get_configuration();

        // PX4 starting with v1.13 does not send heartbeats by default, so we need
        // to initiate the MAVLink connection by sending heartbeats.
        // Therefore, we override the default here and enable sending heartbeats.
        new_configuration.set_always_send_heartbeats(true);
        set_configuration(new_configuration);

        return {ret, handle};

    } else {
        return {ret, Mavsdk::ConnectionHandle{}};
    }
}

Mavsdk::ConnectionHandle
MavsdkImpl::add_connection(const std::shared_ptr<Connection>& new_connection)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);
    auto handle = _connections_handle_factory.create();
    _connections.emplace_back(ConnectionEntry{new_connection, handle});

    return handle;
}

void MavsdkImpl::remove_connection(Mavsdk::ConnectionHandle handle)
{
    std::lock_guard<std::mutex> lock(_connections_mutex);

    _connections.erase(std::remove_if(_connections.begin(), _connections.end(), [&](auto&& entry) {
        return (entry.handle == handle);
    }));
}

Mavsdk::Configuration MavsdkImpl::get_configuration() const
{
    return _configuration;
}

void MavsdkImpl::set_configuration(Mavsdk::Configuration new_configuration)
{
    // We just point the default to the newly created component. This means
    // that the previous default component will be deleted if it is not
    // used/referenced anywhere.
    _default_server_component = server_component_by_id(new_configuration.get_component_id());

    if (new_configuration.get_always_send_heartbeats() &&
        !_configuration.get_always_send_heartbeats()) {
        start_sending_heartbeats();
    } else if (
        !new_configuration.get_always_send_heartbeats() &&
        _configuration.get_always_send_heartbeats() && !is_any_system_connected()) {
        stop_sending_heartbeats();
    }

    _configuration = new_configuration;
}

uint8_t MavsdkImpl::get_own_system_id() const
{
    return _configuration.get_system_id();
}

uint8_t MavsdkImpl::get_own_component_id() const
{
    return _configuration.get_component_id();
}

uint8_t MavsdkImpl::channel() const
{
    // TODO
    return 0;
}

Autopilot MavsdkImpl::autopilot() const
{
    // TODO
    return Autopilot::Px4;
}

// FIXME: this should be per component
uint8_t MavsdkImpl::get_mav_type() const
{
    switch (_configuration.get_component_type()) {
        case Mavsdk::ComponentType::Autopilot:
            return MAV_TYPE_GENERIC;

        case Mavsdk::ComponentType::GroundStation:
            return MAV_TYPE_GCS;

        case Mavsdk::ComponentType::CompanionComputer:
            return MAV_TYPE_ONBOARD_CONTROLLER;

        case Mavsdk::ComponentType::Camera:
            return MAV_TYPE_CAMERA;

        case Mavsdk::ComponentType::Custom:
            return MAV_TYPE_GENERIC;

        default:
            LogErr() << "Unknown configuration";
            return 0;
    }
}

void MavsdkImpl::make_system_with_component(uint8_t system_id, uint8_t comp_id)
{
    // Needs _systems_lock

    if (_should_exit) {
        // When the system got destroyed in the destructor, we have to give up.
        return;
    }

    if (static_cast<int>(system_id) == 0 && static_cast<int>(comp_id) == 0) {
        LogDebug() << "Initializing connection to remote system...";
    } else {
        LogDebug() << "New system ID: " << static_cast<int>(system_id)
                   << " Comp ID: " << static_cast<int>(comp_id);
    }

    // Make a system with its first component
    auto new_system = std::make_shared<System>(*this);
    new_system->init(system_id, comp_id);

    _systems.emplace_back(system_id, new_system);
}

void MavsdkImpl::notify_on_discover()
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
    _new_system_callbacks.queue([this](const auto& func) { call_user_callback(func); });
}

void MavsdkImpl::notify_on_timeout()
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);
    _new_system_callbacks.queue([this](const auto& func) { call_user_callback(func); });
}

Mavsdk::NewSystemHandle
MavsdkImpl::subscribe_on_new_system(const Mavsdk::NewSystemCallback& callback)
{
    std::lock_guard<std::recursive_mutex> lock(_systems_mutex);

    const auto handle = _new_system_callbacks.subscribe(callback);

    if (is_any_system_connected()) {
        _new_system_callbacks.queue([this](const auto& func) { call_user_callback(func); });
    }

    return handle;
}

void MavsdkImpl::unsubscribe_on_new_system(Mavsdk::NewSystemHandle handle)
{
    _new_system_callbacks.unsubscribe(handle);
}

bool MavsdkImpl::is_any_system_connected() const
{
    std::vector<std::shared_ptr<System>> connected_systems = systems();
    return std::any_of(connected_systems.cbegin(), connected_systems.cend(), [](auto& system) {
        return system->is_connected();
    });
}

void MavsdkImpl::work_thread()
{
    while (!_should_exit) {
        timeout_handler.run_once();
        call_every_handler.run_once();

        {
            std::lock_guard<std::mutex> lock(_server_components_mutex);
            for (auto& it : _server_components) {
                if (it.second != nullptr) {
                    it.second->_impl->do_work();
                }
            }
        }

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

        const double timeout_s = 1.0;
        auto cookie = timeout_handler.add(
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
            timeout_s);
        callback.value().func();
        timeout_handler.remove(cookie);
    }
}

void MavsdkImpl::start_sending_heartbeats()
{
    // Before sending out first heartbeats we need to make sure we have a
    // default server component.
    default_server_component_impl();

    call_every_handler.remove(_heartbeat_send_cookie);
    _heartbeat_send_cookie =
        call_every_handler.add([this]() { send_heartbeat(); }, HEARTBEAT_SEND_INTERVAL_S);
}

void MavsdkImpl::stop_sending_heartbeats()
{
    if (!_configuration.get_always_send_heartbeats()) {
        call_every_handler.remove(_heartbeat_send_cookie);
    }
}

ServerComponentImpl& MavsdkImpl::default_server_component_impl()
{
    if (_default_server_component == nullptr) {
        _default_server_component = server_component_by_id(_configuration.get_component_id());
    }
    return *_default_server_component->_impl;
}

void MavsdkImpl::send_heartbeat()
{
    std::lock_guard<std::mutex> lock(_server_components_mutex);

    for (auto& it : _server_components) {
        if (it.second != nullptr) {
            it.second->_impl->send_heartbeat();
        }
    }
}

void MavsdkImpl::intercept_incoming_messages_async(std::function<bool(mavlink_message_t&)> callback)
{
    std::lock_guard<std::mutex> lock(_intercept_callback_mutex);
    _intercept_incoming_messages_callback = callback;
}

void MavsdkImpl::intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback)
{
    std::lock_guard<std::mutex> lock(_intercept_callback_mutex);
    _intercept_outgoing_messages_callback = callback;
}

uint8_t MavsdkImpl::get_target_system_id(const mavlink_message_t& message)
{
    // Checks whether connection knows target system ID by extracting target system if set.
    const mavlink_msg_entry_t* meta = mavlink_get_msg_entry(message.msgid);

    if (meta == nullptr || !(meta->flags & MAV_MSG_ENTRY_FLAG_HAVE_TARGET_SYSTEM)) {
        return 0;
    }

    // Don't look at the target system offset if it is outside the payload length.
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

    // Don't look at the target component offset if it is outside the payload length.
    // This can happen if the fields are trimmed.
    if (meta->target_component_ofs >= message.len) {
        return 0;
    }

    return (_MAV_PAYLOAD(&message))[meta->target_component_ofs];
}

Sender& MavsdkImpl::sender()
{
    return default_server_component_impl().sender();
}

} // namespace mavsdk
