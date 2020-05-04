#include "plugins/log_files/log_files.h"
#include "log_files_impl.h"

namespace mavsdk {

LogFiles::LogFiles(System& system) : PluginBase(), _impl{new LogFilesImpl(system)} {}

LogFiles::~LogFiles() {}

std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> LogFiles::get_entries()
{
    return _impl->get_entries();
}

void LogFiles::get_entries_async(get_entries_callback_t callback)
{
    _impl->get_entries_async(callback);
}

LogFiles::Result LogFiles::download_log_file(unsigned id, const std::string& file_path)
{
    return _impl->download_log_file(id, file_path);
}

void LogFiles::download_log_file_async(
    unsigned id, const std::string& file_path, download_log_file_callback_t callback)
{
    _impl->download_log_file_async(id, file_path, callback);
}

const char* LogFiles::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::PROGRESS:
            return "Progress";
        case Result::NO_LOGFILES:
            return "No logfiles";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::UNKNOWN:
        default:
            return "Unknown";
    }
}

} // namespace mavsdk
