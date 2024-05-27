#pragma once

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>
#include "autopilot.h"
#include "autopilot_callback.h"
#include "mavlink_address.h"
#include "mavlink_include.h"
#include "mavlink_message_handler.h"
#include "timeout_handler.h"
#include "timeout_s_callback.h"
#include "locked_queue.h"
#include "sender.h"

namespace mavsdk {

class MavlinkMissionTransferClient {
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

    class UploadWorkItem : public WorkItem {
    public:
        explicit UploadWorkItem(
            Sender& sender,
            MavlinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            const std::vector<ItemInt>& items,
            double timeout_s,
            ResultCallback callback,
            ProgressCallback progress_callback,
            bool debugging,
            uint8_t target_system_id,
            Autopilot autopilot);

        ~UploadWorkItem() override;
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

        void update_progress(float progress);

        enum class Step {
            SendCount,
            SendItems,
        } _step{Step::SendCount};

        std::vector<ItemInt> _items{};
        ResultCallback _callback{nullptr};
        ProgressCallback _progress_callback{nullptr};
        std::size_t _next_sequence{0};
        TimeoutHandler::Cookie _cookie{};
        unsigned _retries_done{0};

        uint8_t _target_system_id;
        Autopilot _autopilot;
    };

    class DownloadWorkItem : public WorkItem {
    public:
        explicit DownloadWorkItem(
            Sender& sender,
            MavlinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            double timeout_s,
            ResultAndItemsCallback callback,
            ProgressCallback progress_callback,
            bool debugging,
            uint8_t target_system_id);

        ~DownloadWorkItem() override;
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

        void update_progress(float progress);

        enum class Step {
            RequestList,
            RequestItem,
        } _step{Step::RequestList};

        std::vector<ItemInt> _items{};
        ResultAndItemsCallback _callback{nullptr};
        ProgressCallback _progress_callback{nullptr};
        TimeoutHandler::Cookie _cookie{};
        std::size_t _next_sequence{0};
        std::size_t _expected_count{0};
        unsigned _retries_done{0};
        uint8_t _target_system_id;
    };

    class ClearWorkItem : public WorkItem {
    public:
        ClearWorkItem(
            Sender& sender,
            MavlinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            uint8_t type,
            double timeout_s,
            ResultCallback callback,
            bool debugging,
            uint8_t target_system_id);

        ~ClearWorkItem() override;
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
        TimeoutHandler::Cookie _cookie{};
        unsigned _retries_done{0};
        uint8_t _target_system_id;
    };

    class SetCurrentWorkItem : public WorkItem {
    public:
        SetCurrentWorkItem(
            Sender& sender,
            MavlinkMessageHandler& message_handler,
            TimeoutHandler& timeout_handler,
            int current,
            double timeout_s,
            ResultCallback callback,
            bool debugging,
            uint8_t target_system_id);

        ~SetCurrentWorkItem() override;
        void start() override;
        void cancel() override;

        SetCurrentWorkItem(const SetCurrentWorkItem&) = delete;
        SetCurrentWorkItem(SetCurrentWorkItem&&) = delete;
        SetCurrentWorkItem& operator=(const SetCurrentWorkItem&) = delete;
        SetCurrentWorkItem& operator=(SetCurrentWorkItem&&) = delete;

    private:
        void send_current_mission_item();

        void process_mission_current(const mavlink_message_t& message);
        void process_timeout();
        void callback_and_reset(Result result);

        int _current{0};
        ResultCallback _callback{nullptr};
        TimeoutHandler::Cookie _cookie{};
        unsigned _retries_done{0};
        uint8_t _target_system_id;
    };

    static constexpr unsigned retries = 5;

    explicit MavlinkMissionTransferClient(
        Sender& sender,
        MavlinkMessageHandler& message_handler,
        TimeoutHandler& timeout_handler,
        TimeoutSCallback get_timeout_s_callback,
        AutopilotCallback autopilot_callback);

    ~MavlinkMissionTransferClient() = default;

    std::weak_ptr<WorkItem> upload_items_async(
        uint8_t type,
        uint8_t target_system_id,
        const std::vector<ItemInt>& items,
        const ResultCallback& callback,
        const ProgressCallback& progress_callback = nullptr);

    std::weak_ptr<WorkItem> download_items_async(
        uint8_t type,
        uint8_t target_system_id,
        ResultAndItemsCallback callback,
        ProgressCallback progress_callback = nullptr);

    void clear_items_async(uint8_t type, uint8_t target_system_id, ResultCallback callback);

    void set_current_item_async(int current, uint8_t target_system_id, ResultCallback callback);

    void do_work();
    bool is_idle();

    void set_int_messages_supported(bool supported);

    // Non-copyable
    MavlinkMissionTransferClient(const MavlinkMissionTransferClient&) = delete;
    const MavlinkMissionTransferClient& operator=(const MavlinkMissionTransferClient&) = delete;

private:
    Sender& _sender;
    MavlinkMessageHandler& _message_handler;
    TimeoutHandler& _timeout_handler;
    TimeoutSCallback _timeout_s_callback;
    AutopilotCallback _autopilot_callback;

    LockedQueue<WorkItem> _work_queue{};

    bool _int_messages_supported{true};
    bool _debugging{false};
};

} // namespace mavsdk
