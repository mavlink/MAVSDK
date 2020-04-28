#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

// This plugin provides/includes the mavlink 2.0 header files.
#include "mavlink/v2.0/common/mavlink.h"
#include "plugin_base.h"

namespace mavsdk {

class System;
class FtpImpl;

/**
 * @brief The Ftp class provides file uploading and downloading using MAVLink FTP.
 *
 */
class Ftp : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto ftp = std::make_shared<Ftp>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Ftp(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Ftp();

    /**
     * @brief Possible results returned for requests.
     */
    enum class Result {
        SUCCESS,
        TIMEOUT, ///< Timeout
        IN_PROGRESS, ///< Operation is already in progress
        FILE_IO_ERROR, ///< File IO operation error
        FILE_EXISTS, ///< File exists already
        FILE_DOES_NOT_EXIST, ///< File does not exist
        FILE_PROTECTED, ///< File is write protected
        INVALID_PARAMETER, ///< Invalid parameter
        UNSUPPORTED, ///< Unsupported command
        PROTOCOL_ERROR ///< General protocol error
    };

    /**
     * @brief Returns a human-readable English string for `Ftp::Result`.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the `Ftp::Result`.
     */
    std::string result_str(Result result);

    /**
     * @brief Result callback type for async ftp calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Progress callback type for async ftp calls.
     */
    typedef std::function<void(uint32_t bytes_read, uint32_t total_bytes)> progress_callback_t;

    /**
     * @brief Callback type for `list_directory_async()` call to get directory items and result.
     */
    typedef std::function<void(Result, std::vector<std::string>)>
        directory_items_and_result_callback_t;

    /**
     * @brief Callback type for `are_files_identical()` call.
     */
    typedef std::function<void(Result, bool)> are_files_identical_callback_t;

    /**
     * @brief Resets FTP server in case there are stale open sessions (asynchronous).
     *
     * @param callback Callback to receive result of this request.
     */
    void reset_async(result_callback_t callback);

    /**
     * @brief Downloads a file to local folder (asynchronous).
     *
     * @param remote_file_path Remote file to download
     * @param local_folder Local folder where downloaded file will be stored
     * @param progress_callback Callback to receive progress of this request.
     * @param result_callback Callback to receive result of this request.
     */
    void download_async(
        const std::string& remote_file_path,
        const std::string& local_folder,
        progress_callback_t progress_callback,
        result_callback_t result_callback);

    /**
     * @brief Uploads local file to remote folder (asynchronous).
     *
     * @param local_file_path Local file to upload
     * @param remote_folder Remote folder where uploaded file will be stored
     * @param progress_callback Callback to receive progress of this request.
     * @param result_callback Callback to receive result of this request.
     */
    void upload_async(
        const std::string& local_file_path,
        const std::string& remote_folder,
        progress_callback_t progress_callback,
        result_callback_t result_callback);

    /**
     * @brief Downloads a vector of directory items from the system (asynchronous).
     *
     * @param path Path of a directory to list
     * @param callback Callback to receive directory entries and result of this request.
     */
    void
    list_directory_async(const std::string& path, directory_items_and_result_callback_t callback);

    /**
     * @brief Creates directory (asynchronous).
     *
     * @param path Path of a directory to create
     * @param callback Callback to receive result of this request.
     */
    void create_directory_async(const std::string& path, result_callback_t callback);

    /**
     * @brief Removes directory (asynchronous).
     *
     * @param path Path of a directory to remove
     * @param callback Callback to receive result of this request.
     */
    void remove_directory_async(const std::string& path, result_callback_t callback);

    /**
     * @brief Removes file (asynchronous).
     *
     * @param path Path of a file to remove
     * @param callback Callback to receive result of this request.
     */
    void remove_file_async(const std::string& path, result_callback_t callback);

    /**
     * @brief Renames fromPath to toPath (asynchronous).
     *
     * @param from_path Path to rename
     * @param to_path Destination path
     * @param callback Callback to receive result of this request.
     */
    void rename_async(
        const std::string& from_path, const std::string& to_path, result_callback_t callback);

    /**
     * @brief Compare two files using CRC32 checksum.
     *
     * @param local_path The path of the local file.
     * @param remote_path The path of the remote file.
     * @param callback The callback for the result
     */
    void are_files_identical_async(
        const std::string& local_path,
        const std::string& remote_path,
        are_files_identical_callback_t callback);

    /**
     * @brief Set root dir for Mavlink FTP server.
     *
     * @param root_dir Root dir for
     */
    void set_root_dir(const std::string& root_dir);

    /**
     * @brief Set target component id. By default it is the autopilot
     *
     * @param component_id Target component id
     */
    void set_target_component_id(uint8_t component_id);

    /**
     * @brief Get our own component ID.
     *
     * @return our own component ID.
     */
    uint8_t get_our_compid() const;

    /**
     * @brief Copy Constructor (object is not copyable).
     */
    Ftp(const Ftp&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Ftp& operator=(const Ftp&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<FtpImpl> _impl;
};

} // namespace mavsdk
