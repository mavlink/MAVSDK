#include "global_include.h"
#include "log_files_impl.h"
#include "dronecode_sdk_impl.h"

namespace dronecode_sdk {

LogFilesImpl::LogFilesImpl(System &system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

LogFilesImpl::~LogFilesImpl()
{
    _parent->unregister_plugin(this);
}

void LogFilesImpl::init()
{
    using namespace std::placeholders; // for `_1`

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOG_ENTRY, std::bind(&LogFilesImpl::process_log_entry, this, _1), this);
}

void LogFilesImpl::deinit()
{
    _parent->unregister_all_mavlink_message_handlers(this);
}

void LogFilesImpl::enable() {}

void LogFilesImpl::disable() {}

std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> LogFilesImpl::get_entries()
{
    auto prom =
        std::make_shared<std::promise<std::pair<LogFiles::Result, std::vector<LogFiles::Entry>>>>();
    auto future_result = prom->get_future();

    get_entries_async([prom](LogFiles::Result result, std::vector<LogFiles::Entry> entries) {
        prom->set_value(std::make_pair<>(result, entries));
    });
    return future_result.get();
}

void LogFilesImpl::get_entries_async(LogFiles::get_entries_callback_t callback)
{
    {
        std::lock_guard<std::mutex> lock(_entries.mutex);
        _entries.entry_map.clear();
        _entries.callback = callback;
        _entries.max_list_id = 0;
        _entries.retries = 0;
    }

    _parent->register_timeout_handler(
        std::bind(&LogFilesImpl::request_list_timeout, this), 0.5, &_entries.cookie);

    request_list_entry(-1);
}

void LogFilesImpl::request_list_entry(int entry_id)
{
    // all
    uint16_t index_min = 0;
    uint16_t index_max = 0xFFFF;

    if (entry_id >= 0) {
        index_min = entry_id;
        index_max = entry_id;
    }

    mavlink_message_t msg;
    mavlink_msg_log_request_list_pack(GCSClient::system_id,
                                      GCSClient::component_id,
                                      &msg,
                                      _parent->get_system_id(),
                                      MAV_COMP_ID_AUTOPILOT1,
                                      index_min,
                                      index_max);

    _parent->send_message(msg);
}

void LogFilesImpl::process_log_entry(const mavlink_message_t &message)
{
    mavlink_log_entry_t log_entry;
    mavlink_msg_log_entry_decode(&message, &log_entry);

    LogFiles::Entry new_entry;
    new_entry.id = log_entry.id;
    // TODO: add date
    new_entry.bytes = log_entry.size;
    {
        std::lock_guard<std::mutex> lock(_entries.mutex);
        _entries.entry_map[new_entry.id] = new_entry;
        _entries.max_list_id = log_entry.num_logs;
        _parent->refresh_timeout_handler(_entries.cookie);
    }
}

void LogFilesImpl::request_list_timeout()
{
    std::lock_guard<std::mutex> lock(_entries.mutex);
    if (_entries.entry_map.size() == 0) {
        LogWarn() << "No entries received";
    } else if (_entries.entry_map.size() == _entries.max_list_id) {
        LogDebug() << "Received all entries";
        // Copy map entries into list to return;
        std::vector<LogFiles::Entry> entry_list{};
        for (unsigned i = 0; i < _entries.max_list_id; ++i) {
            entry_list.push_back(_entries.entry_map[i]);
        }
        if (_entries.callback) {
            _parent->call_user_callback(
                [this, entry_list]() { _entries.callback(LogFiles::Result::SUCCESS, entry_list); });
        }
    } else {
        if (_entries.retries > 20) {
            LogWarn() << "Too many log entry retries, giving up.";
            if (_entries.callback) {
                _parent->call_user_callback([this]() {
                    std::vector<LogFiles::Entry> empty_vector{};
                    _entries.callback(LogFiles::Result::TOO_MANY_RETRIES, empty_vector);
                });
            }
        } else {
            for (unsigned i = 0; i < _entries.max_list_id; ++i) {
                auto it = _entries.entry_map.find(i);
                if (it == _entries.entry_map.end()) {
                    LogDebug() << "Requesting log entry " << i << " again";
                    request_list_entry(int(i));
                }
            }
            _parent->register_timeout_handler(
                std::bind(&LogFilesImpl::request_list_timeout, this), 0.5, &_entries.cookie);
            _entries.retries++;
        }
    }
}

} // namespace dronecode_sdk
