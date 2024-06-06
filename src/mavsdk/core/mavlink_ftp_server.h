#pragma once

#include <cinttypes>
#include <fstream>
#include <unordered_map>
#include <mutex>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <thread>

#include "mavlink_include.h"

// As found in
// https://stackoverflow.com/questions/1537964#answer-3312896
#ifdef _MSC_VER // MSVC
#define PACK(__Declaration__) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#else
#define PACK(__Declaration__) __Declaration__ __attribute__((__packed__))
#endif

namespace mavsdk {

class ServerComponentImpl;

class MavlinkFtpServer {
public:
    explicit MavlinkFtpServer(ServerComponentImpl& server_component_impl);
    ~MavlinkFtpServer();

    struct ProgressData {
        uint32_t bytes_transferred{}; /**< @brief The number of bytes already transferred. */
        uint32_t total_bytes{}; /**< @brief The total bytes to transfer. */
    };

    void set_root_directory(const std::string& root_dir);

private:
    ServerComponentImpl& _server_component_impl;

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

    /// @brief Command opcodes
    enum class Opcode : uint8_t {
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

    /// @brief Maximum data size in RequestHeader::data
    static constexpr uint8_t max_data_length = 239;

    /// @brief This is the payload which is in mavlink_file_transfer_protocol_t.payload.
    /// This needs to be packed, because it's typecasted from
    /// mavlink_file_transfer_protocol_t.payload, which starts at a 3 byte offset, causing an
    /// unaligned access to seq_number and offset
    PACK(struct PayloadHeader {
        uint16_t seq_number; ///< sequence number for message
        uint8_t session; ///< Session id for read and write commands
        Opcode opcode; ///< Command opcode
        uint8_t size; ///< Size of data
        Opcode req_opcode; ///< Request opcode returned in RSP_ACK, RSP_NAK message
        uint8_t burst_complete; ///< Only used if req_opcode=CMD_BURST_READ_FILE - 1: set of burst
        ///< packets complete, 0: More burst packets coming.
        uint8_t padding; ///< 32 bit alignment padding
        uint32_t offset; ///< Offsets for List and Read commands
        uint8_t data[max_data_length]; ///< command data, varies by Opcode
    });

    static_assert(
        sizeof(PayloadHeader) == sizeof(mavlink_file_transfer_protocol_t::payload),
        "PayloadHeader size is incorrect.");

    ServerResult _calc_local_file_crc32(const std::string& path, uint32_t& csum);

    void _send_mavlink_ftp_message(const PayloadHeader& payload);

    void _reset();

    void process_mavlink_ftp_message(const mavlink_message_t& msg);

    std::string _data_as_string(const PayloadHeader& payload, size_t entry = 0);
    std::variant<std::string, ServerResult>
    _path_from_payload(const PayloadHeader& payload, size_t entry = 0);
    std::variant<std::string, ServerResult> _path_from_string(const std::string& payload_path);

    void _work_list(const PayloadHeader& payload);
    void _work_open_file_readonly(const PayloadHeader& payload);
    void _work_open_file_writeonly(const PayloadHeader& payload);
    void _work_create_file(const PayloadHeader& payload);
    void _work_read(const PayloadHeader& payload);
    void _work_burst(const PayloadHeader& payload);
    void _work_write(const PayloadHeader& payload);
    void _work_terminate(const PayloadHeader& payload);
    void _work_reset(const PayloadHeader& payload);
    void _work_remove_directory(const PayloadHeader& payload);
    void _work_create_directory(const PayloadHeader& payload);
    void _work_remove_file(const PayloadHeader& payload);
    void _work_rename(const PayloadHeader& payload);
    void _work_calc_file_CRC32(const PayloadHeader& payload);

    bool _send_burst_packet();
    void _make_burst_packet(PayloadHeader& packet);

    std::mutex _mutex{};
    struct SessionInfo {
        uint32_t file_size{0};
        uint32_t burst_offset{0};
        uint8_t burst_chunk_size{0};
        std::ifstream ifstream;
        std::ofstream ofstream;
        bool burst_stop{false};
        std::thread burst_thread;
    } _session_info{};

    uint8_t _network_id = 0;
    uint8_t _target_system_id = 0;
    uint8_t _target_component_id = 0;
    std::string _root_dir{};

    std::mutex _tmp_files_mutex{};
    std::unordered_map<std::string, std::string> _tmp_files{};
    std::string _tmp_dir{};

    uint16_t _burst_seq = 0;

    bool _debugging{false};
};

} // namespace mavsdk
