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
            if (callback) {
                _system_impl->call_user_callback(
                    [temp_callback = callback, result, progress_data, this]() {
                        temp_callback(
                            result_from_mavlink_ftp_result(result),
                            progress_data_from_mavlink_ftp_progress_data(progress_data));
                    });
            }
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
            if (callback) {
                _system_impl->call_user_callback(
                    [temp_callback = callback, result, progress_data, this]() {
                        temp_callback(
                            result_from_mavlink_ftp_result(result),
                            progress_data_from_mavlink_ftp_progress_data(progress_data));
                    });
            }
        });
}

std::pair<Ftp::Result, Ftp::ListDirectoryData> FtpImpl::list_directory(const std::string& path)
{
    std::promise<std::pair<Ftp::Result, Ftp::ListDirectoryData>> prom;
    auto fut = prom.get_future();

    list_directory_async(path, [&](Ftp::Result result, Ftp::ListDirectoryData data) {
        prom.set_value(std::pair<Ftp::Result, Ftp::ListDirectoryData>(result, data));
    });
    return fut.get();
}

void FtpImpl::list_directory_async(const std::string& path, Ftp::ListDirectoryCallback callback)
{
    _system_impl->mavlink_ftp_client().list_directory_async(
        path,
        [callback, this](
            MavlinkFtpClient::ClientResult result,
            std::vector<std::string> dirs,
            std::vector<std::string> files) {
            if (callback) {
                _system_impl->call_user_callback([=]() {
                    callback(
                        result_from_mavlink_ftp_result(result),
                        Ftp::ListDirectoryData{dirs, files});
                });
            }
        });
}

Ftp::Result FtpImpl::create_directory(const std::string& path)
{
    std::promise<Ftp::Result> prom{};
    auto fut = prom.get_future();

    create_directory_async(path, [&](Ftp::Result result) { prom.set_value(result); });
    return fut.get();
}

void FtpImpl::create_directory_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().create_directory_async(
        path, [callback, this](MavlinkFtpClient::ClientResult result) {
            if (callback) {
                _system_impl->call_user_callback([temp_callback = callback, result, this]() {
                    temp_callback(result_from_mavlink_ftp_result(result));
                });
            }
        });
}

Ftp::Result FtpImpl::remove_directory(const std::string& path)
{
    std::promise<Ftp::Result> prom{};
    auto fut = prom.get_future();

    remove_directory_async(path, [&](Ftp::Result result) { prom.set_value(result); });

    return fut.get();
}

void FtpImpl::remove_directory_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().remove_directory_async(
        path, [callback, this](MavlinkFtpClient::ClientResult result) {
            if (callback) {
                _system_impl->call_user_callback([temp_callback = callback, result, this]() {
                    temp_callback(result_from_mavlink_ftp_result(result));
                });
            }
        });
}

Ftp::Result FtpImpl::remove_file(const std::string& path)
{
    std::promise<Ftp::Result> prom;
    auto fut = prom.get_future();

    remove_file_async(path, [&](Ftp::Result result) { prom.set_value(result); });

    return fut.get();
}

void FtpImpl::remove_file_async(const std::string& path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().remove_file_async(
        path, [callback, this](MavlinkFtpClient::ClientResult result) {
            if (callback) {
                _system_impl->call_user_callback([temp_callback = callback, result, this]() {
                    temp_callback(result_from_mavlink_ftp_result(result));
                });
            }
        });
}

Ftp::Result FtpImpl::rename(const std::string& from_path, const std::string& to_path)
{
    std::promise<Ftp::Result> prom{};
    auto fut = prom.get_future();

    rename_async(from_path, to_path, [&](Ftp::Result result) { prom.set_value(result); });

    return fut.get();
}

void FtpImpl::rename_async(
    const std::string& from_path, const std::string& to_path, Ftp::ResultCallback callback)
{
    _system_impl->mavlink_ftp_client().rename_async(
        from_path, to_path, [callback, this](MavlinkFtpClient::ClientResult result) {
            if (callback) {
                _system_impl->call_user_callback([temp_callback = callback, result, this]() {
                    temp_callback(result_from_mavlink_ftp_result(result));
                });
            }
        });
}

std::pair<Ftp::Result, bool>
FtpImpl::are_files_identical(const std::string& local_path, const std::string& remote_path)
{
    std::promise<std::pair<Ftp::Result, bool>> prom{};
    auto fut = prom.get_future();

    are_files_identical_async(local_path, remote_path, [&](Ftp::Result result, bool identical) {
        prom.set_value(std::pair<Ftp::Result, bool>{result, identical});
    });

    return fut.get();
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
            if (callback) {
                _system_impl->call_user_callback(
                    [temp_callback = callback, result, identical, this]() {
                        temp_callback(result_from_mavlink_ftp_result(result), identical);
                    });
            }
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
