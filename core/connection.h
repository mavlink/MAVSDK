#pragma once

//#include "dronecode_sdk.h"
#include "mavlink_receiver.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>

namespace dronecode_sdk {

class Connection {
public:
    typedef std::function<void(const mavlink_message_t &message)> receiver_callback_t;

    Connection(receiver_callback_t receiver_callback);
    virtual ~Connection();

    /**
     * @brief Result type returned when adding a connection.
     *
     * **Note**: DronecodeSDK does not throw exceptions. Instead a result of this type will be
     * returned when you add a connection: add_udp_connection().
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Connection succeeded. */
        TIMEOUT, /**< @brief %Connection timed out. */
        SOCKET_ERROR, /**< @brief Socket error. */
        BIND_ERROR, /**< @brief Bind error. */
        SOCKET_CONNECTION_ERROR, /**< @brief Socket connection error. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        NOT_IMPLEMENTED, /**< @brief %Connection type not implemented. */
        SYSTEM_NOT_CONNECTED, /**< @brief No system is connected. */
        SYSTEM_BUSY, /**< @brief %System is busy. */
        COMMAND_DENIED, /**< @brief Command is denied. */
        DESTINATION_IP_UNKNOWN, /**< @brief %Connection IP is unknown. */
        CONNECTIONS_EXHAUSTED, /**< @brief %Connections exhausted. */
        CONNECTION_URL_INVALID /**< @brief URL invalid. */
    };

    virtual Result start() = 0;
    virtual Result stop() = 0;

    virtual bool send_message(const mavlink_message_t &message) = 0;

    inline const char *result_str(const Result result);

    // Non-copyable
    Connection(const Connection &) = delete;
    const Connection &operator=(const Connection &) = delete;

protected:
    bool start_mavlink_receiver();
    void stop_mavlink_receiver();
    void receive_message(const mavlink_message_t &message);

    receiver_callback_t _receiver_callback{};
    std::unique_ptr<MAVLinkReceiver> _mavlink_receiver;

    // void received_mavlink_message(mavlink_message_t &);
};

} // namespace dronecode_sdk
