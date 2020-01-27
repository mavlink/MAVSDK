#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <vector>
#include "mavlink_include.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"

namespace mavsdk {

class Sender {
public:
    virtual ~Sender() = default;
    virtual bool send_message(const mavlink_message_t& message) = 0;
};

class MAVLinkMissionTransfer {
public:
    struct Config {
        uint8_t own_system_id;
        uint8_t own_component_id;
        uint8_t target_system_id;
        uint8_t target_component_id;
    };

    enum class Result {
        Success,
        ConnectionError,
        TooManyMissionItems,
        Timeout,
        Unsupported,
        NoMissionAvailable,
        Cancelled,
        MissionTypeNotConsistent,
        WrongSequence,
        CurrentInvalid,
    };

    struct ItemInt {
        uint8_t target_system; /**< @brief System ID. */
        uint8_t target_component; /**< @brief Component ID. */
        uint16_t seq; /**< @brief Sequence. */
        uint8_t frame; /**< @brief The coordinate system of the waypoint. */
        uint16_t command; /**< @brief The scheduled action for the waypoint. */
        uint8_t current; /**< @brief false:0, true:1. */
        uint8_t autocontinue; /**< @brief Autocontinue to next waypoint. */
        float param1; /**< @brief PARAM1, see MAV_CMD enum. */
        float param2; /**< @brief PARAM2, see MAV_CMD enum. */
        float param3; /**< @brief PARAM3, see MAV_CMD enum. */
        float param4; /**< @brief PARAM4, see MAV_CMD enum. */
        int32_t x; /**< @brief PARAM5 / local: x position in meters * 1e4, global: latitude in
                      degrees * 10^7. */
        int32_t y; /**< @brief PARAM6 / y position: local: x position in meters * 1e4, global:
                      longitude in degrees *10^7. */
        float z; /**< @brief PARAM7 / local: Z coordinate, global: altitude (relative or absolute,
                    depending on frame). */
        uint8_t mission_type; /**< @brief Mission type. */
    };

    static constexpr double timeout_s = 1.0;

    using ResultCallback = std::function<void(Result result)>;

    MAVLinkMissionTransfer(
        Config config,
        Sender& sender,
        MAVLinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler);

    ~MAVLinkMissionTransfer();

    void upload_items_async(const std::vector<ItemInt>& items, ResultCallback callback);

    // Non-copyable
    MAVLinkMissionTransfer(const MAVLinkMissionTransfer&) = delete;
    const MAVLinkMissionTransfer& operator=(const MAVLinkMissionTransfer&) = delete;

private:
    void process_mission_request_int(const mavlink_message_t& message);
    void process_timeout();

    Config _config;
    Sender& _sender;
    MAVLinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;

    int _next_sequence_expected{-1};
    void* _cookie{nullptr};
    ResultCallback _callback{nullptr};
    std::vector<ItemInt> _items{};
};

} // namespace mavsdk
