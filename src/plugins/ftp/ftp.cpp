#include "plugins/ftp/ftp.h"
#include "ftp_impl.h"

namespace mavsdk {

Ftp::Ftp(System& system) : PluginBase(), _impl{new FtpImpl(system)} {}

Ftp::~Ftp() {}

std::string Ftp::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::TIMEOUT:
            return "Command timeout";
        case Result::IN_PROGRESS:
            return "Operation already in progress";
        case Result::FILE_IO_ERROR:
            return "File IO error";
        case Result::INVALID_PARAMETER:
            return "Invalid parameter";
        case Result::FILE_EXISTS:
            return "File exists already";
        case Result::FILE_DOES_NOT_EXIST:
            return "File does not exist";
        case Result::FILE_PROTECTED:
            return "File is write protected";
        case Result::UNSUPPORTED:
            return "Unsupported command";
        case Result::PROTOCOL_ERROR:
            return "Protocol error";
        default:
            return "Unknown";
    }
}

void Ftp::set_root_dir(const std::string& root_dir)
{
    _impl->set_root_dir(root_dir);
}

void Ftp::set_target_component_id(uint8_t component_id)
{
    _impl->set_target_component_id(component_id);
}

uint8_t Ftp::get_our_compid() const
{
    return _impl->get_our_compid();
}

void Ftp::reset_async(result_callback_t callback)
{
    _impl->reset_async(callback);
}

void Ftp::download_async(
    const std::string& remote_file_path,
    const std::string& local_folder,
    progress_callback_t progress_callback,
    result_callback_t result_callback)
{
    _impl->download_async(remote_file_path, local_folder, progress_callback, result_callback);
}

void Ftp::upload_async(
    const std::string& local_file_path,
    const std::string& remote_folder,
    progress_callback_t progress_callback,
    result_callback_t result_callback)
{
    _impl->upload_async(local_file_path, remote_folder, progress_callback, result_callback);
}

void Ftp::list_directory_async(
    const std::string& path, directory_items_and_result_callback_t callback)
{
    _impl->list_directory_async(path, callback);
}

void Ftp::create_directory_async(const std::string& path, result_callback_t callback)
{
    _impl->create_directory_async(path, callback);
}

void Ftp::remove_directory_async(const std::string& path, result_callback_t callback)
{
    _impl->remove_directory_async(path, callback);
}

void Ftp::remove_file_async(const std::string& path, result_callback_t callback)
{
    _impl->remove_file_async(path, callback);
}

void Ftp::rename_async(
    const std::string& fromPath, const std::string& toPath, result_callback_t callback)
{
    _impl->rename_async(fromPath, toPath, callback);
}

void Ftp::are_files_identical_async(
    const std::string& local_path,
    const std::string& remote_path,
    are_files_identical_callback_t callback)
{
    _impl->are_files_identical_async(local_path, remote_path, callback);
}

} // namespace mavsdk
