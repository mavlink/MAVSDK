#include "global_include.h"
#include "log_files_impl.h"
#include "dronecode_sdk_impl.h"
#include <fstream>

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

    _parent->register_mavlink_message_handler(
        MAVLINK_MSG_ID_LOG_DATA, std::bind(&LogFilesImpl::process_log_data, this, _1), this);
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
        std::bind(&LogFilesImpl::list_timeout, this), 0.5, &_entries.cookie);

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
                std::bind(&LogFilesImpl::list_timeout, this), 0.5, &_entries.cookie);
            _entries.retries++;
        }
    }
}

void LogFilesImpl::download_log_file_async(unsigned id,
                                           const std::string &file_path,
                                           LogFiles::download_log_file_callback_t callback)
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

        // TODO: check for busy
        _data.rerequesting = false;
        _data.id = id;
        _data.bytes.resize(bytes_to_get);
        _data.num_chunks = bytes_to_get / CHUNK_SIZE;
        _data.file_path = file_path;
        if (bytes_to_get % CHUNK_SIZE) {
            ++_data.num_chunks;
        }
        _data.chunks_received.resize(_data.num_chunks);

        _parent->register_timeout_handler(
            std::bind(&LogFilesImpl::data_timeout, this), 0.2, &_data.cookie);
    }

    request_log_data(id, 0, bytes_to_get);

    UNUSED(callback);
}

void LogFilesImpl::process_log_data(const mavlink_message_t &message)
{
    mavlink_log_data_t log_data;
    mavlink_msg_log_data_decode(&message, &log_data);

#if 0
    // To test retransmission
    static unsigned counter = 0;
    if (counter < 3 && (log_data.ofs == 1800 || log_data.ofs == 3600)) {
        ++counter;
        return;
    }
#endif

    {
        std::lock_guard<std::mutex> lock(_data.mutex);

        _parent->refresh_timeout_handler(_data.cookie);

        LogDebug() << "Received log data id: " << int(log_data.id) << ", ofs: " << int(log_data.ofs)
                   << ", count: " << int(log_data.count);

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

        if (log_data.ofs / CHUNK_SIZE + 1 == _data.chunks_received.size()) {
            _data.rerequesting = true;
        }
    }

    check_missing_log_data();
}

void LogFilesImpl::check_missing_log_data()
{
    {
        std::lock_guard<std::mutex> lock(_data.mutex);

        if (!_data.rerequesting) {
            return;
        }

        LogDebug() << "Checking if we received all log data";
        for (unsigned i = 0; i < _data.chunks_received.size(); ++i) {
            if (!_data.chunks_received[i]) {
                unsigned bytes_to_get = CHUNK_SIZE;
                if (i + 1 == _data.chunks_received.size()) {
                    bytes_to_get =
                        _data.bytes.size() - (_data.chunks_received.size() * CHUNK_SIZE - 1);
                }
                LogDebug() << "Re-requesting log data " << i * CHUNK_SIZE;
                request_log_data(_data.id, i * CHUNK_SIZE, bytes_to_get);
                return;
            }
        }

        LogDebug() << "Received all log data.";
        _parent->unregister_timeout_handler(_data.cookie);
    }

    write_log_data_to_disk();
}

void LogFilesImpl::request_log_data(unsigned id, unsigned chunk_id, unsigned bytes_to_get)
{
    mavlink_message_t msg;
    mavlink_msg_log_request_data_pack(GCSClient::system_id,
                                      GCSClient::component_id,
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
    std::lock_guard<std::mutex> lock(_data.mutex);
    _parent->register_timeout_handler(
        std::bind(&LogFilesImpl::data_timeout, this), 0.2, &_data.cookie);
    check_missing_log_data();
}

void LogFilesImpl::write_log_data_to_disk()
{
    std::lock_guard<std::mutex> lock(_data.mutex);

    std::ofstream out_file;
    out_file.open(_data.file_path, std::ios::out | std::ios::binary);
    out_file.write(reinterpret_cast<char *>(_data.bytes.data()), _data.bytes.size());
    out_file.close();
}

} // namespace dronecode_sdk
