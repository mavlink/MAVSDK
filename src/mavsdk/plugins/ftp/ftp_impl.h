#pragma once

#include <fstream>
#include <mutex>
#include <string>

#include "mavlink_include.h"
#include "plugins/ftp/ftp.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class FtpImpl : public PluginImplBase {
public:
    explicit FtpImpl(System& system);
    explicit FtpImpl(std::shared_ptr<System> system);
    FtpImpl(const FtpImpl&) = delete;
    const FtpImpl& operator=(const FtpImpl&) = delete;

    ~FtpImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    std::pair<Ftp::Result, Ftp::ListDirectoryData> list_directory(const std::string& path);
    Ftp::Result create_directory(const std::string& path);
    Ftp::Result remove_directory(const std::string& path);
    Ftp::Result remove_file(const std::string& path);
    Ftp::Result rename(const std::string& from_path, const std::string& to_path);
    std::pair<Ftp::Result, bool>
    are_files_identical(const std::string& local_path, const std::string& remote_path);

    void reset_async(Ftp::ResultCallback callback);
    void download_async(
        const std::string& remote_file_path,
        const std::string& local_folder,
        bool use_burst,
        Ftp::DownloadCallback callback);
    void upload_async(
        const std::string& local_file_path,
        const std::string& remote_folder,
        Ftp::UploadCallback callback);
    void list_directory_async(const std::string& path, Ftp::ListDirectoryCallback callback);
    void create_directory_async(const std::string& path, Ftp::ResultCallback callback);
    void remove_directory_async(const std::string& path, Ftp::ResultCallback callback);
    void remove_file_async(const std::string& path, Ftp::ResultCallback callback);
    void rename_async(
        const std::string& from_path, const std::string& to_path, Ftp::ResultCallback callback);
    void are_files_identical_async(
        const std::string& local_path,
        const std::string& remote_path,
        Ftp::AreFilesIdenticalCallback callback);

    Ftp::Result set_target_compid(uint8_t component_id);

private:
    Ftp::Result result_from_mavlink_ftp_result(MavlinkFtpClient::ClientResult result);
    Ftp::ProgressData
    progress_data_from_mavlink_ftp_progress_data(MavlinkFtpClient::ProgressData progress_data);
};

} // namespace mavsdk
