#pragma once

#include <atomic>
#include <cstdint>
#include <mutex>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <atomic>
#include <thread>
#include <queue>

#include "autopilot.h"
#include "call_every_handler.h"
#include "component_type.h"
#include "connection.h"
#include "libmav_receiver.h"
#include "cli_arg.h"
#include "handle_factory.h"
#include "handle.h"
#include "mavsdk.h"
#include "mavlink_include.h"
#include "mavlink_address.h"
#include "mavlink_message_handler.h"
#include "mavlink_command_receiver.h"
#include "locked_queue.h"
#include "server_component.h"
#include "system.h"
#include "sender.h"
#include "timeout_handler.h"
#include "callback_list.h"

// Forward declarations to avoid including MessageSet.h in header
namespace mav {
class MessageSet;
}

namespace mavsdk {

class MavsdkImpl {
public:
    MavsdkImpl(const Mavsdk::Configuration& configuration);
    ~MavsdkImpl();
    MavsdkImpl(const MavsdkImpl&) = delete;
    void operator=(const MavsdkImpl&) = delete;

    static std::string version();

    void forward_message(mavlink_message_t& message, Connection* connection);
    void receive_message(mavlink_message_t& message, Connection* connection);
    void receive_libmav_message(const LibmavMessage& message, Connection* connection);

    std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
    add_any_connection(const std::string& connection_url, ForwardingOption forwarding_option);
    void remove_connection(Mavsdk::ConnectionHandle handle);

    std::vector<std::shared_ptr<System>> systems() const;

    std::optional<std::shared_ptr<System>> first_autopilot(double timeout_s);

    void set_configuration(Mavsdk::Configuration new_configuration);
    Mavsdk::Configuration get_configuration() const;

    bool send_message(mavlink_message_t& message);
    uint8_t get_own_system_id() const;
    uint8_t get_own_component_id() const;
    uint8_t channel() const;
    Autopilot autopilot() const;

    Sender& sender();

    uint8_t get_mav_type() const;

    Mavsdk::NewSystemHandle subscribe_on_new_system(const Mavsdk::NewSystemCallback& callback);
    void unsubscribe_on_new_system(Mavsdk::NewSystemHandle handle);

    void notify_on_discover();
    void notify_on_timeout();

    void start_sending_heartbeats();
    void stop_sending_heartbeats();

    void intercept_incoming_messages_async(std::function<bool(mavlink_message_t&)> callback);
    void intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback);

    Mavsdk::ConnectionErrorHandle
    subscribe_connection_errors(Mavsdk::ConnectionErrorCallback callback);
    void unsubscribe_connection_errors(Mavsdk::ConnectionErrorHandle handle);

    std::shared_ptr<ServerComponent> server_component(unsigned instance = 0);

    std::shared_ptr<ServerComponent>
    server_component_by_type(ComponentType server_component_type, unsigned instance = 0);
    std::shared_ptr<ServerComponent> server_component_by_id(uint8_t component_id);

    Time time{};
    TimeoutHandler timeout_handler;
    CallEveryHandler call_every_handler;

    void call_user_callback_located(
        const std::string& filename, int linenumber, const std::function<void()>& func);

    void set_timeout_s(double timeout_s) { _timeout_s = timeout_s; }

    double timeout_s() const { return _timeout_s; };

    MavlinkMessageHandler mavlink_message_handler{};

    ServerComponentImpl& default_server_component_impl();

    // Get connections for sending messages
    std::vector<Connection*> get_connections() const;

    // Get MessageSet for message creation and parsing
    mav::MessageSet& get_message_set() const;

private:
    static constexpr float DEFAULT_TIMEOUT_S = 0.5f;

    std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
    add_udp_connection(const CliArg::Udp& udp, ForwardingOption forwarding_option);

    std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
    add_tcp_connection(const CliArg::Tcp& tcp, ForwardingOption forwarding_option);
    std::pair<ConnectionResult, Mavsdk::ConnectionHandle> add_serial_connection(
        const std::string& dev_path,
        int baudrate,
        bool flow_control,
        ForwardingOption forwarding_option);

    Mavsdk::ConnectionHandle add_connection(std::unique_ptr<Connection>&& connection);
    void make_system_with_component(uint8_t system_id, uint8_t component_id);

    void send_heartbeats();

    void work_thread();
    void process_user_callbacks_thread();

    void process_messages();
    void process_message(mavlink_message_t& message, Connection* connection);

    void process_libmav_messages();
    void process_libmav_message(const LibmavMessage& message, Connection* connection);

    void deliver_messages();
    void deliver_message(mavlink_message_t& message);

    bool is_any_system_connected() const;

    std::shared_ptr<ServerComponent> server_component_by_id_with_lock(uint8_t component_id);
    ServerComponentImpl& default_server_component_with_lock();

    static uint8_t get_target_system_id(const mavlink_message_t& message);
    static uint8_t get_target_component_id(const mavlink_message_t& message);

    mutable std::recursive_mutex _mutex{};

    // Message set for libmav message handling (shared across all connections)
    std::unique_ptr<mav::MessageSet> _message_set;

    HandleFactory<> _connections_handle_factory;
    struct ConnectionEntry {
        std::unique_ptr<Connection> connection;
        Handle<> handle;
    };
    std::vector<ConnectionEntry> _connections{};
    CallbackList<Mavsdk::ConnectionError> _connections_errors_subscriptions{};

    std::vector<std::pair<uint8_t, std::shared_ptr<System>>> _systems{};

    std::recursive_mutex _server_components_mutex;
    std::vector<std::pair<uint8_t, std::shared_ptr<ServerComponent>>> _server_components{};
    std::shared_ptr<ServerComponent> _default_server_component{nullptr};

    CallbackList<> _new_system_callbacks{};

    Mavsdk::Configuration _configuration{ComponentType::GroundStation};
    std::atomic<uint8_t> _our_system_id{0};
    std::atomic<uint8_t> _our_component_id{0};

    struct UserCallback {
        UserCallback() = default;
        explicit UserCallback(std::function<void()> func_) : func(std::move(func_)) {}
        UserCallback(std::function<void()> func_, std::string filename_, const int linenumber_) :
            func(std::move(func_)),
            filename(std::move(filename_)),
            linenumber(linenumber_)
        {}

        std::function<void()> func{};
        std::string filename{};
        int linenumber{};
    };

    std::thread* _work_thread{nullptr};
    std::thread* _process_user_callbacks_thread{nullptr};
    LockedQueue<UserCallback> _user_callback_queue{};

    bool _message_logging_on{false};
    bool _callback_debugging{false};
    bool _system_debugging{false};

    mutable std::mutex _intercept_callbacks_mutex{};
    std::function<bool(mavlink_message_t&)> _intercept_incoming_messages_callback{nullptr};
    std::function<bool(mavlink_message_t&)> _intercept_outgoing_messages_callback{nullptr};

    std::atomic<double> _timeout_s{DEFAULT_TIMEOUT_S};

    struct ReceivedMessage {
        mavlink_message_t message;
        Connection* connection_ptr;
    };
    mutable std::mutex _received_messages_mutex{};
    std::queue<ReceivedMessage> _received_messages;
    std::condition_variable _received_messages_cv{};

    struct ReceivedLibmavMessage {
        LibmavMessage message;
        Connection* connection_ptr;
    };
    mutable std::mutex _received_libmav_messages_mutex{};
    std::queue<ReceivedLibmavMessage> _received_libmav_messages;
    std::condition_variable _received_libmav_messages_cv{};

    mutable std::mutex _messages_to_send_mutex{};
    std::queue<mavlink_message_t> _messages_to_send;

    static constexpr double HEARTBEAT_SEND_INTERVAL_S = 1.0;
    std::mutex _heartbeat_mutex{};
    CallEveryHandler::Cookie _heartbeat_send_cookie{};

    std::atomic<bool> _should_exit{false};
};

} // namespace mavsdk
