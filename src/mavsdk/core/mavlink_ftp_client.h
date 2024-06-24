#pragma once

#include <cinttypes>
#include <functional>
#include <fstream>
#include <deque>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "mavlink_include.h"
#include "locked_queue.h"
#include "timeout_handler.h"

// As found in
// https://stackoverflow.com/questions/1537964#answer-3312896
#ifdef _MSC_VER // MSVC
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#else
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

namespace mavsdk {

class SystemImpl;

class MavlinkFtpClient {
public:
    explicit MavlinkFtpClient(SystemImpl& system_impl);
    ~MavlinkFtpClient();

    /**
     * @brief Possible results returned for FTP commands
     */
    enum class ClientResult {
        Unknown, /**< @brief Unknown result. */
        Success, /**< @brief Success. */
        Next, /**< @brief Intermediate message showing progress. */
        Timeout, /**< @brief Timeout. */
        Busy, /**< @brief Operation is already in progress. */
        FileIoError, /**< @brief File IO operation error. */
        FileExists, /**< @brief File exists already. */
        FileDoesNotExist, /**< @brief File does not exist. */
        FileProtected, /**< @brief File is write protected. */
        InvalidParameter, /**< @brief Invalid parameter. */
        Unsupported, /**< @brief Unsupported command. */
        ProtocolError, /**< @brief General protocol error. */
        NoSystem, /**< @brief No system connected. */
    };

    friend std::ostream& operator<<(std::ostream& str, ClientResult const& result);

    struct ProgressData {
        uint32_t bytes_transferred{}; /**< @brief The number of bytes already transferred. */
        uint32_t total_bytes{}; /**< @brief The total bytes to transfer. */
    };

    using ResultCallback = std::function<void(ClientResult)>;
    using UploadCallback = std::function<void(ClientResult, ProgressData)>;
    using DownloadCallback = std::function<void(ClientResult, ProgressData)>;
    using ListDirectoryCallback =
        std::function<void(ClientResult, std::vector<std::string>, std::vector<std::string>)>;
    using AreFilesIdenticalCallback = std::function<void(ClientResult, bool)>;

    void do_work();

    void reset_async(ResultCallback callback);
    void download_async(
        const std::string& remote_file_path,
        const std::string& local_folder,
        bool use_burst,
        DownloadCallback callback,
        std::optional<uint8_t> maybe_target_compid = {});
    void upload_async(
        const std::string& local_file_path,
        const std::string& remote_folder,
        UploadCallback callback);
    void list_directory_async(const std::string& path, ListDirectoryCallback callback);
    void create_directory_async(const std::string& path, ResultCallback callback);
    void remove_directory_async(const std::string& path, ResultCallback callback);
    void remove_file_async(const std::string& path, ResultCallback callback);
    void
    rename_async(const std::string& from_path, const std::string& to_path, ResultCallback callback);
    void are_files_identical_async(
        const std::string& local_path,
        const std::string& remote_path,
        AreFilesIdenticalCallback callback);

    ClientResult set_root_directory(const std::string& root_dir);
    uint8_t get_our_compid();
    ClientResult set_target_compid(uint8_t component_id);

private:
    static constexpr unsigned RETRIES = 10;

    /// @brief Maximum data size in RequestHeader::data
    static constexpr uint8_t max_data_length = 239;

    /// @brief This is the payload which is in mavlink_file_transfer_protocol_t.payload.
    /// This needs to be packed, because it's typecasted from
    /// mavlink_file_transfer_protocol_t.payload, which starts at a 3 byte offset, causing an
    /// unaligned access to seq_number and offset
    PACK(struct PayloadHeader {
        uint16_t seq_number; ///< sequence number for message
        uint8_t session; ///< Session id for read and write commands
        uint8_t opcode; ///< Command opcode
        uint8_t size; ///< Size of data
        uint8_t req_opcode; ///< Request opcode returned in RSP_ACK, RSP_NAK message
        uint8_t burst_complete; ///< Only used if req_opcode=CMD_BURST_READ_FILE - 1: set of burst
        ///< packets complete, 0: More burst packets coming.
        uint8_t padding; ///< 32 bit alignment padding
        uint32_t offset; ///< Offsets for List and Read commands
        uint8_t data[max_data_length]; ///< command data, varies by Opcode
    });

    /// @brief Command opcodes
    enum Opcode : uint8_t {
        CMD_NONE, ///< ignored, always acked
        CMD_TERMINATE_SESSION, ///< Terminates open Read session
        CMD_RESET_SESSIONS, ///< Terminates all open Read sessions
        CMD_LIST_DIRECTORY, ///< List files in <path> from <offset>
        CMD_OPEN_FILE_RO, ///< Opens file at <path> for reading, returns <session>
        CMD_READ_FILE, ///< Reads <size> bytes from <offset> in <session>
        CMD_CREATE_FILE, ///< Creates file at <path> for writing, returns <session>
        CMD_WRITE_FILE, ///< Writes <size> bytes to <offset> in <session>
        CMD_REMOVE_FILE, ///< Remove file at <path>
        CMD_CREATE_DIRECTORY, ///< Creates directory at <path>
        CMD_REMOVE_DIRECTORY, ///< Removes Directory at <path>, must be empty
        CMD_OPEN_FILE_WO, ///< Opens file at <path> for writing, returns <session>
        CMD_TRUNCATE_FILE, ///< Truncate file at <path> to <offset> length
        CMD_RENAME, ///< Rename <path1> to <path2>
        CMD_CALC_FILE_CRC32, ///< Calculate CRC32 for file at <path>
        CMD_BURST_READ_FILE, ///< Burst download session file

        RSP_ACK = 128, ///< Ack response
        RSP_NAK ///< Nak response
    };

    struct DownloadItem {
        std::string remote_path{};
        std::string local_folder{};
        DownloadCallback callback{};
        std::ofstream ofstream{};
        std::size_t file_size{0};
        std::size_t bytes_transferred{0};
        int last_progress_percentage{-1};
    };

    struct DownloadBurstItem {
        std::string remote_path{};
        std::string local_folder{};
        DownloadCallback callback{};
        std::ofstream ofstream{};
        uint32_t file_size{0};
        struct MissingData {
            size_t offset;
            size_t size;
        };
        std::deque<MissingData> missing_data{};
        size_t current_offset{0};
    };

    struct UploadItem {
        std::string local_file_path{};
        std::string remote_folder{};
        UploadCallback callback{};
        std::ifstream ifstream{};
        std::size_t file_size{0};
        std::size_t bytes_transferred{0};
        int last_progress_percentage{-1};
    };

    struct RemoveItem {
        std::string path{};
        ResultCallback callback{};
    };

    struct RenameItem {
        std::string from_path{};
        std::string to_path{};
        ResultCallback callback{};
    };

    struct CreateDirItem {
        std::string path{};
        ResultCallback callback{};
    };

    struct RemoveDirItem {
        std::string path{};
        ResultCallback callback{};
    };

    struct CompareFilesItem {
        std::string local_path{};
        std::string remote_path{};
        uint32_t local_crc{};
        AreFilesIdenticalCallback callback{};
    };

    struct ListDirItem {
        std::string path{};
        ListDirectoryCallback callback{};
        uint32_t offset{0};
        std::vector<std::string> dirs{};
        std::vector<std::string> files{};
    };

    using Item = std::variant<
        DownloadItem,
        DownloadBurstItem,
        UploadItem,
        RemoveItem,
        RenameItem,
        CreateDirItem,
        RemoveDirItem,
        CompareFilesItem,
        ListDirItem>;

    struct Work {
        Item item;
        PayloadHeader payload{}; // The last payload saved for retries
        unsigned retries{RETRIES};
        bool started{false};
        Opcode last_opcode{};
        uint16_t last_received_seq_number{0};
        uint16_t last_sent_seq_number{0};
        uint8_t target_compid{};
        Work(Item new_item, uint8_t target_compid_) :
            item(std::move(new_item)),
            target_compid(target_compid_)
        {}
    };

    /// @brief Possible server results returned for requests.
    enum ServerResult : uint8_t {
        SUCCESS,
        ERR_FAIL, ///< Unknown failure
        ERR_FAIL_ERRNO, ///< Command failed, errno sent back in PayloadHeader.data[1]
        ERR_INVALID_DATA_SIZE, ///< PayloadHeader.size is invalid
        ERR_INVALID_SESSION, ///< Session is not currently open
        ERR_NO_SESSIONS_AVAILABLE, ///< All available Sessions in use
        ERR_EOF, ///< Offset past end of file for List and Read commands
        ERR_UNKOWN_COMMAND, ///< Unknown command opcode
        ERR_FAIL_FILE_EXISTS, ///< File exists already
        ERR_FAIL_FILE_PROTECTED, ///< File is write protected
        ERR_FAIL_FILE_DOES_NOT_EXIST, ///< File does not exist

        // These error codes are returned to client without contacting the server
        ERR_TIMEOUT = 200, ///< Timeout
        ERR_FILE_IO_ERROR, ///< File IO operation error
    };

    static constexpr auto DIRENT_FILE = "F"; ///< Identifies File returned from List command
    static constexpr auto DIRENT_DIR = "D"; ///< Identifies Directory returned from List command
    static constexpr auto DIRENT_SKIP = "S"; ///< Identifies Skipped entry from List command

    static_assert(
        sizeof(PayloadHeader) == sizeof(mavlink_file_transfer_protocol_t::payload),
        "PayloadHeader size is incorrect.");

    struct SessionInfo {
        int fd{-1};
        uint32_t file_size{0};
        bool stream_download{false};
        uint32_t stream_offset{0};
        uint16_t stream_seq_number{0};
        uint8_t stream_target_system_id{0};
        unsigned stream_chunk_transmitted{0};
    };

    bool download_start(Work& work, DownloadItem& item);
    bool download_continue(Work& work, DownloadItem& item, PayloadHeader* payload);

    bool download_burst_start(Work& work, DownloadBurstItem& item);
    bool download_burst_continue(Work& work, DownloadBurstItem& item, PayloadHeader* payload);
    void download_burst_end(Work& work);
    void request_burst(Work& work, DownloadBurstItem& item);
    void request_next_rest(Work& work, DownloadBurstItem& item);
    size_t burst_bytes_transferred(DownloadBurstItem& item);

    bool upload_start(Work& work, UploadItem& item);
    bool upload_continue(Work& work, UploadItem& item);

    bool remove_start(Work& work, RemoveItem& item);

    bool rename_start(Work& work, RenameItem& item);

    bool create_dir_start(Work& work, CreateDirItem& item);

    bool remove_dir_start(Work& work, RemoveDirItem& item);

    bool compare_files_start(Work& work, CompareFilesItem& item);

    bool list_dir_start(Work& work, ListDirItem& item);
    bool list_dir_continue(Work& work, ListDirItem& item, PayloadHeader* payload);

    void terminate_session(Work& work);

    static ClientResult result_from_nak(PayloadHeader* payload);

    void timeout();
    void start_timer(std::optional<double> duration_s = {});
    void stop_timer();

    ClientResult calc_local_file_crc32(const std::string& path, uint32_t& csum);

    static ClientResult translate(ServerResult result);
    void send_mavlink_ftp_message(const PayloadHeader& payload, uint8_t target_compid);

    uint8_t get_target_component_id();

    void process_mavlink_ftp_message(const mavlink_message_t& msg);

    SystemImpl& _system_impl;

    uint8_t _target_component_id = 0;
    bool _target_component_id_set{false};
    uint8_t _session = 0;
    uint8_t _network_id = 0;

    TimeoutHandler::Cookie _timeout_cookie{};

    LockedQueue<Work> _work_queue{};

    bool _debugging{false};
};

} // namespace mavsdk
