#include "global_include.h"
#include "log_files_impl.h"
#include "mavsdk_impl.h"
#include <algorithm>
#include <fstream>
#include <ctime>
#include <cstring>

// TODO: download in chunks of 512*90 bytes like QGC to avoid UDP buffers from overflowing.

namespace mavsdk {

LogFilesImpl::LogFilesImpl(System& system) : PluginImplBase(system)
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

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOG_DATA, std::bind(&LogFilesImpl::process_log_data, this, _1), this);

    // Cancel any log file downloads that might still be going on.
    request_end();
}

void LogFilesImpl::deinit()
{
    {
        std::lock_guard<std::mutex> lock(_entries.mutex);
        _parent->unregister_timeout_handler(_entries.cookie);
    }

    {
        std::lock_guard<std::mutex> lock(_data.mutex);
        _parent->unregister_timeout_handler(_data.cookie);
    }
    _parent->unregister_all_mavlink_message_handlers(this);
}

void LogFilesImpl::enable() {}

void LogFilesImpl::disable() {}

void LogFilesImpl::request_end()
{
    mavlink_message_t msg;
    mavlink_msg_log_request_end_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        _parent->get_system_id(),
        MAV_COMP_ID_AUTOPILOT1);
    _parent->send_message(msg);
}

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
        std::bind(&LogFilesImpl::list_timeout, this), LIST_TIMEOUT_S, &_entries.cookie);

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
    mavlink_msg_log_request_list_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        _parent->get_system_id(),
        MAV_COMP_ID_AUTOPILOT1,
        index_min,
        index_max);

    _parent->send_message(msg);
}

void LogFilesImpl::process_log_entry(const mavlink_message_t& message)
{
    mavlink_log_entry_t log_entry;
    mavlink_msg_log_entry_decode(&message, &log_entry);

    // Catch case where there are no log files to be found.
    if (log_entry.num_logs == 0 && log_entry.id == 0) {
        std::lock_guard<std::mutex> lock(_entries.mutex);
        _parent->unregister_timeout_handler(_entries.cookie);
        if (_entries.callback) {
            const auto tmp_callback = _entries.callback;
            std::vector<LogFiles::Entry> empty_list{};
            _parent->call_user_callback([tmp_callback, empty_list]() {
                tmp_callback(LogFiles::Result::NO_LOGFILES, empty_list);
            });
        }
        return;
    }

    LogFiles::Entry new_entry;
    new_entry.id = log_entry.id;

    // Convert milliseconds to ISO 8601 date string in UTC.
    char buf[sizeof "2018-08-31T20:50:42Z"];
    const time_t time_utc = log_entry.time_utc;
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&time_utc));

    new_entry.date = buf;
    new_entry.size_bytes = log_entry.size;
    {
        std::lock_guard<std::mutex> lock(_entries.mutex);
        _entries.entry_map[new_entry.id] = new_entry;
        _entries.max_list_id = log_entry.num_logs;
        _parent->refresh_timeout_handler(_entries.cookie);
    }
}

void LogFilesImpl::list_timeout()
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
            const auto tmp_callback = _entries.callback;
            _parent->call_user_callback([tmp_callback, entry_list]() {
                tmp_callback(LogFiles::Result::SUCCESS, entry_list);
            });
        }
    } else {
        if (_entries.retries > 3) {
            LogWarn() << "Too many log entry retries, giving up.";
            if (_entries.callback) {
                const auto tmp_callback = _entries.callback;
                _parent->call_user_callback([tmp_callback]() {
                    std::vector<LogFiles::Entry> empty_vector{};
                    tmp_callback(LogFiles::Result::TIMEOUT, empty_vector);
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
                std::bind(&LogFilesImpl::list_timeout, this), LIST_TIMEOUT_S, &_entries.cookie);
            _entries.retries++;
        }
    }
}

LogFiles::Result LogFilesImpl::download_log_file(unsigned id, const std::string& file_path)
{
    auto prom = std::make_shared<std::promise<LogFiles::Result>>();
    auto future_result = prom->get_future();

    download_log_file_async(id, file_path, [prom](LogFiles::Result result, float progress) {
        if (result == LogFiles::Result::PROGRESS) {
            LogInfo() << "Download progress: " << 100.0f * progress;
        } else {
            prom->set_value(result);
        }
    });
    return future_result.get();
}

void LogFilesImpl::download_log_file_async(
    unsigned id, const std::string& file_path, LogFiles::download_log_file_callback_t callback)
{
    unsigned bytes_to_get;
    {
        std::lock_guard<std::mutex> lock(_entries.mutex);

        auto it = _entries.entry_map.find(id);
        if (it == _entries.entry_map.end()) {
            LogErr() << "Log entry id " << id << " not found";
            return;
        }

        bytes_to_get = _entries.entry_map[id].size_bytes;
    }

    {
        std::lock_guard<std::mutex> lock(_data.mutex);

        _data.id = id;
        _data.bytes.resize(bytes_to_get);
        _data.file_path = file_path;
        _data.callback = callback;
        _data.time_started = _time.steady_time();

        unsigned num_chunks = bytes_to_get / CHUNK_SIZE;
        if (bytes_to_get % CHUNK_SIZE) {
            ++num_chunks;
        }
        _data.chunks_received.resize(num_chunks);

        _parent->register_timeout_handler(
            std::bind(&LogFilesImpl::data_timeout, this), 0.5, &_data.cookie);

        if (_data.callback) {
            const auto tmp_callback = _data.callback;
            _parent->call_user_callback(
                [tmp_callback]() { tmp_callback(LogFiles::Result::PROGRESS, 0.0f); });
        }
    }

    request_log_data(id, 0, bytes_to_get);
}

void LogFilesImpl::process_log_data(const mavlink_message_t& message)
{
    mavlink_log_data_t log_data;
    mavlink_msg_log_data_decode(&message, &log_data);

#if 0
    // To test retransmission
    static unsigned counter = 0;
    if (counter < 10 && (log_data.ofs == 1800 || log_data.ofs == 3600)) {
        ++counter;
        return;
    }
#endif


    {
        std::lock_guard<std::mutex> lock(_data.mutex);

        _parent->refresh_timeout_handler(_data.cookie);

        if (log_data.count > CHUNK_SIZE) {
            LogErr() << "Ignoring wrong count";
            return;
        }

        if (log_data.ofs / CHUNK_SIZE >= _data.chunks_received.size()) {
            LogErr() << "Ignoring wrong offset";
            return;
        }

        std::memcpy(&_data.bytes[log_data.ofs], log_data.data, log_data.count);
        _data.chunks_received[log_data.ofs / CHUNK_SIZE] = true;
        _data.bytes_received += log_data.count;

        auto new_progress = get_progress();

        // Only report every 1%
        if (new_progress != _data.last_progress_percentage) {
            _data.last_progress_percentage = new_progress;
            if (_data.callback) {
                const auto tmp_callback = _data.callback;
                _parent->call_user_callback([tmp_callback, new_progress]() {
                    tmp_callback(LogFiles::Result::PROGRESS, new_progress / 100.0f);
                });
            }

            const float kib_s = float(_data.bytes_received) /
                                float(_time.elapsed_since_s(_data.time_started)) / 1024.0f;

            LogDebug() << _data.bytes_received << " B of " << _data.bytes.size() << " B (" << kib_s
                       << " kiB/s)";
        }
    }

    check_missing_log_data();
}

void LogFilesImpl::check_missing_log_data()
{
    std::lock_guard<std::mutex> lock(_data.mutex);

    if (!_data.rerequesting) {
        return;
    }

    unsigned num_missing = 0;
    unsigned id_to_get = 0;
    unsigned ofs_to_get = 0;
    unsigned bytes_to_get = 0;

    for (unsigned i = 0; i < _data.chunks_received.size(); ++i) {
        if (!_data.chunks_received[i]) {
            if (num_missing == 0) {
                bytes_to_get = CHUNK_SIZE;
                if (i + 1 == _data.chunks_received.size()) {
                    bytes_to_get = static_cast<unsigned>(
                        _data.bytes.size() - (_data.chunks_received.size() * CHUNK_SIZE - 1));
                }

                id_to_get = _data.id;
                ofs_to_get = i * CHUNK_SIZE;
            }
            ++num_missing;
        }
    }

    if (num_missing > 0) {
        LogDebug() << "Re-requesting log data " << ofs_to_get;
        if (_data.last_ofs_rerequested != int(ofs_to_get)) {
            _data.last_ofs_rerequested = ofs_to_get;
            _data.retries = 0;
        } else {
            ++_data.retries;
        }

        if (_data.retries > 3) {
            LogWarn() << "Too many log data retries, giving up.";
            if (_data.callback) {
                const auto tmp_callback = _data.callback;
                _parent->call_user_callback(
                    [tmp_callback]() { tmp_callback(LogFiles::Result::TIMEOUT, 0.0f); });
            }
            _parent->unregister_timeout_handler(_data.cookie);
            reset_data();
            return;
        }

        request_log_data(id_to_get, ofs_to_get, bytes_to_get);
        return;
    }

    _parent->unregister_timeout_handler(_data.cookie);

    write_log_data_to_disk();
    if (_data.callback) {
        const auto tmp_callback = _data.callback;
        _parent->call_user_callback(
            [tmp_callback]() { tmp_callback(LogFiles::Result::SUCCESS, 1.0f); });
    }
    reset_data();
}

void LogFilesImpl::request_log_data(unsigned id, unsigned chunk_id, unsigned bytes_to_get)
{
    mavlink_message_t msg;
    mavlink_msg_log_request_data_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        _parent->get_system_id(),
        MAV_COMP_ID_AUTOPILOT1,
        id,
        chunk_id,
        bytes_to_get);
    _parent->send_message(msg);
}

void LogFilesImpl::data_timeout()
{
    LogDebug() << "data timeout!";
    {
        std::lock_guard<std::mutex> lock(_data.mutex);
        _parent->register_timeout_handler(
            std::bind(&LogFilesImpl::data_timeout, this), DATA_TIMEOUT_S, &_data.cookie);
        _data.rerequesting = true;
    }
    check_missing_log_data();
}

void LogFilesImpl::write_log_data_to_disk()
{
    // Assumes to have the _data.mutex locked

    std::ofstream out_file;
    out_file.open(_data.file_path, std::ios::out | std::ios::binary);
    out_file.write(reinterpret_cast<char*>(_data.bytes.data()), _data.bytes.size());
    out_file.close();
}

void LogFilesImpl::reset_data()
{
    // Assumes to have a lock on _data.
    _data.id = 0;
    _data.bytes.clear();
    _data.chunks_received.clear();
    _data.retries = 0;
    _data.rerequesting = false;
    _data.last_ofs_rerequested = -1;
    _data.last_progress_percentage = -1;
    _data.bytes_received = 0;
    _data.file_path.clear();
    _data.callback = nullptr;
}

int LogFilesImpl::get_progress()
{
    // Assumes to have the _data.mutex locked
    const unsigned received =
        std::count(_data.chunks_received.begin(), _data.chunks_received.end(), true);

    const unsigned total = _data.chunks_received.size();

    return int((float(received) / float(total)) * 100.0f);
}

} // namespace mavsdk
