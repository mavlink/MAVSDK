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
    _system_impl->unregister_all_mavlink_message_handlers_blocking(this);
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
    std::lock_guard<std::mutex> lock(_entries_mutex);
    _log_entries.clear();
    _entries_user_callback = callback;
    _total_entries = 0;
    _entries_retry_count = 0;
    _min_entry_id = 0xFFFF; // Reset to detect indexing scheme

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
    // PX4 uses 0-based indexing: valid IDs are 0 to num_logs-1
    // ArduPilot uses 1-based indexing: valid IDs are 1 to num_logs
    // We detect the scheme by tracking the minimum ID seen
    if (msg.num_logs == 0 || msg.id > msg.num_logs) {
        LogWarn() << "No logs available";

        _system_impl->unregister_timeout_handler(_entries_timeout_cookie);

        // Clear callback before invoking to prevent double-invocation
        const auto cb = _entries_user_callback;
        _entries_user_callback = nullptr;
        if (cb) {
            _system_impl->call_user_callback(
                [cb]() { cb(LogFiles::Result::NoLogfiles, std::vector<LogFiles::Entry>()); });
        }
        return;
    }

    // Track minimum ID to determine indexing scheme (0-based vs 1-based)
    if (msg.id < _min_entry_id) {
        _min_entry_id = msg.id;
    }

    // Initialize vector if this is the first entry
    if (_total_entries != msg.num_logs) {
        _total_entries = msg.num_logs;
        _log_entries.clear();
        _log_entries.resize(_total_entries);
    }

    LogFiles::Entry new_entry;
    new_entry.id = msg.id;

    // Convert milliseconds to ISO 8601 date string in UTC.
    char buf[sizeof "2018-08-31T20:50:42Z"];
    const time_t time_utc = msg.time_utc;
    strftime(buf, sizeof buf, "%FT%TZ", gmtime(&time_utc));

    new_entry.date = buf;
    new_entry.size_bytes = msg.size;

    // Store entry using 0-based index (subtract min_entry_id to handle both schemes)
    const uint16_t storage_index = msg.id - _min_entry_id;
    if (storage_index >= _log_entries.size()) {
        LogWarn() << "Log entry ID out of range: " << msg.id;
        return;
    }
    _log_entries[storage_index] = new_entry;

    // Check if all entries are received
    bool all_received =
        std::all_of(_log_entries.begin(), _log_entries.end(), [](const auto& entry_opt) {
            return entry_opt.has_value();
        });

    if (all_received) {
        _system_impl->unregister_timeout_handler(_entries_timeout_cookie);

        // Build result list from vector (safe since all entries are present)
        std::vector<LogFiles::Entry> entry_list;
        entry_list.reserve(_log_entries.size());
        std::transform(
            _log_entries.begin(),
            _log_entries.end(),
            std::back_inserter(entry_list),
            [](const auto& entry_opt) { return entry_opt.value(); });

        // Clear callback before invoking to prevent double-invocation if another
        // LOG_ENTRY message arrives before the async callback runs
        const auto cb = _entries_user_callback;
        _entries_user_callback = nullptr;
        if (cb) {
            _system_impl->call_user_callback(
                [cb, entry_list]() { cb(LogFiles::Result::Success, entry_list); });
        }
    } else {
        // Check for missing entries when we might have all entries
        // This handles: receiving the last expected entry AND receiving retried entries that might
        // complete the set
        if (_total_entries > 0) {
            // Expected last ID depends on indexing scheme: min_entry_id + num_logs - 1
            const uint32_t expected_last_entry_id = _min_entry_id + _total_entries - 1;
            const uint32_t last_storage_index = _total_entries - 1;
            // Check if this could be the last entry we need (either the expected last one, or a
            // retried one)
            if (msg.id == expected_last_entry_id || _log_entries[last_storage_index].has_value()) {
                check_and_request_missing_entries();
            }
        }
    }
}

void LogFilesImpl::entries_timeout()
{
    std::lock_guard<std::mutex> lock(_entries_mutex);

    LogDebug() << "Request entries timeout! Retry count: " << _entries_retry_count;

    constexpr uint32_t MAX_RETRIES = 5;

    if (_entries_retry_count < MAX_RETRIES) {
        // Try to request missing entries
        check_and_request_missing_entries();
        _entries_retry_count++;

        // Reset timeout for another attempt (use normal timeout, not * 10.0)
        _entries_timeout_cookie = _system_impl->register_timeout_handler(
            [this]() { entries_timeout(); }, _system_impl->timeout_s());

        // Don't call user callback yet - give it another chance
        return;
    }

    // Max retries exceeded - give up and call user with timeout error
    // Clear callback before invoking to prevent double-invocation
    const auto cb = _entries_user_callback;
    _entries_user_callback = nullptr;
    if (cb) {
        _system_impl->call_user_callback(
            [cb]() { cb(LogFiles::Result::Timeout, std::vector<LogFiles::Entry>()); });
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

    // Convert entry.id to storage index using detected min_entry_id
    // Handles both 0-based (PX4) and 1-based (ArduPilot) indexing
    const bool id_in_range =
        entry.id >= _min_entry_id && (entry.id - _min_entry_id) < _log_entries.size();
    const uint16_t storage_index = entry.id - _min_entry_id;

    bool error = !id_in_range || !_log_entries[storage_index].has_value() ||
                 fs::is_directory(fs::path(file_path)) || fs::exists(file_path);

    if (error) {
        LogErr() << "error: download_log_file_async failed";

        if (callback) {
            _system_impl->call_user_callback([callback]() {
                callback(LogFiles::Result::InvalidArgument, LogFiles::ProgressData());
            });
        }
        return;
    }

    const auto& found_entry = _log_entries[storage_index].value();

    // Check for zero-sized file and abort early
    if (found_entry.size_bytes == 0) {
        LogErr() << "Cannot download zero-sized log file";

        if (callback) {
            _system_impl->call_user_callback([callback]() {
                callback(LogFiles::Result::InvalidArgument, LogFiles::ProgressData());
            });
        }
        return;
    }

    _download_data = LogData(found_entry, file_path, callback);

    if (!_download_data.file_is_open()) {
        if (callback) {
            _system_impl->call_user_callback([callback]() {
                callback(LogFiles::Result::FileOpenFailed, LogFiles::ProgressData());
            });
        }
        return;
    }

    _download_data.timeout_cookie = _system_impl->register_timeout_handler(
        [this]() { LogFilesImpl::data_timeout(); }, _system_impl->timeout_s());

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

    // Mark bin as received (quietly ignore duplicates)
    _download_data.chunk_bin_table[bin] = true;

    // Check if all bins in the chunk have been received
    const uint32_t bins_in_chunk = _download_data.bins_in_chunk();
    bool chunk_complete = std::all_of(
        _download_data.chunk_bin_table.begin(),
        _download_data.chunk_bin_table.begin() + bins_in_chunk,
        [](bool received) { return received; });

    if (chunk_complete) {
        uint32_t chunk_bytes = _download_data.current_chunk_size();

        auto result = LogFiles::Result::Next;

        _download_data.current_chunk++;
        _download_data.total_bytes_written += chunk_bytes;
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
    } else {
        // Check for missing bins when we might have all bins for this chunk
        // This handles: receiving the last expected bin AND receiving retried bins that might
        // complete the chunk
        if (bins_in_chunk > 0) {
            const uint32_t expected_last_bin = bins_in_chunk - 1;
            // Check if this could be the last bin we need (either the expected last one, or a
            // retried one)
            if (bin == expected_last_bin || _download_data.chunk_bin_table[expected_last_bin]) {
                check_and_request_missing_bins();
            }
        }
    }
}

void LogFilesImpl::data_timeout()
{
    std::lock_guard<std::mutex> lock(_download_data_mutex);

    LogErr() << "Timeout!";
    LogErr() << "Requesting missing chunk:\t" << _download_data.current_chunk << "/"
             << _download_data.total_chunks();

    // Don't reset chunk data - preserve what we've received
    // Instead, request missing ranges based on bin table
    check_and_request_missing_bins();

    _download_data.timeout_cookie = _system_impl->register_timeout_handler(
        [this]() { LogFilesImpl::data_timeout(); }, _system_impl->timeout_s());
}

void LogFilesImpl::check_and_request_missing_bins()
{
    // Note: This function assumes _download_data_mutex is already locked by caller

    // Find missing ranges and request the first one
    const uint32_t chunk_start = _download_data.current_chunk * CHUNK_SIZE;
    const uint32_t bins_in_chunk = _download_data.bins_in_chunk();

    uint32_t range_start = 0;
    bool in_missing_range = false;
    bool requested_something = false;

    for (uint32_t bin = 0; bin <= bins_in_chunk; ++bin) {
        bool is_missing = (bin < bins_in_chunk) ? !_download_data.chunk_bin_table[bin] : false;

        if (is_missing && !in_missing_range) {
            // Start of a missing range
            range_start = bin;
            in_missing_range = true;
        } else if (!is_missing && in_missing_range) {
            // End of a missing range, request it (but only request the first one)
            if (!requested_something) {
                const uint32_t missing_start =
                    chunk_start + (range_start * MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN);
                const uint32_t missing_count =
                    (bin - range_start) * MAVLINK_MSG_LOG_DATA_FIELD_DATA_LEN;

                LogDebug() << "Requesting missing range from offset " << missing_start << " count "
                           << missing_count;
                request_log_data(_download_data.entry.id, missing_start, missing_count);
                requested_something = true;
            }
            in_missing_range = false;
        }
    }
}

void LogFilesImpl::check_and_request_missing_entries()
{
    // Note: This function assumes _entries_mutex is already locked by caller

    // Find missing entry ranges and request them
    std::vector<uint16_t> missing_ids;
    for (uint16_t i = 0; i < _log_entries.size(); ++i) {
        if (!_log_entries[i].has_value()) {
            // Convert storage index back to MAVLink ID using min_entry_id
            // Works for both 0-based (PX4) and 1-based (ArduPilot) indexing
            missing_ids.push_back(i + _min_entry_id);
        }
    }

    if (missing_ids.empty()) {
        return;
    }

    // Group consecutive missing IDs into ranges for efficient requests
    // For now, request the first missing range (similar to bins approach)
    uint16_t range_start = missing_ids[0];
    uint16_t range_end = range_start;

    // Find end of first consecutive range
    for (size_t i = 1; i < missing_ids.size(); ++i) {
        if (missing_ids[i] == missing_ids[i - 1] + 1) {
            range_end = missing_ids[i];
        } else {
            break;
        }
    }

    LogDebug() << "Requesting missing log entries from " << range_start << " to " << range_end;
    request_log_list(range_start, range_end);
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
