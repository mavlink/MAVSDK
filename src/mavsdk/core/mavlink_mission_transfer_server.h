#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include "mavlink_address.h"
#include "mavlink_include.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "timeout_s_callback.h"
#include "locked_queue.h"
#include "sender.h"

namespace mavsdk {

class MavlinkMissionTransferServer {
public:
    enum class Result {
        Success,
        ConnectionError,
        Denied,
        TooManyMissionItems,
        Timeout,
        Unsupported,
        UnsupportedFrame,
        NoMissionAvailable,
        Cancelled,
        MissionTypeNotConsistent,
        InvalidSequence,
        CurrentInvalid,
        ProtocolError,
        InvalidParam,
        IntMessagesNotSupported
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

        bool operator==(const ItemInt& other) const
        {
            return (
                seq == other.seq && frame == other.frame && command == other.command &&
                current == other.current && autocontinue == other.autocontinue &&
                param1 == other.param1 && param2 == other.param2 && param3 == other.param3 &&
                param4 == other.param4 && x == other.x && y == other.y && z == other.z &&
                mission_type == other.mission_type);
        }
    };

    using ResultCallback = std::function<void(Result result)>;
    using ResultAndItemsCallback = std::function<void(Result result, std::vector<ItemInt> items)>;
    using ProgressCallback = std::function<void(float progress)>;

    class WorkItem {
    public:
        explicit WorkItem(
            Sender& sender,
            MavlinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            double timeout_s,
            bool debugging);
        virtual ~WorkItem();
        virtual void start() = 0;
        virtual void cancel() = 0;
        bool has_started();
        bool is_done();

        WorkItem(const WorkItem&) = delete;
        WorkItem(WorkItem&&) = delete;
        WorkItem& operator=(const WorkItem&) = delete;
        WorkItem& operator=(WorkItem&&) = delete;

    protected:
        Sender& _sender;
        MavlinkMessageHandler& _message_handler;
        TimeoutHandler& _timeout_handler;
        uint8_t _type;
        double _timeout_s;
        bool _started{false};
        bool _done{false};
        std::mutex _mutex{};
        bool _debugging;
    };

    class ReceiveIncomingMission : public WorkItem {
    public:
        explicit ReceiveIncomingMission(
            Sender& sender,
            MavlinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            double timeout_s,
            ResultAndItemsCallback callback,
            uint32_t mission_count,
            uint8_t target_system_id,
            uint8_t target_component_id,
            bool debugging);
        ~ReceiveIncomingMission() override;

        void start() override;
        void cancel() override;

        ReceiveIncomingMission(const ReceiveIncomingMission&) = delete;
        ReceiveIncomingMission(ReceiveIncomingMission&&) = delete;
        ReceiveIncomingMission& operator=(const ReceiveIncomingMission&) = delete;
        ReceiveIncomingMission& operator=(ReceiveIncomingMission&&) = delete;

    private:
        void request_item();
        void send_ack_and_finish();
        void send_cancel_and_finish();
        void process_mission_count();
        void process_mission_item_int(const mavlink_message_t& message);
        void process_timeout();
        void callback_and_reset(Result result);

        enum class Step {
            RequestList,
            RequestItem,
        } _step{Step::RequestList};

        std::vector<ItemInt> _items{};
        ResultAndItemsCallback _callback{nullptr};
        TimeoutHandler::Cookie _cookie{};
        std::size_t _next_sequence{0};
        std::size_t _expected_count{0};
        unsigned _retries_done{0};
        uint32_t _mission_count{0};
        uint8_t _target_system_id{0};
        uint8_t _target_component_id{0};
    };
    static constexpr unsigned retries = 5;

    explicit MavlinkMissionTransferServer(
        Sender& sender,
        MavlinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler,
        TimeoutSCallback get_timeout_s_callback);

    ~MavlinkMissionTransferServer() = default;

    std::weak_ptr<WorkItem> receive_incoming_items_async(
        uint8_t type,
        uint32_t mission_count,
        uint8_t target_system,
        uint8_t target_component,
        ResultAndItemsCallback callback);

    void do_work();
    bool is_idle();

    void set_int_messages_supported(bool supported);

    // Non-copyable
    MavlinkMissionTransferServer(const MavlinkMissionTransferServer&) = delete;
    const MavlinkMissionTransferServer& operator=(const MavlinkMissionTransferServer&) = delete;

private:
    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;
    TimeoutSCallback _timeout_s_callback;

    LockedQueue<WorkItem> _work_queue{};

    bool _int_messages_supported{true};
    bool _debugging{false};
};

} // namespace mavsdk
