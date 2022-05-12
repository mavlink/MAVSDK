#include "mavlink_ftp.h"
#include "system_impl.h"
#include <fstream>

#if defined(WINDOWS)
#include "tronkko_dirent.h"
#include "stackoverflow_unistd.h"
#else
#include <dirent.h>
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/stat.h>

#ifndef O_ACCMODE
#define O_ACCMODE (O_RDONLY | O_WRONLY | O_RDWR)
#endif

#include "crc32.h"
#include "fs.h"
#include <algorithm>

namespace mavsdk {

bool seq_lt(uint16_t a, uint16_t b)
{
    // From https://en.wikipedia.org/wiki/Serial_number_arithmetic
    return (a < b && (b - a) < (std::numeric_limits<uint16_t>::max() / 2)) ||
           (a > b && (a - b) > (std::numeric_limits<uint16_t>::max() / 2));
}

MavlinkFtp::MavlinkFtp(SystemImpl& system_impl) : _system_impl(system_impl)
{
    _system_impl.register_mavlink_message_handler(
        MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL,
        [this](const mavlink_message_t& message) { process_mavlink_ftp_message(message); },
        this);
}

void MavlinkFtp::process_mavlink_ftp_message(const mavlink_message_t& msg)
{
    if (msg.msgid != MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL) {
        return;
    }

    bool stream_send = false;
    mavlink_file_transfer_protocol_t ftp_req;
    mavlink_msg_file_transfer_protocol_decode(&msg, &ftp_req);

    if (ftp_req.target_component != 0 && ftp_req.target_component != get_our_compid()) {
        LogWarn() << "wrong compid!";
        return;
    }

    PayloadHeader* payload = reinterpret_cast<PayloadHeader*>(&ftp_req.payload[0]);

    ServerResult error_code = ServerResult::SUCCESS;

    // basic sanity checks; must validate length before use
    if (payload->size > max_data_length) {
        error_code = ServerResult::ERR_INVALID_DATA_SIZE;
    } else {
        /*
            LogDebug() << "ftp - opc: " << (int)payload->opcode << " size: "
                << (int)payload->size << " offset: " << (int)payload->offset << " seq: " <<
        payload->seq_number;
        */

        // check the sequence number: if this is a resent request, resend the last response
        if (_last_reply_valid) {
            if (payload->seq_number + 1 == _last_reply_seq) {
                // This is the same request as the one we replied to last.
                LogWarn() << "Wrong sequence - resend last response";
                _system_impl.send_message(_last_reply);
                return;
            }
        }

        switch (payload->opcode) {
            case CMD_NONE:
                LogInfo() << "OPC:CMD_NONE";
                break;

            case CMD_TERMINATE_SESSION:
                LogInfo() << "OPC:CMD_TERMINATE_SESSION";
                error_code = _work_terminate(payload);
                break;

            case CMD_RESET_SESSIONS:
                LogInfo() << "OPC:CMD_RESET_SESSIONS";
                error_code = _work_reset(payload);
                break;

            case CMD_LIST_DIRECTORY:
                LogInfo() << "OPC:CMD_LIST_DIRECTORY";
                error_code = _work_list(payload);
                break;

            case CMD_OPEN_FILE_RO:
                LogInfo() << "OPC:CMD_OPEN_FILE_RO";
                error_code = _work_open(payload, O_RDONLY);
                break;

            case CMD_CREATE_FILE:
                LogInfo() << "OPC:CMD_CREATE_FILE";
                error_code = _work_open(payload, O_CREAT | O_TRUNC | O_WRONLY);
                break;

            case CMD_OPEN_FILE_WO:
                LogInfo() << "OPC:CMD_OPEN_FILE_WO";
                error_code = _work_open(payload, O_CREAT | O_WRONLY);
                break;

            case CMD_READ_FILE:
                LogInfo() << "OPC:CMD_READ_FILE";
                error_code = _work_read(payload);
                break;

            case CMD_BURST_READ_FILE:
                LogInfo() << "OPC:CMD_BURST_READ_FILE";
                error_code = _work_burst(payload);
                stream_send = true;
                break;

            case CMD_WRITE_FILE:
                LogInfo() << "OPC:CMD_WRITE_FILE";
                error_code = _work_write(payload);
                break;

            case CMD_REMOVE_FILE:
                LogInfo() << "OPC:CMD_REMOVE_FILE";
                error_code = _work_remove_file(payload);
                break;

            case CMD_RENAME:
                LogInfo() << "OPC:CMD_RENAME";
                error_code = _work_rename(payload);
                break;

            case CMD_CREATE_DIRECTORY:
                LogInfo() << "OPC:CMD_CREATE_DIRECTORY";
                error_code = _work_create_directory(payload);
                break;

            case CMD_REMOVE_DIRECTORY:
                LogInfo() << "OPC:CMD_REMOVE_DIRECTORY";
                error_code = _work_remove_directory(payload);
                break;

            case CMD_CALC_FILE_CRC32:
                LogInfo() << "OPC:CMD_CALC_FILE_CRC32";
                error_code = _work_calc_file_CRC32(payload);
                break;

            case RSP_ACK:
                _process_ack(payload);
                return;

            case RSP_NAK:
                _process_nak(payload);
                return;

            default:
                LogWarn() << "OPC:Unknown command: " << static_cast<int>(payload->opcode);
                error_code = ServerResult::ERR_UNKOWN_COMMAND;
                break;
        }
    }

    payload->seq_number++;

    // handle success vs. error
    if (error_code == ServerResult::SUCCESS) {
        payload->req_opcode = payload->opcode;
        payload->opcode = RSP_ACK;

    } else {
        uint8_t r_errno = errno;
        payload->req_opcode = payload->opcode;
        payload->opcode = RSP_NAK;
        payload->size = 1;

        if (r_errno == EEXIST) {
            error_code = ServerResult::ERR_FAIL_FILE_EXISTS;
        } else if (r_errno == ENOENT && error_code == ServerResult::ERR_FAIL_ERRNO) {
            error_code = ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
        }

        *reinterpret_cast<ServerResult*>(payload->data) = error_code;

        if (error_code == ServerResult::ERR_FAIL_ERRNO) {
            payload->size = 2;
            *reinterpret_cast<uint8_t*>(payload->data + 1) = r_errno;
        }
    }

    _last_reply_valid = false;

    // Stream download replies are sent through mavlink stream mechanism. Unless we need to Nack.
    if (!stream_send || error_code != ServerResult::SUCCESS) {
        // keep a copy of the last sent response ((n)ack), so that if it gets lost and the GCS
        // resends the request, we can simply resend the response.
        _last_reply_valid = true;
        _last_reply_seq = payload->seq_number;
        mavlink_msg_file_transfer_protocol_pack(
            _system_impl.get_own_system_id(),
            _system_impl.get_own_component_id(),
            &_last_reply,
            _network_id,
            _system_impl.get_system_id(),
            _get_target_component_id(),
            reinterpret_cast<const uint8_t*>(payload));
        _system_impl.send_message(_last_reply);
    }
}

MavlinkFtp::~MavlinkFtp() {}

void MavlinkFtp::_process_ack(PayloadHeader* payload)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);

    if (seq_lt(payload->seq_number, _seq_number)) {
        // (payload->seq_number < _seq_number) with wrap around
        // received an ack for a previous seq that we already considered done
        return;
    }

    if (_curr_op != payload->req_opcode) {
        LogWarn() << "Received ACK not matching our current operation";
        return;
    }

    switch (_curr_op) {
        case CMD_NONE:
            LogWarn() << "Received ACK without active operation";
            break;

        case CMD_OPEN_FILE_RO:
            _curr_op = CMD_NONE;
            _session_valid = true;
            _session = payload->session;
            _bytes_transferred = 0;
            _file_size = *(reinterpret_cast<uint32_t*>(payload->data));
            _call_op_progress_callback(_bytes_transferred, _file_size);
            _read();
            break;

        case CMD_READ_FILE:
            _ofstream.stream.write(reinterpret_cast<const char*>(payload->data), payload->size);
            if (!_ofstream.stream) {
                _session_result = ServerResult::ERR_FILE_IO_ERROR;
                _end_read_session();
                return;
            }
            _bytes_transferred += payload->size;
            _call_op_progress_callback(_bytes_transferred, _file_size);
            _read();
            break;

        case CMD_OPEN_FILE_WO:
            _curr_op = CMD_NONE;
            _session_valid = true;
            _session = payload->session;
            _bytes_transferred = 0;
            _call_op_progress_callback(_bytes_transferred, _file_size);
            _write();
            break;

        case CMD_WRITE_FILE:
            _call_op_progress_callback(_bytes_transferred, _file_size);
            _write();
            break;

        case CMD_TERMINATE_SESSION:
            _curr_op = CMD_NONE;
            _session_valid = false;
            _stop_timer();
            _call_op_result_callback(_session_result);
            break;

        case CMD_RESET_SESSIONS:
            _curr_op = CMD_NONE;
            _session_valid = false;
            _stop_timer();
            _call_op_result_callback(_session_result);
            break;

        case CMD_LIST_DIRECTORY: {
            bool added = false;
            uint8_t start = 0;
            for (uint8_t i = 0; i < payload->size; i++) {
                if (payload->data[i] == 0) {
                    std::string entry = std::string(reinterpret_cast<char*>(&payload->data[start]));
                    if (entry.length() > 0) {
                        added = true;
                        _curr_directory_list.emplace_back(entry);
                    }
                    start = i + 1;
                }
            }
            if (added) {
                // Ask for next batch of file names
                _list_directory(_curr_directory_list.size());
            } else {
                // We came to end - report entire list
                _curr_op = CMD_NONE;
                _stop_timer();
                _call_dir_items_result_callback(ServerResult::SUCCESS, _curr_directory_list);
            }
            break;
        }

        case CMD_CALC_FILE_CRC32: {
            _curr_op = CMD_NONE;
            uint32_t checksum = *reinterpret_cast<uint32_t*>(payload->data);
            _stop_timer();
            _call_crc32_result_callback(ServerResult::SUCCESS, checksum);
            break;
        }

        default:
            _curr_op = CMD_NONE;
            _stop_timer();
            _call_op_result_callback(ServerResult::SUCCESS);
            break;
    }
}

void MavlinkFtp::_process_nak(PayloadHeader* payload)
{
    if (payload != nullptr) {
        ServerResult sr = static_cast<ServerResult>(payload->data[0]);
        // PX4 Mavlink FTP returns "File doesn't exist" this way
        if (sr == ServerResult::ERR_FAIL_ERRNO && payload->data[1] == ENOENT) {
            sr = ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
        }
        _process_nak(sr);
    }
}

void MavlinkFtp::_process_nak(ServerResult result)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    switch (_curr_op) {
        case CMD_NONE:
            LogWarn() << "Received NAK without active operation";
            break;

        case CMD_OPEN_FILE_RO:
        case CMD_READ_FILE:
            _session_result = result;
            if (_session_valid) {
                const bool delete_file = (result == ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST);
                _end_read_session(delete_file);
            } else {
                _stop_timer();
                _call_op_result_callback(_session_result);
            }
            break;

        case CMD_OPEN_FILE_WO:
        case CMD_WRITE_FILE:
            _session_result = result;
            if (_session_valid) {
                _end_write_session();
            } else {
                _stop_timer();
                _call_op_result_callback(_session_result);
            }
            break;

        case CMD_TERMINATE_SESSION:
            _session_valid = false;
            _stop_timer();
            _call_op_result_callback(_session_result);
            break;

        case CMD_LIST_DIRECTORY:
            _stop_timer();
            if (!_curr_directory_list.empty()) {
                _call_dir_items_result_callback(ServerResult::SUCCESS, _curr_directory_list);
            } else {
                _call_dir_items_result_callback(result, _curr_directory_list);
            }
            break;

        case CMD_CALC_FILE_CRC32:
            _stop_timer();
            _call_crc32_result_callback(result, 0);
            break;

        default:
            _stop_timer();
            _call_op_result_callback(result);
            break;
    }
    _curr_op = CMD_NONE;
}

void MavlinkFtp::_call_op_result_callback(ServerResult result)
{
    if (_curr_op_result_callback) {
        const auto temp_callback = _curr_op_result_callback;
        _system_impl.call_user_callback(
            [temp_callback, result]() { temp_callback(_translate(result)); });
    }
}

void MavlinkFtp::_call_op_progress_callback(uint32_t bytes_read, uint32_t total_bytes)
{
    if (_curr_op_progress_callback) {
        // Slow callback down to only report ever 1%, otherwise we are slowing
        // everything down way too much.
        int percentage = 100 * bytes_read / total_bytes;
        if (_last_progress_percentage != percentage) {
            _last_progress_percentage = percentage;

            const auto temp_callback = _curr_op_progress_callback;
            _system_impl.call_user_callback([temp_callback, bytes_read, total_bytes]() {
                ProgressData progress;
                progress.bytes_transferred = bytes_read;
                progress.total_bytes = total_bytes;
                temp_callback(ClientResult::Next, progress);
            });
        }
    }
}

void MavlinkFtp::_call_dir_items_result_callback(ServerResult result, std::vector<std::string> list)
{
    if (_curr_dir_items_result_callback) {
        const auto temp_callback = _curr_dir_items_result_callback;
        _system_impl.call_user_callback(
            [temp_callback, result, list]() { temp_callback(_translate(result), list); });
    }
}

void MavlinkFtp::_call_crc32_result_callback(ServerResult result, uint32_t crc32)
{
    if (_current_crc32_result_callback) {
        const auto temp_callback = _current_crc32_result_callback;
        _system_impl.call_user_callback(
            [temp_callback, result, crc32]() { temp_callback(_translate(result), crc32); });
    }
}

MavlinkFtp::ClientResult MavlinkFtp::_translate(ServerResult result)
{
    switch (result) {
        case ServerResult::SUCCESS:
            return ClientResult::Success;
        case ServerResult::ERR_TIMEOUT:
            return ClientResult::Timeout;
        case ServerResult::ERR_FILE_IO_ERROR:
            return ClientResult::FileIoError;
        case ServerResult::ERR_FAIL_FILE_EXISTS:
            return ClientResult::FileExists;
        case ServerResult::ERR_FAIL_FILE_PROTECTED:
            return ClientResult::FileProtected;
        case ServerResult::ERR_UNKOWN_COMMAND:
            return ClientResult::Unsupported;
        case ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST:
            return ClientResult::FileDoesNotExist;
        default:
            return ClientResult::ProtocolError;
    }
}

void MavlinkFtp::reset_async(ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    if (_curr_op != CMD_NONE) {
        callback(ClientResult::Busy);
        return;
    }

    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = _session;
    payload.opcode = _curr_op = CMD_RESET_SESSIONS;
    payload.offset = 0;
    payload.size = 0;
    _curr_op_result_callback = callback;
    _send_mavlink_ftp_message(payload);
}

void MavlinkFtp::download_async(
    const std::string& remote_path, const std::string& local_folder, DownloadCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    if (_curr_op != CMD_NONE) {
        ProgressData empty{};
        callback(ClientResult::Busy, empty);
        return;
    }

    std::string local_path = local_folder + path_separator + fs_filename(remote_path);

    _ofstream.stream.open(local_path, std::fstream::trunc | std::fstream::binary);
    _ofstream.path = local_path;
    if (!_ofstream.stream) {
        _end_read_session();
        ProgressData empty{};
        callback(ClientResult::FileIoError, empty);
        return;
    }

    _curr_op_progress_callback = callback;
    _last_progress_percentage = -1;

    const auto result_callback = [callback](ClientResult result) {
        ProgressData empty{};
        callback(result, empty);
    };

    _generic_command_async(CMD_OPEN_FILE_RO, 0, remote_path, result_callback);
}

void MavlinkFtp::_end_read_session(bool delete_file)
{
    _curr_op = CMD_NONE;
    if (_ofstream.stream.is_open()) {
        _ofstream.stream.close();

        if (delete_file) {
            fs_remove(_ofstream.path);
        }
    }
    _terminate_session();
}

void MavlinkFtp::_read()
{
    if (_bytes_transferred >= _file_size) {
        _session_result = ServerResult::SUCCESS;
        _end_read_session();
        return;
    }

    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = _session;
    payload.opcode = _curr_op = CMD_READ_FILE;
    payload.offset = _bytes_transferred;
    payload.size =
        std::min(static_cast<uint32_t>(max_data_length), _file_size - _bytes_transferred);
    _send_mavlink_ftp_message(payload);
}

void MavlinkFtp::upload_async(
    const std::string& local_file_path, const std::string& remote_folder, UploadCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    if (_curr_op != CMD_NONE) {
        ProgressData empty{};
        callback(ClientResult::Busy, empty);
        return;
    }

    if (!fs_exists(local_file_path)) {
        ProgressData empty{};
        callback(ClientResult::FileDoesNotExist, empty);
        return;
    }

    _ifstream.open(local_file_path, std::fstream::binary);
    if (!_ifstream) {
        _end_write_session();
        ProgressData empty{};
        callback(ClientResult::FileIoError, empty);
        return;
    }

    _file_size = fs_file_size(local_file_path);
    _curr_op_progress_callback = callback;
    _last_progress_percentage = -1;

    std::string local_path(local_file_path);
    std::string remote_file_path = remote_folder + path_separator + fs_filename(local_path);

    const auto result_callback = [callback](ClientResult result) {
        ProgressData empty{};
        callback(result, empty);
    };

    _generic_command_async(CMD_OPEN_FILE_WO, 0, remote_file_path, result_callback);
}

void MavlinkFtp::_end_write_session()
{
    _curr_op = CMD_NONE;
    if (_ifstream) {
        _ifstream.close();
    }
    _terminate_session();
}

void MavlinkFtp::_write()
{
    if (_bytes_transferred >= _file_size) {
        _session_result = ServerResult::SUCCESS;
        _end_write_session();
        return;
    }

    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = _session;
    payload.opcode = _curr_op = CMD_WRITE_FILE;
    payload.offset = _bytes_transferred;
    int bytes_read = _ifstream.readsome(reinterpret_cast<char*>(payload.data), max_data_length);
    if (!_ifstream) {
        _end_write_session();
        _call_op_result_callback(ServerResult::ERR_FILE_IO_ERROR);
        return;
    }
    payload.size = bytes_read;
    _bytes_transferred += bytes_read;
    _send_mavlink_ftp_message(payload);
}

void MavlinkFtp::_terminate_session()
{
    if (!_session_valid) {
        return;
    }
    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = _session;
    payload.opcode = _curr_op = CMD_TERMINATE_SESSION;
    payload.offset = 0;
    payload.size = 0;
    _send_mavlink_ftp_message(payload);
}

std::pair<MavlinkFtp::ClientResult, std::vector<std::string>>
MavlinkFtp::list_directory(const std::string& path)
{
    std::promise<std::pair<ClientResult, std::vector<std::string>>> prom;
    auto fut = prom.get_future();

    list_directory_async(
        path, [&prom](const ClientResult result, const std::vector<std::string> dirs) {
            prom.set_value(std::make_pair(result, dirs));
        });

    return fut.get();
}

void MavlinkFtp::list_directory_async(
    const std::string& path, ListDirectoryCallback callback, uint32_t offset)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    if (_curr_op != CMD_NONE && offset == 0) {
        callback(ClientResult::Busy, std::vector<std::string>());
        return;
    }
    if (path.length() >= max_data_length) {
        callback(ClientResult::InvalidParameter, std::vector<std::string>());
        return;
    }

    _last_path = path;
    // TODOTODO
    _curr_dir_items_result_callback = callback;
    _list_directory(offset);
}

void MavlinkFtp::_list_directory(uint32_t offset)
{
    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = 0;
    payload.opcode = _curr_op = CMD_LIST_DIRECTORY;
    payload.offset = offset;
    strncpy(reinterpret_cast<char*>(payload.data), _last_path.c_str(), max_data_length - 1);
    payload.size = _last_path.length() + 1;

    if (offset == 0) {
        _curr_directory_list.clear();
    }
    _send_mavlink_ftp_message(payload);
}

void MavlinkFtp::_generic_command_async(
    Opcode opcode, uint32_t offset, const std::string& path, ResultCallback callback)
{
    if (_curr_op != CMD_NONE) {
        callback(ClientResult::Busy);
        return;
    }
    if (path.length() >= max_data_length) {
        callback(ClientResult::InvalidParameter);
        return;
    }

    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = 0;
    payload.opcode = _curr_op = opcode;
    payload.offset = offset;
    strncpy(reinterpret_cast<char*>(payload.data), path.c_str(), max_data_length - 1);
    payload.size = path.length() + 1;

    _curr_op_result_callback = callback;
    _send_mavlink_ftp_message(payload);
}

MavlinkFtp::ClientResult MavlinkFtp::create_directory(const std::string& path)
{
    std::promise<ClientResult> prom;
    auto fut = prom.get_future();

    create_directory_async(path, [&prom](const ClientResult result) { prom.set_value(result); });

    return fut.get();
}

void MavlinkFtp::create_directory_async(const std::string& path, ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    _generic_command_async(CMD_CREATE_DIRECTORY, 0, path, callback);
}

MavlinkFtp::ClientResult MavlinkFtp::remove_directory(const std::string& path)
{
    std::promise<ClientResult> prom;
    auto fut = prom.get_future();

    remove_directory_async(path, [&prom](const ClientResult result) { prom.set_value(result); });

    return fut.get();
}

void MavlinkFtp::remove_directory_async(const std::string& path, ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    _generic_command_async(CMD_REMOVE_DIRECTORY, 0, path, callback);
}

MavlinkFtp::ClientResult MavlinkFtp::remove_file(const std::string& path)
{
    std::promise<ClientResult> prom;
    auto fut = prom.get_future();

    remove_file_async(path, [&prom](const ClientResult result) { prom.set_value(result); });

    return fut.get();
}

void MavlinkFtp::remove_file_async(const std::string& path, ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    _generic_command_async(CMD_REMOVE_FILE, 0, path, callback);
}

MavlinkFtp::ClientResult
MavlinkFtp::rename(const std::string& from_path, const std::string& to_path)
{
    std::promise<ClientResult> prom;
    auto fut = prom.get_future();

    rename_async(
        from_path, to_path, [&prom](const ClientResult result) { prom.set_value(result); });

    return fut.get();
}

void MavlinkFtp::rename_async(
    const std::string& from_path, const std::string& to_path, ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    if (_curr_op != CMD_NONE) {
        callback(ClientResult::Busy);
        return;
    }
    if (from_path.length() + to_path.length() + 1 >= max_data_length) {
        callback(ClientResult::InvalidParameter);
        return;
    }

    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = 0;
    payload.opcode = _curr_op = CMD_RENAME;
    payload.offset = 0;
    strncpy(reinterpret_cast<char*>(payload.data), from_path.c_str(), max_data_length - 1);
    payload.size = from_path.length() + 1;
    strncpy(
        reinterpret_cast<char*>(&payload.data[payload.size]),
        to_path.c_str(),
        max_data_length - payload.size);
    payload.size += to_path.length() + 1;
    _curr_op_result_callback = callback;
    _send_mavlink_ftp_message(payload);
}

std::pair<MavlinkFtp::ClientResult, bool>
MavlinkFtp::are_files_identical(const std::string& local_path, const std::string& remote_path)
{
    std::promise<std::pair<ClientResult, bool>> prom;
    auto fut = prom.get_future();

    are_files_identical_async(
        local_path, remote_path, [&prom](const ClientResult result, const bool are_identical) {
            prom.set_value(std::make_pair(result, are_identical));
        });

    return fut.get();
}

void MavlinkFtp::are_files_identical_async(
    const std::string& local_path,
    const std::string& remote_path,
    AreFilesIdenticalCallback callback)
{
    if (!callback) {
        return;
    }

    auto temp_callback = callback;

    uint32_t crc_local = 0;
    auto result_local = _calc_local_file_crc32(local_path, crc_local);
    if (result_local != ClientResult::Success) {
        _system_impl.call_user_callback(
            [temp_callback, result_local]() { temp_callback(result_local, false); });
        return;
    }

    _calc_file_crc32_async(
        remote_path,
        [this, crc_local, temp_callback](ClientResult result_remote, uint32_t crc_remote) {
            if (result_remote != ClientResult::Success) {
                _system_impl.call_user_callback(
                    [temp_callback, result_remote]() { temp_callback(result_remote, false); });
            } else {
                _system_impl.call_user_callback([temp_callback, crc_local, crc_remote]() {
                    temp_callback(ClientResult::Success, crc_local == crc_remote);
                });
            }
        });
}

void MavlinkFtp::_calc_file_crc32_async(const std::string& path, file_crc32_ResultCallback callback)
{
    std::lock_guard<std::mutex> lock(_curr_op_mutex);
    if (_curr_op != CMD_NONE) {
        callback(ClientResult::Busy, 0);
        return;
    }
    if (path.length() >= max_data_length) {
        callback(ClientResult::InvalidParameter, 0);
        return;
    }

    auto payload = PayloadHeader{};
    payload.seq_number = _seq_number++;
    payload.session = 0;
    payload.opcode = _curr_op = CMD_CALC_FILE_CRC32;
    payload.offset = 0;
    strncpy(reinterpret_cast<char*>(payload.data), path.c_str(), max_data_length - 1);
    payload.size = path.length() + 1;
    _current_crc32_result_callback = callback;
    _send_mavlink_ftp_message(payload);
}

void MavlinkFtp::_send_mavlink_ftp_message(const PayloadHeader& payload)
{
    mavlink_msg_file_transfer_protocol_pack(
        _system_impl.get_own_system_id(),
        _system_impl.get_own_component_id(),
        &_last_command,
        _network_id,
        _system_impl.get_system_id(),
        _get_target_component_id(),
        reinterpret_cast<const uint8_t*>(&payload));
    _system_impl.send_message(_last_command);

    _reset_timer();
    std::lock_guard<std::mutex> lock(_timer_mutex);
    if (!_last_command_timer_running) {
        _last_command_timer_running = true;
        _system_impl.register_timeout_handler(
            [this]() { _command_timeout(); },
            static_cast<double>(_last_command_timeout) / 1000.0,
            &_last_command_timeout_cookie);
    }
}

void MavlinkFtp::_command_timeout()
{
    if (_last_command_retries >= _max_last_command_retries) {
        LogErr() << "Response timeout " << _curr_op;
        {
            std::lock_guard<std::mutex> lock(_timer_mutex);
            _last_command_timer_running = false;
            _session_result = ServerResult::ERR_TIMEOUT;
            _session_valid = false;
        }
        _process_nak(ServerResult::ERR_TIMEOUT);
    } else {
        _last_command_retries++;
        LogWarn() << "Response timeout. Retry: " << _last_command_retries;
        _system_impl.send_message(_last_command);
        _system_impl.register_timeout_handler(
            [this]() { _command_timeout(); },
            static_cast<double>(_last_command_timeout) / 1000.0,
            &_last_command_timeout_cookie);
    }
}

void MavlinkFtp::_reset_timer()
{
    _system_impl.refresh_timeout_handler(_last_command_timeout_cookie);
    _last_command_retries = 0;
}

void MavlinkFtp::_stop_timer()
{
    {
        std::lock_guard<std::mutex> lock(_timer_mutex);
        if (!_last_command_timer_running) {
            return;
        }
        _last_command_timer_running = false;
    }
    _system_impl.unregister_timeout_handler(_last_command_timeout_cookie);
}

/// @brief Guarantees that the payload data is null terminated.
/// @return Returns payload data as a std string
std::string MavlinkFtp::_data_as_string(PayloadHeader* payload)
{
    // guarantee null termination
    if (payload->size < max_data_length) {
        payload->data[payload->size] = '\0';

    } else {
        payload->data[max_data_length - 1] = '\0';
    }

    // and return data
    return std::string(reinterpret_cast<char*>(&(payload->data[0])));
}

std::string MavlinkFtp::_get_path(PayloadHeader* payload)
{
    return _get_path(_data_as_string(payload));
}

MavlinkFtp::ClientResult MavlinkFtp::set_root_directory(const std::string& root_dir)
{
    _root_dir = fs_canonical(root_dir);
    return ClientResult::Success;
}

std::string MavlinkFtp::_get_path(const std::string& payload_path)
{
    return fs_canonical(_root_dir + path_separator + payload_path);
}

std::string MavlinkFtp::_get_rel_path(const std::string& path)
{
    return path.substr(_root_dir.length());
}

MavlinkFtp::ServerResult MavlinkFtp::_work_list(PayloadHeader* payload, bool list_hidden)
{
    ServerResult error_code = ServerResult::SUCCESS;

    uint8_t offset = 0;
    // move to the requested offset
    uint32_t requested_offset = payload->offset;

    std::string path = _get_path(payload);
    if (path.rfind(_root_dir, 0) != 0) {
        LogWarn() << "FTP: invalid path " << path;
        return ServerResult::ERR_FAIL;
    }
    if (!fs_exists(path)) {
        LogWarn() << "FTP: can't open path " << path;
        // this is not an FTP error, abort directory by simulating eof
        return ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
    }

    struct dirent* dp;
    DIR* dfd = opendir(path.c_str());
    if (dfd != nullptr) {
        while ((dp = readdir(dfd)) != nullptr) {
            if (requested_offset > 0) {
                requested_offset--;
                continue;
            }

            std::string filename = dp->d_name;
            std::string full_path = path + path_separator + filename;

            std::string entry_s = DIRENT_SKIP;
            if (list_hidden || filename.rfind(".", 0) != 0) {
#ifdef _DIRENT_HAVE_D_TYPE
                bool type_reg = (dp->d_type == DT_REG);
                bool type_dir = (dp->d_type == DT_DIR);
#else
                struct stat statbuf;
                stat(full_path.c_str(), &statbuf);
                bool type_reg = S_ISREG(statbuf.st_mode);
                bool type_dir = S_ISDIR(statbuf.st_mode);
#endif
                if (type_reg) {
                    entry_s = DIRENT_FILE + _get_rel_path(full_path) + "\t" +
                              std::to_string(fs_file_size(full_path));
                } else if (type_dir) {
                    entry_s = DIRENT_DIR + _get_rel_path(full_path);
                }
            }

            // Do we have room for the dir entry and the null terminator?
            if (offset + entry_s.length() + 1 > max_data_length) {
                break;
            }
            uint8_t len = static_cast<uint8_t>(entry_s.length() + 1);
            memcpy(&payload->data[offset], entry_s.c_str(), len);
            offset += len;
        }
    }

    payload->size = offset;

    return error_code;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_open(PayloadHeader* payload, int oflag)
{
    if (_session_info.fd >= 0) {
        return ServerResult::ERR_NO_SESSIONS_AVAILABLE;
    }

    std::string path = [payload, this]() {
        std::lock_guard<std::mutex> lock(_tmp_files_mutex);
        const auto it = _tmp_files.find(_data_as_string(payload));
        if (it != _tmp_files.end()) {
            return it->second;
        } else {
            return _get_path(payload);
        }
    }();

    if (path.empty()) {
        return ServerResult::ERR_FAIL;
    }

    // TODO: check again
    // if (path.rfind(_root_dir, 0) != 0) {
    //    LogWarn() << "FTP: invalid path " << path;
    //    return ServerResult::ERR_FAIL;
    // }

    // fail only if requested open for read
    if ((oflag & O_ACCMODE) == O_RDONLY && !fs_exists(path)) {
        LogWarn() << "FTP: Open failed - file not found";
        return ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
    }

    uint32_t file_size = fs_file_size(path);

    // Set mode to 666 in case oflag has O_CREAT
    int fd = ::open(path.c_str(), oflag, 0666);

    if (fd < 0) {
        LogWarn() << "FTP: Open failed";
        return (errno == ENOENT) ? ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST :
                                   ServerResult::ERR_FAIL;
    }

    _session_info.fd = fd;
    _session_info.file_size = file_size;
    _session_info.stream_download = false;

    payload->session = 0;
    payload->size = sizeof(uint32_t);
    memcpy(payload->data, &file_size, payload->size);

    return ServerResult::SUCCESS;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_read(PayloadHeader* payload)
{
    if (payload->session != 0 || _session_info.fd < 0) {
        return ServerResult::ERR_INVALID_SESSION;
    }

    // We have to test seek past EOF ourselves, lseek will allow seek past EOF
    if (payload->offset >= _session_info.file_size) {
        return ServerResult::ERR_EOF;
    }

    if (lseek(_session_info.fd, payload->offset, SEEK_SET) < 0) {
        return ServerResult::ERR_FAIL;
    }

    auto bytes_read = ::read(_session_info.fd, &payload->data[0], max_data_length);

    if (bytes_read < 0) {
        // Negative return indicates error other than eof
        return ServerResult::ERR_FAIL;
    }

    payload->size = bytes_read;

    return ServerResult::SUCCESS;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_burst(PayloadHeader* payload)
{
    if (payload->session != 0 && _session_info.fd < 0) {
        return ServerResult::ERR_INVALID_SESSION;
    }

    // Setup for streaming sends
    _session_info.stream_download = true;
    _session_info.stream_offset = payload->offset;
    _session_info.stream_chunk_transmitted = 0;
    _session_info.stream_seq_number = payload->seq_number + 1;
    _session_info.stream_target_system_id = _system_impl.get_system_id();

    return ServerResult::SUCCESS;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_write(PayloadHeader* payload)
{
    if (payload->session != 0 && _session_info.fd < 0) {
        return ServerResult::ERR_INVALID_SESSION;
    }

    if (lseek(_session_info.fd, payload->offset, SEEK_SET) < 0) {
        // Unable to see to the specified location
        return ServerResult::ERR_FAIL;
    }

    int bytes_written = ::write(_session_info.fd, &payload->data[0], payload->size);

    if (bytes_written < 0) {
        // Negative return indicates error other than eof
        return ServerResult::ERR_FAIL;
    }

    payload->size = sizeof(uint32_t);
    memcpy(payload->data, &bytes_written, payload->size);

    return ServerResult::SUCCESS;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_terminate(PayloadHeader* payload)
{
    if (payload->session != 0 || _session_info.fd < 0) {
        return ServerResult::ERR_INVALID_SESSION;
    }

    close(_session_info.fd);
    _session_info.fd = -1;
    _session_info.stream_download = false;

    payload->size = 0;

    return ServerResult::SUCCESS;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_reset(PayloadHeader* payload)
{
    if (_session_info.fd != -1) {
        close(_session_info.fd);
        _session_info.fd = -1;
        _session_info.stream_download = false;
    }

    payload->size = 0;

    return ServerResult::SUCCESS;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_remove_directory(PayloadHeader* payload)
{
    std::string path = _get_path(payload);
    if (path.rfind(_root_dir, 0) != 0) {
        LogWarn() << "FTP: invalid path " << path;
        return ServerResult::ERR_FAIL;
    }

    if (!fs_exists(path)) {
        return ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
    }
    if (fs_remove(path)) {
        return ServerResult::SUCCESS;
    } else {
        return ServerResult::ERR_FAIL;
    }
}

MavlinkFtp::ServerResult MavlinkFtp::_work_create_directory(PayloadHeader* payload)
{
    std::string path = _get_path(payload);
    if (path.rfind(_root_dir, 0) != 0) {
        LogWarn() << "FTP: invalid path " << path;
        return ServerResult::ERR_FAIL;
    }

    if (fs_exists(path)) {
        return ServerResult::ERR_FAIL_FILE_EXISTS;
    }
    if (fs_create_directory(path)) {
        return ServerResult::SUCCESS;
    } else {
        return ServerResult::ERR_FAIL_ERRNO;
    }
}

MavlinkFtp::ServerResult MavlinkFtp::_work_remove_file(PayloadHeader* payload)
{
    std::string path = _get_path(payload);
    if (path.rfind(_root_dir, 0) != 0) {
        LogWarn() << "FTP: invalid path " << path;
        return ServerResult::ERR_FAIL;
    }

    if (!fs_exists(path)) {
        return ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
    }
    if (fs_remove(path)) {
        return ServerResult::SUCCESS;
    } else {
        return ServerResult::ERR_FAIL;
    }
}

MavlinkFtp::ServerResult MavlinkFtp::_work_rename(PayloadHeader* payload)
{
    size_t term_i = payload->size;
    if (payload->size >= max_data_length) {
        term_i = max_data_length - 1;
    }
    payload->data[term_i] = '\0';

    std::string old_name = std::string(reinterpret_cast<char*>(&(payload->data[0])));
    std::string new_name =
        _get_path(std::string(reinterpret_cast<char*>(&(payload->data[old_name.length() + 1]))));
    old_name = _get_path(old_name);
    if (old_name.rfind(_root_dir, 0) != 0 || new_name.rfind(_root_dir, 0) != 0) {
        return ServerResult::ERR_FAIL;
    }

    if (!fs_exists(old_name)) {
        return ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
    }

    if (fs_rename(old_name, new_name)) {
        return ServerResult::SUCCESS;
    } else {
        return ServerResult::ERR_FAIL;
    }
}

MavlinkFtp::ClientResult MavlinkFtp::_calc_local_file_crc32(const std::string& path, uint32_t& csum)
{
    if (!fs_exists(path)) {
        return ClientResult::FileDoesNotExist;
    }

    int fd = ::open(path.c_str(), O_RDONLY);
    if (fd < 0) {
        return ClientResult::FileIoError;
    }

    // Read whole file in buffer size chunks
    Crc32 checksum;
    char buffer[18392];
    ssize_t bytes_read;
    do {
        bytes_read = ::read(fd, buffer, sizeof(buffer));

        if (bytes_read < 0) {
            int r_errno = errno;
            close(fd);
            errno = r_errno;
            return ClientResult::FileIoError;
        }

        checksum.add((uint8_t*)buffer, bytes_read);
    } while (bytes_read == sizeof(buffer));

    close(fd);

    csum = checksum.get();

    return ClientResult::Success;
}

MavlinkFtp::ServerResult MavlinkFtp::_work_calc_file_CRC32(PayloadHeader* payload)
{
    std::string path = _get_path(payload);
    if (path.rfind(_root_dir, 0) != 0) {
        LogWarn() << "FTP: invalid path " << path;
        return ServerResult::ERR_FAIL;
    }

    if (!fs_exists(path)) {
        return ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
    }

    payload->size = sizeof(uint32_t);
    uint32_t checksum;
    ClientResult res = _calc_local_file_crc32(path, checksum);
    if (res != ClientResult::Success) {
        return ServerResult::ERR_FILE_IO_ERROR;
    }
    *reinterpret_cast<uint32_t*>(payload->data) = checksum;

    return ServerResult::SUCCESS;
}

void MavlinkFtp::send()
{
    // Anything to stream?
    if (!_session_info.stream_download) {
        return;
    }
}

uint8_t MavlinkFtp::get_our_compid()
{
    return _system_impl.get_own_component_id();
}

uint8_t MavlinkFtp::_get_target_component_id()
{
    return _target_component_id_set ? _target_component_id : _system_impl.get_autopilot_id();
}

MavlinkFtp::ClientResult MavlinkFtp::set_target_compid(uint8_t component_id)
{
    _target_component_id = component_id;
    _target_component_id_set = true;
    return ClientResult::Success;
}

std::optional<std::string>
MavlinkFtp::write_tmp_file(const std::string& path, const std::string& content)
{
    // TODO: Check if currently an operation is ongoing.

    if (path.find("..") != std::string::npos) {
        LogWarn() << "Path with .. not supported.";
        return {};
    }

    if (path.find('/') != std::string::npos) {
        LogWarn() << "Path with / not supported.";
        return {};
    }

    if (path.find('\\') != std::string::npos) {
        LogWarn() << "Path with \\ not supported.";
        return {};
    }

    // We use a temporary directory to put these
    if (_tmp_dir.empty()) {
        auto maybe_tmp_dir = create_tmp_directory("mavsdk-mavlink-ftp-tmp-files");
        if (maybe_tmp_dir) {
            _tmp_dir = maybe_tmp_dir.value();
        }
        // If we can't get a tmp dir, we'll just try to use our current working dir,
        // or whatever is the root dir by default.
    }

    const auto file_path = _tmp_dir + path_separator + path;
    std::ofstream out(file_path);
    out << content;
    out.flush();
    if (out.bad()) {
        LogWarn() << "Writing to " << file_path << " failed";
        out.close();
        return {};
    } else {
        out.close();

        std::lock_guard<std::mutex> lock(_tmp_files_mutex);
        _tmp_files[path] = file_path;

        return {file_path};
    }
}

std::ostream& operator<<(std::ostream& str, MavlinkFtp::ClientResult const& result)
{
    switch (result) {
        default:
            // Fallthrough
        case MavlinkFtp::ClientResult::Unknown:
            return str << "Unknown";
        case MavlinkFtp::ClientResult::Success:
            return str << "Success";
        case MavlinkFtp::ClientResult::Next:
            return str << "Next";
        case MavlinkFtp::ClientResult::Timeout:
            return str << "Timeout";
        case MavlinkFtp::ClientResult::Busy:
            return str << "Busy";
        case MavlinkFtp::ClientResult::FileIoError:
            return str << "FileIoError";
        case MavlinkFtp::ClientResult::FileExists:
            return str << "FileExists";
        case MavlinkFtp::ClientResult::FileDoesNotExist:
            return str << "FileDoesNotExist";
        case MavlinkFtp::ClientResult::FileProtected:
            return str << "FileProtected";
        case MavlinkFtp::ClientResult::InvalidParameter:
            return str << "InvalidParameter";
        case MavlinkFtp::ClientResult::Unsupported:
            return str << "Unsupported";
        case MavlinkFtp::ClientResult::ProtocolError:
            return str << "ProtocolError";
        case MavlinkFtp::ClientResult::NoSystem:
            return str << "NoSystem";
    }
}

} // namespace mavsdk
