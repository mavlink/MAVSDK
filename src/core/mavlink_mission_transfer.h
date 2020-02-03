#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <vector>
#include "mavlink_include.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "locked_queue.h"

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
    };

    static constexpr double timeout_s = 0.5;
    static constexpr unsigned retries = 4;

    MAVLinkMissionTransfer(
        Config config,
        Sender& sender,
        MAVLinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler);

    ~MAVLinkMissionTransfer();

    using ResultCallback = std::function<void(Result result)>;
    void
    upload_items_async(uint8_t type, const std::vector<ItemInt>& items, ResultCallback callback);

    using ResultAndItemsCallback = std::function<void(Result result, std::vector<ItemInt> items)>;
    void download_items_async(uint8_t type, ResultAndItemsCallback callback);

    void do_work();

    // Non-copyable
    MAVLinkMissionTransfer(const MAVLinkMissionTransfer&) = delete;
    const MAVLinkMissionTransfer& operator=(const MAVLinkMissionTransfer&) = delete;

private:
    class WorkItem {
    public:
        WorkItem(
            Config config,
            Sender& sender,
            MAVLinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type);
        virtual ~WorkItem();
        virtual void start() = 0;
        bool has_started();
        bool is_done();

        WorkItem(const WorkItem&) = delete;
        WorkItem(WorkItem&&) = delete;
        WorkItem& operator=(const WorkItem&) = delete;
        WorkItem& operator=(WorkItem&&) = delete;

    protected:
        Config _config;
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
            Config config,
            Sender& sender,
            MAVLinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            const std::vector<ItemInt>& items,
            ResultCallback callback);

        virtual ~UploadWorkItem();
        void start() override;

        UploadWorkItem(const UploadWorkItem&) = delete;
        UploadWorkItem(UploadWorkItem&&) = delete;
        UploadWorkItem& operator=(const UploadWorkItem&) = delete;
        UploadWorkItem& operator=(UploadWorkItem&&) = delete;

    private:
        void send_count();
        void send_mission_item();
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
            Config config,
            Sender& sender,
            MAVLinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            ResultAndItemsCallback callback);

        virtual ~DownloadWorkItem();
        void start() override;

        DownloadWorkItem(const DownloadWorkItem&) = delete;
        DownloadWorkItem(DownloadWorkItem&&) = delete;
        DownloadWorkItem& operator=(const DownloadWorkItem&) = delete;
        DownloadWorkItem& operator=(DownloadWorkItem&&) = delete;

    private:
        void request_list();
        void request_item();
        void send_ack_and_finish();
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

    Config _config;
    Sender& _sender;
    MAVLinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;

    LockedQueue<WorkItem> _work_queue{};
};

} // namespace mavsdk
