#include "log_files_impl.h"
#include "mavlink_address.h"
#include "mavsdk_impl.h"

#include <algorithm>
#include <cmath>
#include <ctime>
#include <filesystem>

namespace mavsdk {

namespace fs = std::filesystem;

LogData::LogData(
    const LogFiles::Entry& e, const std::string& filepath, LogFiles::DownloadLogFileCallback cb) :
    entry(e),
    file_path(filepath),
    user_callback(cb)
{
    // Reset the table
    chunk_bin_table = std::vector<bool>(bins_in_chunk(), false);
    file.open(file_path, std::ios::out | std::ios::binary);
}

bool LogData::file_is_open()
{
    // TODO: any other checks?
    return !(file.rdstate() & std::ofstream::failbit); // Ensure file is writable
}

uint32_t LogData::total_chunks() const
{
    uint32_t partial_chunk = entry.size_bytes % CHUNK_SIZE;
    uint32_t add_one_chunk = partial_chunk ? 1 : 0;
    return entry.size_bytes / CHUNK_SIZE + add_one_chunk;
}

uint32_t LogData::bins_in_chunk() const
{
    uint32_t size = current_chunk_size();
    uint32_t add_one_bin = size % MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN ? 1 : 0;
    return size / MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN + add_one_bin;
}

uint32_t LogData::current_chunk_size() const
{
    uint32_t chunk_size = CHUNK_SIZE;
    uint32_t chunk_remainder = entry.size_bytes % CHUNK_SIZE;
    bool is_last_chunk = (current_chunk == total_chunks() - 1);
    // Last chunk might be less than CHUNK_SIZE
    if (is_last_chunk && chunk_remainder) {
        chunk_size = entry.size_bytes % CHUNK_SIZE;
    }

    return chunk_size;
}

LogFilesImpl::LogFilesImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

LogFilesImpl::LogFilesImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

LogFilesImpl::~LogFilesImpl()
{
    _system_impl->unregister_plugin(this);
}

void LogFilesImpl::init()
{
    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOG_ENTRY,
        [this](const mavlink_message_t& message) { process_log_entry(message); },
        this);

    _system_impl->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOG_DATA,
        [this](const mavlink_message_t& message) { process_log_data(message); },
        this);

    // Cancel any log file downloads that might still be going on.
    request_end();
}

void LogFilesImpl::deinit()
{
    {
        std::lock_guard<std::mutex> lock(_entries_mutex);
        _system_impl->unregister_timeout_handler(_entries_timeout_cookie);
    }

    {
        std::lock_guard<std::mutex> lock(_download_data_mutex);
        _system_impl->unregister_timeout_handler(_download_data.timeout_cookie);
    }
    _system_impl->unregister_all_mavlink_message_handlers(this);
}

std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> LogFilesImpl::get_entries()
{
    auto prom =
        std::make_shared<std::promise<std::pair<LogFiles::Result, std::vector<LogFiles::Entry>>>>();
    auto future_result = prom->get_future();

    get_entries_async([prom](LogFiles::Result result, std::vector<LogFiles::Entry> entries) {
        prom->set_value(std::make_pair<>(result, entries));
    });

    auto result = future_result.get();

    _entries_user_callback = nullptr;

    return result;
}

void LogFilesImpl::get_entries_async(LogFiles::GetEntriesCallback callback)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);
    _log_entries.clear();
    _entries_user_callback = callback;
    _total_entries = 0;

    _entries_timeout_cookie = _system_impl->register_timeout_handler(
        [this]() { entries_timeout(); }, _system_impl->timeout_s() * 10.0);

    request_log_list(0, 0xFFFF);
}

void LogFilesImpl::request_log_list(uint16_t index_min, uint16_t index_max)
{
    _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_log_request_list_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            MAV_COMP_ID_AUTOPILOT1,
            index_min,
            index_max);
        return message;
    });
}

void LogFilesImpl::process_log_entry(const mavlink_message_t& message)
{
    mavlink_log_entry_t msg;
    mavlink_msg_log_entry_decode(&message, &msg);

    std::lock_guard<std::mutex> lock(_entries_mutex);

    _system_impl->refresh_timeout_handler(_entries_timeout_cookie);

    // Bad data handling
    if (msg.num_logs == 0 || msg.id >= msg.num_logs) {
        LogWarn() << "No logs available";

        _system_impl->unregister_timeout_handler(_entries_timeout_cookie);

        const auto cb = _entries_user_callback;
        if (cb) {
            _system_impl->call_user_callback(
                [cb]() { cb(LogFiles::Result::NoLogfiles, std::vector<LogFiles::Entry>()); });
        }
        return;
    }

    LogFiles::Entry new_entry;
    new_entry.id = msg.id;

    // Convert milliseconds to ISO 8601 date string in UTC.
    char buf[sizeof "2018-08-31T20:50:42Z"];
    const time_t time_utc = msg.time_utc;
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&time_utc));

    new_entry.date = buf;
    new_entry.size_bytes = msg.size;

    _log_entries[new_entry.id] = new_entry;
    _total_entries = msg.num_logs;

    // Check if all entries are received
    if (_log_entries.size() == _total_entries) {
        _system_impl->unregister_timeout_handler(_entries_timeout_cookie);

        // Copy map entries into list to return
        std::vector<LogFiles::Entry> entry_list{};
        for (unsigned i = 0; i < _log_entries.size(); i++) {
            entry_list.push_back(_log_entries[i]);
        }

        const auto cb = _entries_user_callback;
        if (cb) {
            _system_impl->call_user_callback(
                [cb, entry_list]() { cb(LogFiles::Result::Success, entry_list); });
        }
    }
}

void LogFilesImpl::entries_timeout()
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    const auto cb = _entries_user_callback;
    if (cb) {
        _system_impl->call_user_callback([cb]() {
            LogDebug() << "Request entries timeout!";
            cb(LogFiles::Result::Timeout, std::vector<LogFiles::Entry>());
        });
    }
}

std::pair<LogFiles::Result, LogFiles::ProgressData>
LogFilesImpl::download_log_file(LogFiles::Entry entry, const std::string& file_path)
{
    auto prom =
        std::make_shared<std::promise<std::pair<LogFiles::Result, LogFiles::ProgressData>>>();
    auto future_result = prom->get_future();

    download_log_file_async(
        entry, file_path, [prom](LogFiles::Result result, LogFiles::ProgressData progress) {
            if (result != LogFiles::Result::Next) {
                prom->set_value(std::make_pair(result, progress));
            }
        });

    return future_result.get();
}

void LogFilesImpl::download_log_file_async(
    LogFiles::Entry entry, const std::string& file_path, LogFiles::DownloadLogFileCallback callback)
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    auto entry_it = _log_entries.find(entry.id);
    bool error = entry_it == _log_entries.end() || fs::is_directory(fs::path(file_path)) ||
                 fs::exists(file_path);

    if (error) {
        LogErr() << "error: download_log_file_async failed";

        if (callback) {
            _system_impl->call_user_callback([callback]() {
                callback(LogFiles::Result::InvalidArgument, LogFiles::ProgressData());
            });
        }
        return;
    }

    _download_data = LogData(entry_it->second, file_path, callback);

    if (!_download_data.file_is_open()) {
        if (callback) {
            _system_impl->call_user_callback([callback]() {
                callback(LogFiles::Result::FileOpenFailed, LogFiles::ProgressData());
            });
        }
        return;
    }

    _download_data.timeout_cookie = _system_impl->register_timeout_handler(
        [this]() { LogFilesImpl::data_timeout(); }, _system_impl->timeout_s() * 1.0);

    // Request the first chunk
    request_log_data(_download_data.entry.id, 0, _download_data.current_chunk_size());
}

void LogFilesImpl::process_log_data(const mavlink_message_t& message)
{
    mavlink_log_data_t msg;
    mavlink_msg_log_data_decode(&message, &msg);

    std::lock_guard<std::mutex> lock(_download_data_mutex);

    _system_impl->refresh_timeout_handler(_download_data.timeout_cookie);

    if (msg.count > MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN) {
        LogErr() << "Ignoring wrong count" << msg.count;
        return;
    }

    if (msg.id != _download_data.entry.id) {
        LogErr() << "Ignoring wrong ID: actual/expected: " << msg.id << "/"
                 << _download_data.entry.id;
        return;
    }

    if (msg.ofs > _download_data.entry.size_bytes) {
        LogErr() << "Offset greater than file size: offset/size: " << msg.ofs << "/"
                 << _download_data.entry.size_bytes;
        return;
    }

    if (msg.ofs % MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN) {
        LogErr() << "Ignoring misaligned offset: " << msg.ofs;
        return;
    }

    // Calculate current chunk
    const uint32_t chunk = msg.ofs / CHUNK_SIZE;

    if (chunk != _download_data.current_chunk) {
        // Quietly ignore packets for out of order chunks
        // LogErr() << "Ignored packet for out of order chunk actual/expected: " << chunk << "/"
        //          << _download_data.current_chunk;
        return;
    }

    // Calculate current bin within the chunk
    const uint16_t bin = (msg.ofs - chunk * CHUNK_SIZE) / MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN;

    if (bin >= _download_data.chunk_bin_table.size()) {
        LogErr() << "Out of range bin received: bin/size: " << bin << "/"
                 << _download_data.chunk_bin_table.size();
        return;
    }

    if (_download_data.file.tellp() != msg.ofs) {
        if (!_download_data.file.seekp(msg.ofs)) {
            LogErr() << "Error while seeking to log file offset";
            return;
        }
    }

    _download_data.file.write((char*)msg.data, msg.count);

    // Quietly ignore duplicate packets -- we don't want to record the bytes_written twice
    if (!_download_data.chunk_bin_table[bin]) {
        _download_data.chunk_bytes_written += msg.count;
        _download_data.chunk_bin_table[bin] = true;
    }

    bool chunk_complete = _download_data.chunk_bytes_written == _download_data.current_chunk_size();

    if (chunk_complete) {
        auto result = LogFiles::Result::Next;

        _download_data.current_chunk++;
        _download_data.total_bytes_written += _download_data.chunk_bytes_written;
        _download_data.chunk_bytes_written = 0;
        _download_data.chunk_bin_table = std::vector<bool>(_download_data.bins_in_chunk(), false);

        bool log_complete = _download_data.total_bytes_written == _download_data.entry.size_bytes;

        if (log_complete) {
            result = LogFiles::Result::Success;
            _download_data.file.close();
            _system_impl->unregister_timeout_handler(_download_data.timeout_cookie);

        } else {
            // Request the next chunk
            request_log_data(
                _download_data.entry.id,
                _download_data.current_chunk * CHUNK_SIZE,
                _download_data.current_chunk_size());
        }

        LogFiles::ProgressData progress_data;
        progress_data.progress =
            (float)_download_data.total_bytes_written / (float)_download_data.entry.size_bytes;

        // Update progress
        const auto cb = _download_data.user_callback;
        if (cb) {
            _system_impl->call_user_callback(
                [cb, progress_data, result]() { cb(result, progress_data); });
        }
    }
}

void LogFilesImpl::data_timeout()
{
    std::lock_guard<std::mutex> lock(_download_data_mutex);

    LogErr() << "Timeout!";
    LogErr() << "Requesting missing chunk:\t" << _download_data.current_chunk << "/"
             << _download_data.total_chunks();

    // Reset chunk data
    _download_data.chunk_bytes_written = 0;
    _download_data.chunk_bin_table = std::vector<bool>(_download_data.bins_in_chunk(), false);

    request_log_data(
        _download_data.entry.id,
        _download_data.current_chunk * CHUNK_SIZE,
        _download_data.current_chunk_size());

    _download_data.timeout_cookie = _system_impl->register_timeout_handler(
        [this]() { LogFilesImpl::data_timeout(); }, _system_impl->timeout_s() * 1.0);
}

void LogFilesImpl::request_log_data(unsigned id, unsigned start, unsigned count)
{
    _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_log_request_data_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            MAV_COMP_ID_AUTOPILOT1,
            id,
            start,
            count);
        return message;
    });
}

LogFiles::Result LogFilesImpl::erase_all_log_files()
{
    _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_log_erase_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            MAV_COMP_ID_AUTOPILOT1);
        return message;
    });

    // TODO: find a good way to know about the success or failure of the operation
    return LogFiles::Result::Success;
}

void LogFilesImpl::request_end()
{
    _system_impl->queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_log_request_end_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _system_impl->get_system_id(),
            MAV_COMP_ID_AUTOPILOT1);
        return message;
    });
}

} // namespace mavsdk
