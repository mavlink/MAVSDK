#include <cstdint>
#include <vector>
#include "mavlink_include.h"

namespace mavsdk {

class Sender {
public:
    virtual ~Sender() = default;
    virtual bool send_message(const mavlink_message_t& message) = 0;
};

class FakeSender : public Sender {
public:
    virtual ~FakeSender() = default;

    bool send_message(const mavlink_message_t& message) override
    {
        if (able_to_send) {
            _last_message = message;
            return true;
        } else {
            return false;
        }
    }

    const mavlink_message_t& last_message() const
    {
        return _last_message;
    }

    bool able_to_send {true};

private:
    mavlink_message_t _last_message {};
};

class Receiver {
public:
    virtual ~Receiver() = default;
};

class FakeReceiver : public Receiver {
public:
    virtual ~FakeReceiver() = default;
};

class Timeouter {
public:
    virtual ~Timeouter() = default;

};

class FakeTimeouter : public Timeouter {
public:
    virtual ~FakeTimeouter() = default;
};

class MAVLinkMissionTransfer {
public:

    enum class Result {
        Success,
        ConnectionError,
        TooManyMissionItems,
        Timeout,
        Unsupported,
        NoMissionAvailable,
        Cancelled
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

    using ResultCallback = void(Result result);

    MAVLinkMissionTransfer(Sender& sender, Receiver& receiver, Timeouter& timeouter) :
        _sender(sender),
        _receiver(receiver),
        _timeouter(timeouter)
    {}


    void upload_items_async(const std::vector<ItemInt>& items, ResultCallback callback);


private:
    Sender& _sender;
    Receiver& _receiver;
    Timeouter& _timeouter;
};


} // namespace mavsdk
