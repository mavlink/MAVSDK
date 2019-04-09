#pragma once

#include "plugin_impl_base.h"
#include "mavlink_include.h"
#include "system.h"
#include "plugins/log_files/log_files.h"

namespace dronecode_sdk {

class LogFilesImpl : public PluginImplBase {
public:
    LogFilesImpl(System &system);
    ~LogFilesImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> get_entries();
    void get_entries_async(LogFiles::get_entries_callback_t callback);

    LogFiles::Result download_log_file(unsigned id, const std::string &file_path);
    void download_log_file_async(unsigned id,
                                 const std::string &file_path,
                                 LogFiles::download_log_file_callback_t callback);

private:
    void request_end();

    void process_log_entry(const mavlink_message_t &message);
    void process_log_data(const mavlink_message_t &message);
    void list_timeout();

    void request_list_entry(int entry_id);

    void check_missing_log_data();
    void request_log_data(unsigned id, unsigned offset, unsigned bytes_to_get);
    void data_timeout();
    void write_log_data_to_disk();

    Time _time{};

    struct {
        std::mutex mutex{};
        std::map<unsigned, LogFiles::Entry> entry_map{};
        LogFiles::get_entries_callback_t callback{nullptr};
        unsigned max_list_id{0};
        unsigned retries{0};
        void *cookie{nullptr};
    } _entries{};

    struct {
        unsigned id{0};
        std::mutex mutex{};
        std::vector<uint8_t> bytes{};
        std::vector<bool> chunks_received{};
        unsigned retries{0};
        bool rerequesting{false};
        void *cookie{nullptr};
        std::string file_path{};
        LogFiles::download_log_file_callback_t callback{nullptr};
        unsigned last_progress_percentage{0};
        unsigned chunks_to_rerequest_initially{0};
        unsigned bytes_received{};
        dl_time_t time_started{};
    } _data{};

    static constexpr unsigned CHUNK_SIZE = 90;
};

} // namespace dronecode_sdk
