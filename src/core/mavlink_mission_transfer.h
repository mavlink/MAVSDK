#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>
#include "mavlink_address.h"
#include "mavlink_include.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "locked_queue.h"

namespace mavsdk {

class Sender {
public:
    Sender(MAVLinkAddress& new_own_address, MAVLinkAddress& new_target_address) :
        own_address(new_own_address),
        target_address(new_target_address)
    {}
    virtual ~Sender() = default;
    virtual bool send_message(mavlink_message_t& message) = 0;
    MAVLinkAddress& own_address;
    MAVLinkAddress& target_address;
};

class MAVLinkMissionTransfer {
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

    class WorkItem {
    public:
        WorkItem(
            Sender& sender,
            MAVLinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type);
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
        MAVLinkMessageHandler& _message_handler;
        TimeoutHandler& _timeout_handler;
        uint8_t _type;
        bool _started{false};
        bool _done{false};
    };

    class UploadWorkItem : public WorkItem {
    public:
        UploadWorkItem(
            Sender& sender,
            MAVLinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            const std::vector<ItemInt>& items,
            ResultCallback callback);

        virtual ~UploadWorkItem();
        void start() override;
        void cancel() override;

        UploadWorkItem(const UploadWorkItem&) = delete;
        UploadWorkItem(UploadWorkItem&&) = delete;
        UploadWorkItem& operator=(const UploadWorkItem&) = delete;
        UploadWorkItem& operator=(UploadWorkItem&&) = delete;

    private:
        void send_count();
        void send_mission_item();
        void send_cancel_and_finish();

        void process_mission_request(const mavlink_message_t& message);
        void process_mission_request_int(const mavlink_message_t& message);
        void process_mission_ack(const mavlink_message_t& message);
        void process_timeout();
        void callback_and_reset(Result result);

        enum class Step {
            SendCount,
            SendItems,
        } _step{Step::SendCount};

        std::vector<ItemInt> _items{};
        ResultCallback _callback{nullptr};
        std::size_t _next_sequence{0};
        void* _cookie{nullptr};
        unsigned _retries_done{0};
    };

    class DownloadWorkItem : public WorkItem {
    public:
        DownloadWorkItem(
            Sender& sender,
            MAVLinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            ResultAndItemsCallback callback);

        virtual ~DownloadWorkItem();
        void start() override;
        void cancel() override;

        DownloadWorkItem(const DownloadWorkItem&) = delete;
        DownloadWorkItem(DownloadWorkItem&&) = delete;
        DownloadWorkItem& operator=(const DownloadWorkItem&) = delete;
        DownloadWorkItem& operator=(DownloadWorkItem&&) = delete;

    private:
        void request_list();
        void request_item();
        void send_ack_and_finish();
        void send_cancel_and_finish();
        void process_mission_count(const mavlink_message_t& message);
        void process_mission_item_int(const mavlink_message_t& message);
        void process_timeout();
        void callback_and_reset(Result result);

        enum class Step {
            RequestList,
            RequestItem,
        } _step{Step::RequestList};

        std::vector<ItemInt> _items{};
        ResultAndItemsCallback _callback{nullptr};
        void* _cookie{nullptr};
        std::size_t _next_sequence{0};
        std::size_t _expected_count{0};
        unsigned _retries_done{0};
    };

    class ClearWorkItem : public WorkItem {
    public:
        ClearWorkItem(
            Sender& sender,
            MAVLinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            ResultCallback callback);

        virtual ~ClearWorkItem();
        void start() override;
        void cancel() override;

        ClearWorkItem(const ClearWorkItem&) = delete;
        ClearWorkItem(ClearWorkItem&&) = delete;
        ClearWorkItem& operator=(const ClearWorkItem&) = delete;
        ClearWorkItem& operator=(ClearWorkItem&&) = delete;

    private:
        void send_clear();
        void process_mission_ack(const mavlink_message_t& message);
        void process_timeout();
        void callback_and_reset(Result result);

        ResultCallback _callback{nullptr};
        void* _cookie{nullptr};
        unsigned _retries_done{0};
    };

    static constexpr double timeout_s = 0.5;
    static constexpr unsigned retries = 4;

    MAVLinkMissionTransfer(
        Sender& sender, MAVLinkMessageHandler& message_handler, TimeoutHandler& timeout_handler);

    ~MAVLinkMissionTransfer();

    std::weak_ptr<WorkItem>
    upload_items_async(uint8_t type, const std::vector<ItemInt>& items, ResultCallback callback);

    std::weak_ptr<WorkItem> download_items_async(uint8_t type, ResultAndItemsCallback callback);

    void clear_items_async(uint8_t type, ResultCallback callback);

    void do_work();
    bool is_idle();

    // Non-copyable
    MAVLinkMissionTransfer(const MAVLinkMissionTransfer&) = delete;
    const MAVLinkMissionTransfer& operator=(const MAVLinkMissionTransfer&) = delete;

private:
    Sender& _sender;
    MAVLinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;

    LockedQueue<WorkItem> _work_queue{};
};

} // namespace mavsdk
