#pragma once

#include "mavlink_include.h"
#include "plugins/log_files/log_files.h"
#include "plugin_impl_base.h"
#include "system.h"
#include <fstream>
#include <optional>

namespace mavsdk {

// We need to keep this smaller, otherwise we end up running over the UDP buffer in Linux when PX4
// sends too much all at once.
static constexpr uint32_t TABLE_BINS = 128;
static constexpr uint32_t CHUNK_SIZE = (TABLE_BINS * MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN);

struct LogData {
    LogData() = default;
    LogData(
        const LogFiles::Entry& e,
        const std::string& filepath,
        LogFiles::DownloadLogFileCallback cb);

    bool file_is_open();
    uint32_t current_chunk_size() const;
    uint32_t total_chunks() const;
    uint32_t bins_in_chunk() const;

    LogFiles::Entry entry{};

    std::string file_path{};
    std::ofstream file{};

    uint32_t current_chunk{};
    std::vector<bool> chunk_bin_table{};

    uint32_t total_bytes_written{};

    TimeoutHandler::Cookie timeout_cookie{};

    LogFiles::DownloadLogFileCallback user_callback{};
};

class LogFilesImpl : public PluginImplBase {
public:
    explicit LogFilesImpl(System& system);
    explicit LogFilesImpl(std::shared_ptr<System> system);
    ~LogFilesImpl() override;

    void init() override;
    void deinit() override;

    void enable() override {};
    void disable() override {};

    std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> get_entries();
    void get_entries_async(LogFiles::GetEntriesCallback callback);

    std::pair<LogFiles::Result, LogFiles::ProgressData>
    download_log_file(LogFiles::Entry entry, const std::string& file_path);

    void download_log_file_async(
        LogFiles::Entry entry,
        const std::string& file_path,
        LogFiles::DownloadLogFileCallback callback);

    LogFiles::Result erase_all_log_files();

private:
    void process_log_entry(const mavlink_message_t& message);
    void entries_timeout();

    void process_log_data(const mavlink_message_t& message);
    void data_timeout();
    void check_and_request_missing_bins();
    void check_and_request_missing_entries();

    void request_log_list(uint16_t index_min, uint16_t index_max);
    void request_log_data(unsigned id, unsigned start, unsigned count);

    void request_end();

    std::mutex _entries_mutex;
    std::vector<std::optional<LogFiles::Entry>> _log_entries;
    uint32_t _total_entries{0};
    uint32_t _entries_retry_count{0};
    uint16_t _min_entry_id{0xFFFF}; // Track min ID to detect 0-based vs 1-based indexing
    TimeoutHandler::Cookie _entries_timeout_cookie{};
    LogFiles::GetEntriesCallback _entries_user_callback{};

    // The current log download data structure
    std::mutex _download_data_mutex;
    LogData _download_data;
};

} // namespace mavsdk
