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
#include "compatibility_mode.h"
#include "call_every_handler.h"
#include "component_type.h"
#include "connection.h"
#include "libmav_receiver.h"
#include <mav/BufferParser.h>
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
#include "callback_tracker.h"

// Forward declarations to avoid including MessageSet.h in header
namespace mav {
class MessageSet;
class Message;
class MessageDefinition;
class BufferParser;
} // namespace mav

namespace mavsdk {

class RawConnection; // Forward declaration

class MavsdkImpl {
public:
    MavsdkImpl(const Mavsdk::Configuration& configuration);
    ~MavsdkImpl();
    MavsdkImpl(const MavsdkImpl&) = delete;
    void operator=(const MavsdkImpl&) = delete;

    static std::string version();

    void forward_message(mavlink_message_t& message, Connection* connection);
    void receive_message(
        MavlinkReceiver::ParseResult result, mavlink_message_t& message, Connection* connection);
    void receive_libmav_message(const Mavsdk::MavlinkMessage& message, Connection* connection);

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

    Sender& sender();

    uint8_t get_mav_type() const;

    // Server identification (what MAV_AUTOPILOT we send in heartbeats)
    Autopilot get_autopilot() const;
    uint8_t get_mav_autopilot() const;

    // Compatibility mode
    CompatibilityMode get_compatibility_mode() const;

    // Returns effective autopilot for behavior decisions
    // If compatibility_mode is Auto, returns detected autopilot
    // Otherwise returns the forced mode (Pure/Px4/ArduPilot)
    Autopilot effective_autopilot(Autopilot detected) const;

    Mavsdk::NewSystemHandle subscribe_on_new_system(const Mavsdk::NewSystemCallback& callback);
    void unsubscribe_on_new_system(Mavsdk::NewSystemHandle handle);

    void notify_on_discover();
    void notify_on_timeout();

    void start_sending_heartbeats();
    void stop_sending_heartbeats();

    void intercept_incoming_messages_async(std::function<bool(mavlink_message_t&)> callback);
    void intercept_outgoing_messages_async(std::function<bool(mavlink_message_t&)> callback);

    // JSON message interception
    Mavsdk::InterceptJsonHandle
    subscribe_incoming_messages_json(const Mavsdk::InterceptJsonCallback& callback);
    void unsubscribe_incoming_messages_json(Mavsdk::InterceptJsonHandle handle);
    Mavsdk::InterceptJsonHandle
    subscribe_outgoing_messages_json(const Mavsdk::InterceptJsonCallback& callback);
    void unsubscribe_outgoing_messages_json(Mavsdk::InterceptJsonHandle handle);

    Mavsdk::ConnectionErrorHandle
    subscribe_connection_errors(Mavsdk::ConnectionErrorCallback callback);
    void unsubscribe_connection_errors(Mavsdk::ConnectionErrorHandle handle);

    // Raw bytes API
    void pass_received_raw_bytes(const char* bytes, size_t length);
    Mavsdk::RawBytesHandle subscribe_raw_bytes_to_be_sent(const Mavsdk::RawBytesCallback& callback);
    void unsubscribe_raw_bytes_to_be_sent(Mavsdk::RawBytesHandle handle);
    bool notify_raw_bytes_sent(const char* bytes, size_t length);

    std::shared_ptr<ServerComponent> server_component(unsigned instance = 0);

    std::shared_ptr<ServerComponent>
    server_component_by_type(ComponentType server_component_type, unsigned instance = 0);
    std::shared_ptr<ServerComponent> server_component_by_id(uint8_t component_id, uint8_t mav_type);

    Time time{};
    TimeoutHandler timeout_handler;
    CallEveryHandler call_every_handler;

    void call_user_callback_located(
        const std::string& filename, int linenumber, const std::function<void()>& func);

    void set_timeout_s(double timeout_s) { _timeout_s = timeout_s; }
    double timeout_s() const { return _timeout_s; }

    void set_heartbeat_timeout_s(double timeout_s) { _heartbeat_timeout_s = timeout_s; }
    double heartbeat_timeout_s() const { return _heartbeat_timeout_s; }

    MavlinkMessageHandler mavlink_message_handler{};

    ServerComponentImpl& default_server_component_impl();

    // Get connections for sending messages
    std::vector<Connection*> get_connections() const;

    // Get MessageSet for message creation and parsing
    mav::MessageSet& get_message_set() const;

    // Thread-safe MessageSet operations
    bool load_custom_xml_to_message_set(const std::string& xml_content);
    std::optional<std::string> message_id_to_name_safe(uint32_t id) const;
    std::optional<int> message_name_to_id_safe(const std::string& name) const;
    std::optional<mav::Message> create_message_safe(const std::string& message_name) const;

    // Thread-safe MessageSet operations for LibmavReceiver
    std::optional<mav::Message>
    parse_message_safe(const uint8_t* buffer, size_t buffer_len, size_t& bytes_consumed) const;
    mav::OptionalReference<const mav::MessageDefinition>
    get_message_definition_safe(int message_id) const;

private:
    static constexpr float DEFAULT_TIMEOUT_S = 0.5f;
    static constexpr double DEFAULT_HEARTBEAT_TIMEOUT_S = 3.0;

    std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
    add_udp_connection(const CliArg::Udp& udp, ForwardingOption forwarding_option);

    std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
    add_tcp_connection(const CliArg::Tcp& tcp, ForwardingOption forwarding_option);
    std::pair<ConnectionResult, Mavsdk::ConnectionHandle> add_serial_connection(
        const std::string& dev_path,
        int baudrate,
        bool flow_control,
        ForwardingOption forwarding_option);
    std::pair<ConnectionResult, Mavsdk::ConnectionHandle>
    add_raw_connection(ForwardingOption forwarding_option);

    Mavsdk::ConnectionHandle add_connection(std::unique_ptr<Connection>&& connection);
    void make_system_with_component(uint8_t system_id, uint8_t component_id);

    void send_heartbeats();

    void work_thread();
    void process_user_callbacks_thread();

    void process_messages();
    void process_message(mavlink_message_t& message, Connection* connection);

    void process_libmav_messages();
    void process_libmav_message(const Mavsdk::MavlinkMessage& message, Connection* connection);

    void deliver_messages();
    void deliver_message(mavlink_message_t& message);

    bool is_any_system_connected() const;

    std::shared_ptr<ServerComponent>
    server_component_by_id_with_lock(uint8_t component_id, uint8_t mav_type);
    ServerComponentImpl& default_server_component_with_lock();

    static uint8_t get_target_system_id(const mavlink_message_t& message);
    static uint8_t get_target_component_id(const mavlink_message_t& message);

    // Helper methods for JSON message conversion
    bool call_json_interception_callbacks(
        const Mavsdk::MavlinkMessage& json_message,
        std::vector<std::pair<Mavsdk::InterceptJsonHandle, Mavsdk::InterceptJsonCallback>>&
            callback_list);

    // Find the raw connection in the connections list
    RawConnection* find_raw_connection();

    mutable std::recursive_mutex _mutex{};

    // Message set for libmav message handling (shared across all connections)
    std::unique_ptr<mav::MessageSet> _message_set;
    std::unique_ptr<mav::BufferParser> _buffer_parser; // Thread-safe parser
    mutable std::mutex _message_set_mutex;

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
    std::unique_ptr<CallbackTracker> _callback_tracker;

    mutable std::mutex _intercept_callbacks_mutex{};
    std::function<bool(mavlink_message_t&)> _intercept_incoming_messages_callback{nullptr};
    std::function<bool(mavlink_message_t&)> _intercept_outgoing_messages_callback{nullptr};

    // JSON message interception
    std::vector<std::pair<Mavsdk::InterceptJsonHandle, Mavsdk::InterceptJsonCallback>>
        _incoming_json_message_subscriptions{};
    std::vector<std::pair<Mavsdk::InterceptJsonHandle, Mavsdk::InterceptJsonCallback>>
        _outgoing_json_message_subscriptions{};
    mutable std::mutex _json_subscriptions_mutex{};
    HandleFactory<bool(Mavsdk::MavlinkMessage)> _json_handle_factory{};

    // Raw bytes subscriptions
    CallbackList<const char*, size_t> _raw_bytes_subscriptions{};

    std::atomic<double> _timeout_s{DEFAULT_TIMEOUT_S};
    std::atomic<double> _heartbeat_timeout_s{DEFAULT_HEARTBEAT_TIMEOUT_S};

    struct ReceivedMessage {
        mavlink_message_t message;
        Connection* connection_ptr;
    };
    mutable std::mutex _received_messages_mutex{};
    std::queue<ReceivedMessage> _received_messages;
    std::condition_variable _received_messages_cv{};

    struct ReceivedLibmavMessage {
        Mavsdk::MavlinkMessage message;
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
