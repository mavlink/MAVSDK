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

private:
    void process_log_entry(const mavlink_message_t &message);
    void request_list_timeout();

    void request_list_entry(int entry_id);

    struct {
        std::mutex mutex{};
        std::map<unsigned, LogFiles::Entry> entry_map{};
        LogFiles::get_entries_callback_t callback{nullptr};
        unsigned max_list_id{0};
        unsigned retries{0};
        void *cookie{nullptr};
    } _entries;
};

} // namespace dronecode_sdk
