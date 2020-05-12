#include "global_include.h"
#include "log_files_impl.h"
#include "mavsdk_impl.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstring>

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
    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOG_ENTRY,
        [this](const mavlink_message_t& message) { process_log_entry(message); },
        this);

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOG_DATA,
        [this](const mavlink_message_t& message) { process_log_data(message); },
        this);

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

void LogFilesImpl::get_entries_async(LogFiles::GetEntriesCallback callback)
{
    {
        std::lock_guard<std::mutex> lock(_entries.mutex);
        _entries.entry_map.clear();
        _entries.callback = callback;
        _entries.max_list_id = 0;
        _entries.retries = 0;
    }

    _parent->register_timeout_handler(
        [this]() { list_timeout(); }, LIST_TIMEOUT_S, &_entries.cookie);

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
                tmp_callback(LogFiles::Result::NoLogfiles, empty_list);
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
                tmp_callback(LogFiles::Result::Success, entry_list);
            });
        }
    } else {
        if (_entries.retries > 3) {
            LogWarn() << "Too many log entry retries, giving up.";
            if (_entries.callback) {
                const auto tmp_callback = _entries.callback;
                _parent->call_user_callback([tmp_callback]() {
                    std::vector<LogFiles::Entry> empty_vector{};
                    tmp_callback(LogFiles::Result::Timeout, empty_vector);
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
                [this]() { list_timeout(); }, LIST_TIMEOUT_S, &_entries.cookie);
            _entries.retries++;
        }
    }
}

void LogFilesImpl::download_log_file_async(
    unsigned id, const std::string& file_path, LogFiles::DownloadLogFileCallback callback)
{
    unsigned bytes_to_get;
    {
        std::lock_guard<std::mutex> lock(_entries.mutex);

        auto it = _entries.entry_map.find(id);
        if (it == _entries.entry_map.end()) {
            LogErr() << "Log entry id " << id << " not found";
            if (callback) {
                const auto tmp_callback = callback;
                _parent->call_user_callback([tmp_callback]() {
                    LogFiles::ProgressData progress;
                    progress.progress = 0.0f;
                    tmp_callback(LogFiles::Result::InvalidArgument, progress);
                });
            }
            return;
        }

        bytes_to_get = _entries.entry_map[id].size_bytes;
    }

    {
        std::lock_guard<std::mutex> lock(_data.mutex);

        if (!start_logfile(file_path)) {
            if (callback) {
                const auto tmp_callback = callback;
                _parent->call_user_callback([tmp_callback]() {
                    LogFiles::ProgressData progress;
                    progress.progress = NAN;
                    tmp_callback(LogFiles::Result::FileOpenFailed, progress);
                });
            }
            return;
        }

        _data.id = id;
        _data.callback = callback;
        _data.time_started = _time.steady_time();
        _data.bytes_to_get = bytes_to_get;
        _data.part_start = 0;
        const auto part_size = determine_part_end() - _data.part_start;
        _data.bytes.resize(part_size);
        _data.chunks_received.resize(part_size / MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN);

        _parent->register_timeout_handler(
            std::bind(&LogFilesImpl::data_timeout, this), DATA_TIMEOUT_S, &_data.cookie);

        request_log_data(_data.id, _data.part_start, _data.bytes.size());

        if (_data.callback) {
            const auto tmp_callback = _data.callback;
            _parent->call_user_callback([tmp_callback]() {
                LogFiles::ProgressData progress;
                progress.progress = 0.0f;
                tmp_callback(LogFiles::Result::Next, progress);
            });
        }
    }
}

std::size_t LogFilesImpl::determine_part_end()
{
    // Assumes to have the lock for _data.mutex.

    return std::min(
        _data.part_start + PART_SIZE * MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN,
        std::size_t(_data.bytes_to_get));
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

    std::lock_guard<std::mutex> lock(_data.mutex);

    _parent->refresh_timeout_handler(_data.cookie);

    if (log_data.count > MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN) {
        LogErr() << "Ignoring wrong count";
        return;
    }

    if (log_data.ofs < _data.part_start ||
        log_data.ofs + log_data.count > _data.part_start + _data.bytes.size()) {
        LogErr() << "Ignoring wrong offset";
        return;
    }

    std::memcpy(&_data.bytes[log_data.ofs - _data.part_start], log_data.data, log_data.count);
    _data.chunks_received[(log_data.ofs - _data.part_start) / MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN] =
        true;

    if (log_data.ofs + log_data.count - _data.part_start == _data.bytes.size() ||
        _data.rerequesting) {
        // We received last message of this part.
        _data.rerequesting = true;
        check_part();
    }
}

void LogFilesImpl::report_progress(unsigned transferred, unsigned total)
{
    float progress = float(transferred) / float(total);

    if (_data.callback) {
        const auto tmp_callback = _data.callback;
        _parent->call_user_callback([tmp_callback, progress]() {
            LogFiles::ProgressData progress_data;
            progress_data.progress = progress;

            tmp_callback(LogFiles::Result::Next, progress_data);
        });
    }
}

void LogFilesImpl::check_part()
{
    // Assumes to have the lock for _data.mutex.

    auto first_missing =
        std::find(_data.chunks_received.begin(), _data.chunks_received.end(), false);
    if (first_missing != _data.chunks_received.end()) {
        auto first_missing_index = std::distance(_data.chunks_received.begin(), first_missing);

        auto first_not_missing = std::find(first_missing, _data.chunks_received.end(), true);

        auto first_not_missing_index =
            std::distance(_data.chunks_received.begin(), first_not_missing);

        request_log_data(
            _data.id,
            _data.part_start + first_missing_index * MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN,
            (first_not_missing_index - first_missing_index) * MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN);
    } else {
        _data.rerequesting = false;

        write_part_to_disk();

        report_progress(_data.part_start + _data.bytes.size(), _data.bytes_to_get);

        const float kib_s = float(_data.part_start + _data.bytes.size()) /
                            float(_time.elapsed_since_s(_data.time_started)) / 1024.0f;

        LogDebug() << _data.part_start + _data.bytes.size() << " B of " << _data.bytes_to_get
                   << " B (" << kib_s << " kiB/s)";

        if (_data.part_start + _data.bytes.size() == _data.bytes_to_get) {
            _parent->unregister_timeout_handler(_data.cookie);

            finish_logfile();

            if (_data.callback) {
                const auto tmp_callback = _data.callback;
                _parent->call_user_callback([tmp_callback]() {
                    LogFiles::ProgressData progress_data;
                    progress_data.progress = 1.0f;
                    tmp_callback(LogFiles::Result::Success, progress_data);
                });
            }

            reset_data();
        } else {
            _data.part_start = _data.part_start + _data.bytes.size();

            const auto part_size = determine_part_end() - _data.part_start;
            _data.bytes.resize(part_size);
            _data.chunks_received.resize(part_size / MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN);
            std::fill(_data.chunks_received.begin(), _data.chunks_received.end(), false);

            request_log_data(_data.id, _data.part_start, _data.bytes.size());
        }
    }
}

void LogFilesImpl::request_log_data(unsigned id, unsigned start, unsigned count)
{
    // LogDebug() << "requesting: " << start << ".." << start+count << " (" << id << ")";
    mavlink_message_t msg;
    mavlink_msg_log_request_data_pack(
        _parent->get_own_system_id(),
        _parent->get_own_component_id(),
        &msg,
        _parent->get_system_id(),
        MAV_COMP_ID_AUTOPILOT1,
        id,
        start,
        count);
    _parent->send_message(msg);
}

void LogFilesImpl::data_timeout()
{
    {
        std::lock_guard<std::mutex> lock(_data.mutex);
        _parent->register_timeout_handler(
            std::bind(&LogFilesImpl::data_timeout, this), DATA_TIMEOUT_S, &_data.cookie);
        _data.rerequesting = true;
        check_part();
    }
}

bool LogFilesImpl::start_logfile(const std::string& path)
{
    // Assumes to have the lock for _data.mutex.

    _data.file.open(path, std::ios::out | std::ios::binary);

    return ((_data.file.rdstate() & std::ofstream::failbit) == 0);
}

void LogFilesImpl::write_part_to_disk()
{
    // Assumes to have the lock for _data.mutex.

    _data.file.write(reinterpret_cast<char*>(_data.bytes.data()), _data.bytes.size());
}

void LogFilesImpl::finish_logfile()
{
    // Assumes to have the lock for _data.mutex.

    _data.file.close();
}

void LogFilesImpl::reset_data()
{
    // Assumes to have the lock for _data.mutex.
    _data.id = 0;
    _data.bytes_to_get = 0;
    _data.bytes.clear();
    _data.chunks_received.clear();
    _data.part_start = 0;
    _data.retries = 0;
    _data.rerequesting = false;
    _data.last_ofs_rerequested = -1;
    _data.callback = nullptr;
}

} // namespace mavsdk
