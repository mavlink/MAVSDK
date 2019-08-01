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
class MavlinkFTPImpl;

/**
 * @brief The MavlinkFTP class provides file uploading and downloading using MAVLink FTP.
 *
 */
class MavlinkFTP : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto mavlink_ftp = std::make_shared<MavlinkFTP>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit MavlinkFTP(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~MavlinkFTP();

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
     * @brief Returns a human-readable English string for `MavlinkFTP::Result`.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the `MavlinkFTP::Result`.
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
     * @brief Callback type for `calc_file_crc32_async()` call to get file CRC32.
     */
    typedef std::function<void(Result, uint32_t)> file_crc32_result_callback_t;

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
     * @param callback Callback to receive result of this request.
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
     * @param callback Callback to receive result of this request.
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
     * @param fromPath Path to rename
     * @param toPath Destination path
     * @param callback Callback to receive result of this request.
     */
    void rename_async(
        const std::string& from_path, const std::string& to_path, result_callback_t callback);

    /**
     * @brief Calculate CRC32 for file (asynchronous).
     *
     * @param path Path to file for which CRC32 is calculated.
     * @param callback Callback to receive result of this request.
     */
    void calc_file_crc32_async(const std::string& path, file_crc32_result_callback_t callback);

    /**
     * @brief Calculate CRC32 for local file.
     *
     * @param path Path to local file for which CRC32 is calculated.
     * @param checksum CRC32 of the file
     * @return Result of the operation
     */
    MavlinkFTP::Result calc_local_file_crc32(const std::string& path, uint32_t& checksum);

    /**
     * @brief Set timeout for Mavlink FTP operation.
     *
     * @param timeout Timeout in milliseconds
     */
    void set_timeout(uint32_t timeout);

    /**
     * @brief Set number of retries after timeout for Mavlink FTP operation.
     *
     * @param retries Number of retries before ERR_TIMEOUT is issued
     */
    void set_retries(uint32_t retries);

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
    MavlinkFTP(const MavlinkFTP&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const MavlinkFTP& operator=(const MavlinkFTP&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<MavlinkFTPImpl> _impl;
};

} // namespace mavsdk
