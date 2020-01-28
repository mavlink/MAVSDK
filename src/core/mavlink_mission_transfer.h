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
        uint16_t seq;
        uint8_t frame;
        uint16_t command;
        uint8_t current;
        uint8_t autocontinue;
        float param1;
        float param2;
        float param3;
        float param4;
        int32_t x;
        int32_t y;
        float z;
        uint8_t mission_type;
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
