#include <functional>
#include <iostream>

#if defined(WINDOWS)
#include "tronkko_dirent.h"
#include "stackoverflow_unistd.h"
// Fix MSVC error C4003
// https://stackoverflow.com/a/6884102/8548472
#ifdef max
#undef max
#endif
#else
#include <dirent.h>
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>
#include <algorithm>

#include "crc32.h"
#include "fs.h"
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

void FtpImpl::reset_async(Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp().reset_async([callback, this](MavlinkFtp::ClientResult result) {
        callback(result_from_mavlink_ftp_result(result));
    });
}

void FtpImpl::download_async(
    const std::string& remote_path, const std::string& local_folder, Ftp::DownloadCallback callback)
{
    _system_impl->mavlink_ftp().download_async(
        remote_path,
        local_folder,
        [callback, this](MavlinkFtp::ClientResult result, MavlinkFtp::ProgressData progress_data) {
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
    _system_impl->mavlink_ftp().upload_async(
        local_file_path,
        remote_folder,
        [callback, this](MavlinkFtp::ClientResult result, MavlinkFtp::ProgressData progress_data) {
            callback(
                result_from_mavlink_ftp_result(result),
                progress_data_from_mavlink_ftp_progress_data(progress_data));
        });
}

std::pair<Ftp::Result, std::vector<std::string>> FtpImpl::list_directory(const std::string& path)
{
    auto ret = _system_impl->mavlink_ftp().list_directory(path);
    return std::pair{result_from_mavlink_ftp_result(ret.first), ret.second};
}

void FtpImpl::list_directory_async(
    const std::string& path, Ftp::ListDirectoryCallback callback, uint32_t offset)
{
    _system_impl->mavlink_ftp().list_directory_async(
        path,
        [callback, this](MavlinkFtp::ClientResult result, auto&& dirs) {
            callback(result_from_mavlink_ftp_result(result), dirs);
        },
        offset);
}

Ftp::Result FtpImpl::create_directory(const std::string& path)
{
    return result_from_mavlink_ftp_result(_system_impl->mavlink_ftp().create_directory(path));
}

void FtpImpl::create_directory_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp().create_directory_async(
        path, [callback, this](MavlinkFtp::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

Ftp::Result FtpImpl::remove_directory(const std::string& path)
{
    return result_from_mavlink_ftp_result(_system_impl->mavlink_ftp().remove_directory(path));
}

void FtpImpl::remove_directory_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp().remove_directory_async(
        path, [callback, this](MavlinkFtp::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

Ftp::Result FtpImpl::remove_file(const std::string& path)
{
    return result_from_mavlink_ftp_result(_system_impl->mavlink_ftp().remove_file(path));
}

void FtpImpl::remove_file_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp().remove_file_async(
        path, [callback, this](MavlinkFtp::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

Ftp::Result FtpImpl::rename(const std::string& from_path, const std::string& to_path)
{
    return result_from_mavlink_ftp_result(_system_impl->mavlink_ftp().rename(from_path, to_path));
}

void FtpImpl::rename_async(
    const std::string& from_path, const std::string& to_path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp().rename_async(
        from_path, to_path, [callback, this](MavlinkFtp::ClientResult result) {
            callback(result_from_mavlink_ftp_result(result));
        });
}

std::pair<Ftp::Result, bool>
FtpImpl::are_files_identical(const std::string& local_path, const std::string& remote_path)
{
    auto ret = _system_impl->mavlink_ftp().are_files_identical(local_path, remote_path);
    return std::pair<Ftp::Result, bool>{result_from_mavlink_ftp_result(ret.first), ret.second};
}

void FtpImpl::are_files_identical_async(
    const std::string& local_path,
    const std::string& remote_path,
    Ftp::AreFilesIdenticalCallback callback)
{
    _system_impl->mavlink_ftp().are_files_identical_async(
        local_path, remote_path, [callback, this](MavlinkFtp::ClientResult result, bool identical) {
            callback(result_from_mavlink_ftp_result(result), identical);
        });
}

Ftp::Result FtpImpl::set_root_directory(const std::string& root_dir)
{
    return result_from_mavlink_ftp_result(_system_impl->mavlink_ftp().set_root_directory(root_dir));
}

void FtpImpl::set_retries(uint32_t retries)
{
    _system_impl->mavlink_ftp().set_retries(retries);
}

Ftp::Result FtpImpl::set_target_compid(uint8_t component_id)
{
    return result_from_mavlink_ftp_result(
        _system_impl->mavlink_ftp().set_target_compid(component_id));
}

Ftp::Result FtpImpl::result_from_mavlink_ftp_result(MavlinkFtp::ClientResult result)
{
    switch (result) {
        case MavlinkFtp::ClientResult::Unknown:
            return Ftp::Result::Unknown;
        case MavlinkFtp::ClientResult::Success:
            return Ftp::Result::Success;
        case MavlinkFtp::ClientResult::Next:
            return Ftp::Result::Next;
        case MavlinkFtp::ClientResult::Timeout:
            return Ftp::Result::Timeout;
        case MavlinkFtp::ClientResult::Busy:
            return Ftp::Result::Busy;
        case MavlinkFtp::ClientResult::FileIoError:
            return Ftp::Result::FileIoError;
        case MavlinkFtp::ClientResult::FileExists:
            return Ftp::Result::FileExists;
        case MavlinkFtp::ClientResult::FileDoesNotExist:
            return Ftp::Result::FileDoesNotExist;
        case MavlinkFtp::ClientResult::FileProtected:
            return Ftp::Result::FileProtected;
        case MavlinkFtp::ClientResult::InvalidParameter:
            return Ftp::Result::InvalidParameter;
        case MavlinkFtp::ClientResult::Unsupported:
            return Ftp::Result::Unsupported;
        case MavlinkFtp::ClientResult::ProtocolError:
            return Ftp::Result::ProtocolError;
        case MavlinkFtp::ClientResult::NoSystem:
            return Ftp::Result::NoSystem;
        default:
            return Ftp::Result::Unknown;
    }
}

Ftp::ProgressData
FtpImpl::progress_data_from_mavlink_ftp_progress_data(MavlinkFtp::ProgressData progress_data)
{
    return {progress_data.bytes_transferred, progress_data.total_bytes};
}

} // namespace mavsdk
