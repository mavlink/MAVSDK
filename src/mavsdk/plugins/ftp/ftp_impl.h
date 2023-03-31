#pragma once

#include <fstream>
#include <mutex>
#include <string>

#include "mavlink_include.h"
#include "plugins/ftp/ftp.h"
#include "plugin_impl_base.h"

// As found in
// https://stackoverflow.com/questions/1537964#answer-3312896
#ifdef _MSC_VER // MSVC
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#else
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

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

    std::pair<Ftp::Result, std::vector<std::string>> list_directory(const std::string& path);
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
        Ftp::DownloadCallback callback);
    void upload_async(
        const std::string& local_file_path,
        const std::string& remote_folder,
        Ftp::UploadCallback callback);
    void list_directory_async(
        const std::string& path, Ftp::ListDirectoryCallback callback, uint32_t offset = 0);
    void create_directory_async(const std::string& path, Ftp::ResultCallback callback);
    void remove_directory_async(const std::string& path, Ftp::ResultCallback callback);
    void remove_file_async(const std::string& path, Ftp::ResultCallback callback);
    void rename_async(
        const std::string& from_path, const std::string& to_path, Ftp::ResultCallback callback);
    void are_files_identical_async(
        const std::string& local_path,
        const std::string& remote_path,
        Ftp::AreFilesIdenticalCallback callback);

    void set_retries(uint32_t retries);
    Ftp::Result set_root_directory(const std::string& root_dir);
    uint8_t get_our_compid() { return _system_impl->get_own_component_id(); };
    Ftp::Result set_target_compid(uint8_t component_id);

private:
    Ftp::Result result_from_mavlink_ftp_result(MavlinkFtp::ClientResult result);
    Ftp::ProgressData
    progress_data_from_mavlink_ftp_progress_data(MavlinkFtp::ProgressData progress_data);
};

} // namespace mavsdk
