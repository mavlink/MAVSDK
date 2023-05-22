#include <algorithm>
#include <functional>
#include <iostream>

#include "ftp_impl.h"
#include "system.h"

namespace mavsdk {

FtpImpl::FtpImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

FtpImpl::FtpImpl(std::shared_ptr<System> system) : PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

FtpImpl::~FtpImpl()
{
    _system_impl->unregister_plugin(this);
}

void FtpImpl::init() {}

void FtpImpl::deinit() {}

void FtpImpl::enable() {}

void FtpImpl::disable() {}

void FtpImpl::download_async(
    const std::string& remote_path,
    const std::string& local_folder,
    bool use_burst,
    Ftp::DownloadCallback callback)
{
    _system_impl->mavlink_ftp_client().download_async(
        remote_path,
        local_folder,
        use_burst,
        [callback, this](
            MavlinkFtpClient::ClientResult result, MavlinkFtpClient::ProgressData progress_data) {
            callback(
                result_from_mavlink_ftp_result(result),
                progress_data_from_mavlink_ftp_progress_data(progress_data));
        });
}

void FtpImpl::upload_async(
    const std::string& local_file_path,
    const std::string& remote_folder,
    Ftp::UploadCallback callback)
{
    _system_impl->mavlink_ftp_client().upload_async(
        local_file_path,
        remote_folder,
        [callback, this](
            MavlinkFtpClient::ClientResult result, MavlinkFtpClient::ProgressData progress_data) {
            callback(
                result_from_mavlink_ftp_result(result),
                progress_data_from_mavlink_ftp_progress_data(progress_data));
        });
}

std::pair<Ftp::Result, std::vector<std::string>> FtpImpl::list_directory(const std::string& path)
{
    auto ret = _system_impl->mavlink_ftp_client().list_directory(path);
    return std::pair{result_from_mavlink_ftp_result(ret.first), ret.second};
}

void FtpImpl::list_directory_async(const std::string& path, Ftp::ListDirectoryCallback callback)
{
    _system_impl->mavlink_ftp_client().list_directory_async(
        path, [callback, this](MavlinkFtpClient::ClientResult result, auto&& dirs) {
            callback(result_from_mavlink_ftp_result(result), dirs);
        });
}

Ftp::Result FtpImpl::create_directory(const std::string& path)
{
    return result_from_mavlink_ftp_result(
        _system_impl->mavlink_ftp_client().create_directory(path));
}

void FtpImpl::create_directory_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().create_directory_async(
        path, [callback, this](MavlinkFtpClient::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

Ftp::Result FtpImpl::remove_directory(const std::string& path)
{
    return result_from_mavlink_ftp_result(
        _system_impl->mavlink_ftp_client().remove_directory(path));
}

void FtpImpl::remove_directory_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().remove_directory_async(
        path, [callback, this](MavlinkFtpClient::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

Ftp::Result FtpImpl::remove_file(const std::string& path)
{
    return result_from_mavlink_ftp_result(_system_impl->mavlink_ftp_client().remove_file(path));
}

void FtpImpl::remove_file_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().remove_file_async(
        path, [callback, this](MavlinkFtpClient::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

Ftp::Result FtpImpl::rename(const std::string& from_path, const std::string& to_path)
{
    return result_from_mavlink_ftp_result(
        _system_impl->mavlink_ftp_client().rename(from_path, to_path));
}

void FtpImpl::rename_async(
    const std::string& from_path, const std::string& to_path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().rename_async(
        from_path, to_path, [callback, this](MavlinkFtpClient::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

std::pair<Ftp::Result, bool>
FtpImpl::are_files_identical(const std::string& local_path, const std::string& remote_path)
{
    auto ret = _system_impl->mavlink_ftp_client().are_files_identical(local_path, remote_path);
    return std::pair<Ftp::Result, bool>{result_from_mavlink_ftp_result(ret.first), ret.second};
}

void FtpImpl::are_files_identical_async(
    const std::string& local_path,
    const std::string& remote_path,
    Ftp::AreFilesIdenticalCallback callback)
{
    _system_impl->mavlink_ftp_client().are_files_identical_async(
        local_path,
        remote_path,
        [callback, this](MavlinkFtpClient::ClientResult result, bool identical) {
            callback(result_from_mavlink_ftp_result(result), identical);
        });
}

Ftp::Result FtpImpl::set_target_compid(uint8_t component_id)
{
    return result_from_mavlink_ftp_result(
        _system_impl->mavlink_ftp_client().set_target_compid(component_id));
}

Ftp::Result FtpImpl::result_from_mavlink_ftp_result(MavlinkFtpClient::ClientResult result)
{
    switch (result) {
        case MavlinkFtpClient::ClientResult::Unknown:
            return Ftp::Result::Unknown;
        case MavlinkFtpClient::ClientResult::Success:
            return Ftp::Result::Success;
        case MavlinkFtpClient::ClientResult::Next:
            return Ftp::Result::Next;
        case MavlinkFtpClient::ClientResult::Timeout:
            return Ftp::Result::Timeout;
        case MavlinkFtpClient::ClientResult::Busy:
            return Ftp::Result::Busy;
        case MavlinkFtpClient::ClientResult::FileIoError:
            return Ftp::Result::FileIoError;
        case MavlinkFtpClient::ClientResult::FileExists:
            return Ftp::Result::FileExists;
        case MavlinkFtpClient::ClientResult::FileDoesNotExist:
            return Ftp::Result::FileDoesNotExist;
        case MavlinkFtpClient::ClientResult::FileProtected:
            return Ftp::Result::FileProtected;
        case MavlinkFtpClient::ClientResult::InvalidParameter:
            return Ftp::Result::InvalidParameter;
        case MavlinkFtpClient::ClientResult::Unsupported:
            return Ftp::Result::Unsupported;
        case MavlinkFtpClient::ClientResult::ProtocolError:
            return Ftp::Result::ProtocolError;
        case MavlinkFtpClient::ClientResult::NoSystem:
            return Ftp::Result::NoSystem;
        default:
            return Ftp::Result::Unknown;
    }
}

Ftp::ProgressData
FtpImpl::progress_data_from_mavlink_ftp_progress_data(MavlinkFtpClient::ProgressData progress_data)
{
    return {progress_data.bytes_transferred, progress_data.total_bytes};
}

} // namespace mavsdk
