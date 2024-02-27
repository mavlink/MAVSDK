#pragma once

#include "mavlink_include.h"
#include "plugins/log_files/log_files.h"
#include "plugin_impl_base.h"
#include "system.h"
#include <fstream>

namespace mavsdk {

static constexpr uint32_t kTableBins = 512;
static constexpr uint32_t kChunkSize = (kTableBins * MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN);

struct LogData {
    LogData() = default;
    LogData(
        const LogFiles::Entry& e,
        const std::string& filepath,
        LogFiles::DownloadLogFileCallback cb);

    bool file_is_open();
    uint32_t current_chunk_size();
    uint32_t total_chunks();
    uint32_t bins_in_chunk();

    LogFiles::Entry entry{};

    std::string file_path{};
    std::ofstream file{};

    uint32_t current_chunk{};
    std::vector<bool> chunk_bin_table{};

    uint32_t chunk_bytes_written{};
    uint32_t total_bytes_written{};

    void* timeout_cookie{};

    LogFiles::DownloadLogFileCallback user_callback{};
};

class LogFilesImpl : public PluginImplBase {
public:
    explicit LogFilesImpl(System& system);
    explicit LogFilesImpl(std::shared_ptr<System> system);
    ~LogFilesImpl() override;

    void init() override;
    void deinit() override;

    void enable() override{};
    void disable() override{};

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

    void request_log_list(uint16_t index_min, uint16_t index_max);
    void request_log_data(unsigned id, unsigned start, unsigned count);

    void request_end();

    std::mutex _entries_mutex;
    std::unordered_map<uint16_t, LogFiles::Entry> _log_entries;
    uint32_t _total_entries;
    void* _entries_timeout_cookie{};
    LogFiles::GetEntriesCallback _entries_user_callback{};
    uint32_t _entries_request_attempts{};

    // The current log download data structure
    std::mutex _download_data_mutex;
    LogData _download_data;
};

} // namespace mavsdk
