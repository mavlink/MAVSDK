#include "mavlink_ftp_client.hpp"
#include "system_impl.hpp"
#include "overloaded.hpp"
#include "unused.hpp"
#include <algorithm>
#include <charconv>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <future>
#include <numeric>

#include "crc32.hpp"
#include "fs_utils.hpp"

namespace mavsdk {

namespace fs = std::filesystem;

MavlinkFtpClient::MavlinkFtpClient(SystemImpl& system_impl) :
    _system_impl(system_impl),
    _io_context(system_impl.io_context())
{
    if (const char* env_p = std::getenv("MAVSDK_FTP_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug("Ftp debugging is on.");
            _debugging = true;
        }
    }

    _system_impl.register_mavlink_message_handler(
        MAVLINK_MSG_ID_FILE_TRANSFER_PROTOCOL,
        [this](const mavlink_message_t& message) { process_mavlink_ftp_message(message); },
        this);
}

MavlinkFtpClient::~MavlinkFtpClient()
{
    // Blocking variants, so that neither a timeout nor a message can be dispatched into us
    // while we are being destroyed.
    _system_impl.unregister_timeout_handler_blocking(_timeout_cookie);
    _system_impl.unregister_all_mavlink_message_handlers_blocking(this);
}

void MavlinkFtpClient::do_work()
{
    if (_work_queue.empty()) {
        return;
    }
    auto work = _work_queue.front();

    if (work->started) {
        return;
    }
    work->started = true;

    // We're mainly starting the process here. After that, it continues
    // based on returned acks or timeouts.

    std::visit(
        overloaded{
            [&](DownloadItem& item) {
                if (!download_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](DownloadBurstItem& item) {
                if (!download_burst_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](UploadItem& item) {
                if (!upload_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](RemoveItem& item) {
                if (!remove_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](RenameItem& item) {
                if (!rename_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](CreateDirItem& item) {
                if (!create_dir_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](RemoveDirItem& item) {
                if (!remove_dir_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](CompareFilesItem& item) {
                if (!compare_files_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](ListDirItem& item) {
                if (!list_dir_start(*work, item)) {
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            }},
        work->item);
}

void MavlinkFtpClient::process_mavlink_ftp_message(const mavlink_message_t& msg)
{
    mavlink_file_transfer_protocol_t ftp_req;
    mavlink_msg_file_transfer_protocol_decode(&msg, &ftp_req);

    if (ftp_req.target_system != 0 && ftp_req.target_system != _system_impl.get_own_system_id()) {
        if (_debugging) {
            LogDebug("Received FTP message with wrong target system ID");
        }
        return;
    }

    if (ftp_req.target_component != 0 &&
        ftp_req.target_component != _system_impl.get_own_component_id()) {
        if (_debugging) {
            LogDebug("Received FTP message with wrong target component ID");
        }
        return;
    }

    PayloadHeader* payload = reinterpret_cast<PayloadHeader*>(&ftp_req.payload[0]);

    if (payload->size > max_data_length) {
        LogWarn("Received FTP payload with invalid size");
        return;
    } else {
        if (_debugging) {
            LogDebug(
                "FTP: opcode: {}, req_opcode: {}, size: {}, offset: {}, seq: {} from: {}/{}",
                (int)payload->opcode,
                (int)payload->req_opcode,
                (int)payload->size,
                (int)payload->offset,
                (int)payload->seq_number,
                (int)msg.sysid,
                (int)msg.compid);
        }
    }

    if (_work_queue.empty()) {
        return;
    }
    auto work = _work_queue.front();

    // A burst download takes its data wherever it comes from: burst packets of a burst we
    // have stopped following and replies to reads we have already timed out on still carry
    // bytes we need, and every one of them says where in the file it belongs. Dropping
    // them here used to mean re-requesting exactly the data that was arriving.
    const bool is_burst_download_data =
        std::holds_alternative<DownloadBurstItem>(work->item) && payload->opcode == RSP_ACK &&
        (payload->req_opcode == CMD_BURST_READ_FILE || payload->req_opcode == CMD_READ_FILE);

    if (work->last_opcode != payload->req_opcode && !is_burst_download_data) {
        // Ignore, but the other side is clearly still talking to us, so don't
        // keep backing off the retries: a burst that we have stopped following
        // still arrives while we ask for the parts we missed, and those
        // requests need to go out at the normal rate to get us back in step.
        note_link_alive();
        LogWarn("Ignore: last: {}, req: {}", (int)work->last_opcode, (int)payload->req_opcode);
        return;
    }
    // For non-burst transfers, strictly check the expected seq_number.
    // work->payload.seq_number is the seq_number of the most-recently sent
    // request; the server echoes back seq_number + 1.  Accepting any other
    // value would allow stale/duplicate UDP datagrams from a previous request
    // to corrupt the transfer (e.g. writing a full 239-byte chunk when only
    // the final 49 bytes remain).
    //
    // For burst transfers we cannot use a strict check: the server sends
    // multiple packets per CMD_BURST_READ_FILE request, each with an
    // incrementing seq_number, and packets may arrive out of order or with
    // gaps that are filled by a subsequent re-request.  For burst we fall back
    // to the original duplicate-rejection approach.
    //
    // This has to key off the opcode of the response, not the type of the work
    // item: a burst download also issues plain CMD_READ_FILE reads to fill
    // gaps, and those are strict request/response.  Exempting them from the
    // seq check lets a late reply from a timed-out request through, where it
    // no longer matches missing_data.front() and kills the whole transfer with
    // an offset mismatch.  On a high latency link that happens routinely.
    const bool is_burst = (payload->req_opcode == CMD_BURST_READ_FILE) || is_burst_download_data;
    if (!is_burst) {
        const auto expected_seq = static_cast<uint16_t>(work->payload.seq_number + 1);
        if (payload->seq_number != expected_seq) {
            note_link_alive();
            LogWarn(
                "Unexpected seq: got {}, expected {}", (int)payload->seq_number, (int)expected_seq);
            return;
        }
    } else {
        if (work->last_received_seq_number != 0 &&
            work->last_received_seq_number == payload->seq_number) {
            note_link_alive();
            LogWarn("Already seen seq: {}", (int)payload->seq_number);
            return;
        }
    }

    if (payload->opcode == RSP_NAK && payload->req_opcode == CMD_TERMINATE_SESSION &&
        static_cast<ServerResult>(payload->data[0]) == ERR_INVALID_SESSION) {
        // The session we are closing is gone already, which is what we were asking
        // for. A server drops a session after a while without requests, so a transfer
        // that rode out a long outage arrives here with all of its data and would
        // otherwise be reported as a protocol error at the very last step.
        LogDebug("Session was closed by the server already");
        payload->opcode = RSP_ACK;
        payload->size = 0;
    }

    std::visit(
        overloaded{
            [&](DownloadItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_OPEN_FILE_RO ||
                        payload->req_opcode == CMD_READ_FILE) {
                        // Whenever we do get an ack, the transfer is alive.
                        work->note_progress();
                        _retry_timeout_s.reset();

                        if (!download_continue(*work, item, payload)) {
                            stop_timer();
                            _work_queue.pop_front();
                            if (!_work_queue.empty()) {
                                asio::post(_io_context, [this] { do_work(); });
                            }
                        }
                    } else if (payload->req_opcode == CMD_TERMINATE_SESSION) {
                        stop_timer();
                        item.ofstream.close();
                        item.callback(ClientResult::Success, {});
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    stop_timer();
                    item.callback(result_from_nak(payload), {});
                    terminate_session(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](DownloadBurstItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_OPEN_FILE_RO ||
                        payload->req_opcode == CMD_BURST_READ_FILE ||
                        payload->req_opcode == CMD_READ_FILE) {
                        // Whenever we do get an ack, the transfer is alive.
                        work->note_progress();
                        _retry_timeout_s.reset();

                        if (!download_burst_continue(*work, item, payload)) {
                            stop_timer();
                            _work_queue.pop_front();
                            if (!_work_queue.empty()) {
                                asio::post(_io_context, [this] { do_work(); });
                            }
                        }
                    } else if (payload->req_opcode == CMD_TERMINATE_SESSION) {
                        stop_timer();
                        item.ofstream.close();
                        item.callback(ClientResult::Success, {});
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    const ServerResult sr = static_cast<ServerResult>(payload->data[0]);
                    // In case there's no session available, there's another transfer in progress
                    // for the given component. Back off and try again later.
                    if (sr == ERR_NO_SESSIONS_AVAILABLE) {
                        payload->seq_number = 0; // Ignore this response
                        start_timer(3.0);
                        LogDebug("No session available, retrying...");
                    } else if (sr == ERR_EOF && payload->req_opcode == CMD_BURST_READ_FILE) {
                        // The PX4 server ends the data of a burst with NAK(EOF) rather than
                        // with a packet that has burst_complete set, and answers a burst
                        // request at the end of the file the same way. Neither is a failure:
                        // it means this part has no more data.
                        if (_debugging) {
                            LogDebug("Burst ended with EOF at {}", item.current_offset);
                        }
                        if (!item.missing_data.empty() || item.current_offset == item.file_size) {
                            work->note_progress();
                            _retry_timeout_s.reset();
                            request_burst_next(*work, item);
                        } else {
                            // The server has no more data although we are not at the end of
                            // the file, which is what a burst whose last packets were lost
                            // looks like. Leave the retry to the timeout: it backs off and
                            // eventually gives up, where asking again from here would spin.
                            start_timer();
                        }
                    } else {
                        LogWarn(
                            "FTP: NAK received: server result {} for opcode {}",
                            static_cast<int>(sr),
                            static_cast<int>(payload->req_opcode));
                        stop_timer();
                        item.callback(result_from_nak(payload), {});
                        terminate_session(*work);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }
                    }
                }
            },
            [&](UploadItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_CREATE_FILE ||
                        payload->req_opcode == CMD_OPEN_FILE_WO ||
                        payload->req_opcode == CMD_WRITE_FILE) {
                        // Whenever we do get an ack, the transfer is alive.
                        work->note_progress();
                        _retry_timeout_s.reset();

                        if (!upload_continue(*work, item)) {
                            stop_timer();
                            _work_queue.pop_front();
                            if (!_work_queue.empty()) {
                                asio::post(_io_context, [this] { do_work(); });
                            }
                        }
                    } else if (payload->req_opcode == CMD_TERMINATE_SESSION) {
                        stop_timer();
                        item.ifstream.close();
                        item.callback(ClientResult::Success, {});
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    stop_timer();
                    item.callback(result_from_nak(payload), {});
                    terminate_session(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](RemoveItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_REMOVE_FILE) {
                        stop_timer();
                        item.callback(ClientResult::Success);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    stop_timer();
                    item.callback(result_from_nak(payload));
                    terminate_session(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](RenameItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_RENAME) {
                        stop_timer();
                        item.callback(ClientResult::Success);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    stop_timer();
                    item.callback(result_from_nak(payload));
                    terminate_session(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](CreateDirItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_CREATE_DIRECTORY) {
                        stop_timer();
                        item.callback(ClientResult::Success);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    stop_timer();
                    item.callback(result_from_nak(payload));
                    terminate_session(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](RemoveDirItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_REMOVE_DIRECTORY) {
                        stop_timer();
                        item.callback(ClientResult::Success);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    stop_timer();
                    item.callback(result_from_nak(payload));
                    terminate_session(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](CompareFilesItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_CALC_FILE_CRC32) {
                        stop_timer();
                        uint32_t remote_crc = *reinterpret_cast<uint32_t*>(payload->data);
                        item.callback(ClientResult::Success, remote_crc == item.local_crc);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }

                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    stop_timer();
                    item.callback(result_from_nak(payload), false);
                    terminate_session(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                }
            },
            [&](ListDirItem& item) {
                if (payload->opcode == RSP_ACK) {
                    if (payload->req_opcode == CMD_LIST_DIRECTORY ||
                        payload->req_opcode == CMD_LIST_DIRECTORY_WITH_TIME) {
                        // Whenever we do get an ack, the transfer is alive.
                        work->note_progress();
                        _retry_timeout_s.reset();

                        if (!list_dir_continue(*work, item, payload)) {
                            stop_timer();
                            _work_queue.pop_front();
                            if (!_work_queue.empty()) {
                                asio::post(_io_context, [this] { do_work(); });
                            }
                        }
                    } else {
                        LogWarn("Unexpected ack");
                    }

                } else if (payload->opcode == RSP_NAK) {
                    const ServerResult sr = static_cast<ServerResult>(payload->data[0]);

                    if (sr == ERR_EOF) {
                        // Completion is indicated by a NAK with EOF.
                        stop_timer();
                        list_dir_finish(item);
                        terminate_session(*work);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }
                    } else if (sr == ERR_UNKOWN_COMMAND && item.with_time) {
                        // The server does not support CMD_LIST_DIRECTORY_WITH_TIME, fall back
                        // to CMD_LIST_DIRECTORY and start over.
                        stop_timer();
                        if (_debugging) {
                            LogDebug("ListDirectoryWithTime unsupported, falling back to "
                                     "ListDirectory");
                        }
                        item.with_time = false;
                        item.offset = 0;
                        item.entries.clear();
                        if (!list_dir_start(*work, item)) {
                            _work_queue.pop_front();
                            if (!_work_queue.empty()) {
                                asio::post(_io_context, [this] { do_work(); });
                            }
                        }
                    } else {
                        stop_timer();
                        item.callback(result_from_nak(payload), {});
                        terminate_session(*work);
                        _work_queue.pop_front();
                        if (!_work_queue.empty()) {
                            asio::post(_io_context, [this] { do_work(); });
                        }
                    }
                }
            }},
        work->item);

    // Track the last received seq for burst duplicate detection.
    if (is_burst) {
        work->last_received_seq_number = payload->seq_number;
    }
}

bool MavlinkFtpClient::download_start(Work& work, DownloadItem& item)
{
    fs::path local_path = fs::path(item.local_folder) / utf8_path(item.remote_path).filename();
    fs::create_directories(fs::path(item.local_folder));

    if (_debugging) {
        LogDebug("Trying to open write to local path: {}", utf8_string(local_path));
    }

    item.ofstream.open(local_path, std::fstream::trunc | std::fstream::binary);
    if (!item.ofstream) {
        LogErr("Could not open it!");
        item.callback(ClientResult::FileIoError, {});
        return false;
    }

    work.last_opcode = CMD_OPEN_FILE_RO;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(
        reinterpret_cast<char*>(work.payload.data), item.remote_path.c_str(), max_data_length - 1);
    work.payload.size = item.remote_path.length() + 1;

    start_timer();
    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::download_continue(Work& work, DownloadItem& item, PayloadHeader* payload)
{
    if (payload->req_opcode == CMD_OPEN_FILE_RO) {
        item.file_size = *(reinterpret_cast<uint32_t*>(payload->data));

        if (_debugging) {
            LogWarn("Download continue, got file size: {}", item.file_size);
        }

    } else if (payload->req_opcode == CMD_READ_FILE) {
        if (_debugging) {
            LogWarn("Download continue, write: {}", payload->size);
        }

        if (item.bytes_transferred < item.file_size) {
            item.ofstream.write(reinterpret_cast<const char*>(payload->data), payload->size);
            if (!item.ofstream) {
                item.callback(ClientResult::FileIoError, {});
                return false;
            }
            item.bytes_transferred += payload->size;

            if (_debugging) {
                LogDebug("Written {} of {} bytes", item.bytes_transferred, item.file_size);
            }
        }
        item.callback(
            ClientResult::Next,
            ProgressData{
                static_cast<uint32_t>(item.bytes_transferred),
                static_cast<uint32_t>(item.file_size)});
    }

    if (item.bytes_transferred < item.file_size) {
        work.last_opcode = CMD_READ_FILE;
        work.payload = {};
        work.payload.seq_number = _last_sent_seq_number++;
        work.payload.session = _session;
        work.payload.opcode = work.last_opcode;
        work.payload.offset = item.bytes_transferred;

        work.payload.size =
            std::min(static_cast<size_t>(max_data_length), item.file_size - item.bytes_transferred);

        if (_debugging) {
            LogWarn(
                "Request size: {} of left {}",
                work.payload.size,
                int(item.file_size - item.bytes_transferred));
        }

        start_timer();
        send_mavlink_ftp_message(work.payload, work.target_compid);

        return true;
    } else {
        if (_debugging) {
            LogDebug("All bytes written, terminating sessio");
        }

        start_timer();
        terminate_session(work);
        return true;
    }

    return true;
}

bool MavlinkFtpClient::download_burst_start(Work& work, DownloadBurstItem& item)
{
    fs::path local_path = fs::path(item.local_folder) / utf8_path(item.remote_path).filename();

    if (_debugging) {
        LogDebug("Trying to open write to local path: {}", utf8_string(local_path));
    }

    item.ofstream.open(local_path, std::fstream::trunc | std::fstream::binary);
    if (!item.ofstream) {
        LogErr("Could not open it!");
        item.callback(ClientResult::FileIoError, {});
        return false;
    }

    work.last_opcode = CMD_OPEN_FILE_RO;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(
        reinterpret_cast<char*>(work.payload.data), item.remote_path.c_str(), max_data_length - 1);
    work.payload.size = item.remote_path.length() + 1;

    start_timer();
    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::download_burst_continue(
    Work& work, DownloadBurstItem& item, PayloadHeader* payload)
{
    if (payload->req_opcode == CMD_OPEN_FILE_RO) {
        std::memcpy(&(item.file_size), payload->data, sizeof(uint32_t));

        if (_debugging) {
            LogDebug("Burst Download continue, got file size: {}", item.file_size);
        }

        request_burst(work, item, item.current_offset);
        return true;
    }

    if (payload->req_opcode != CMD_BURST_READ_FILE && payload->req_opcode != CMD_READ_FILE) {
        LogErr("Unexpected req_opcode");
        download_burst_end(work);
        return false;
    }

    // Data arrives for requests we have moved on from: packets of a burst we stopped
    // following, or the reply to a read that we have already given up on. We take all of
    // it, because it says where it belongs, but only the answer to the request we are
    // actually waiting for may decide what to ask for next.
    const bool drives_next =
        work.last_opcode == payload->req_opcode &&
        (payload->req_opcode == CMD_BURST_READ_FILE ||
         payload->seq_number == static_cast<uint16_t>(work.payload.seq_number + 1));

    if (work.last_opcode == CMD_OPEN_FILE_RO || item.file_size == 0) {
        // We have asked for the file but don't know its size yet, so this is data for
        // something else: a burst from a session that ended, for instance, which a
        // server keeps streaming for a while after a client goes away and comes back.
        LogWarn("Ignoring FTP data that arrived before the file is open");
        note_link_alive();
        return true;
    }

    if (payload->offset > item.file_size || payload->size > item.file_size - payload->offset) {
        // The server should never point us past the end of the file. Drop the packet
        // rather than zero-filling a gap of up to ~4 GB from a bad offset -- and rather
        // than failing the transfer, because a stale packet from an earlier session is
        // not this transfer's fault.
        LogWarn(
            "Ignoring FTP data at offset {} with size {}, past the file size {}",
            (uint32_t)payload->offset,
            (int)payload->size,
            item.file_size);
        note_link_alive();
        return true;
    }

    if (_debugging) {
        LogDebug(
            "Burst download continue, at: {} write: {}",
            (uint32_t)payload->offset,
            (int)payload->size);
    }

    if (!burst_absorb(item, payload->offset, payload->data, payload->size)) {
        item.callback(ClientResult::FileIoError, {});
        download_burst_end(work);
        return false;
    }

    if (item.missing_data.empty() && item.current_offset == item.file_size) {
        if (_debugging) {
            LogDebug("Burst download complete");
        }
        download_burst_end(work);
        return true;
    }

    item.callback(
        ClientResult::Next,
        ProgressData{static_cast<uint32_t>(burst_bytes_transferred(item)), item.file_size});

    if (!drives_next) {
        // Whatever we are waiting for is still outstanding, but the link is alive.
        start_timer();
        return true;
    }

    if (payload->req_opcode == CMD_BURST_READ_FILE && !payload->burst_complete &&
        item.missing_data.size() <= MAX_MISSING_RANGES) {
        // There is more of this part coming, just wait for now.
        start_timer();
        return true;
    }

    request_burst_next(work, item);

    return true;
}

bool MavlinkFtpClient::burst_absorb(
    DownloadBurstItem& item, size_t offset, const uint8_t* data, size_t size)
{
    if (size == 0) {
        return true;
    }

    if (offset > item.current_offset) {
        // We missed a part. Note it down and write zeros as a placeholder, so that the
        // file has the right length while we wait for the real bytes.
        item.missing_data.emplace_back(
            DownloadBurstItem::MissingData{item.current_offset, offset - item.current_offset});

        item.ofstream.seekp(item.current_offset);
        const std::vector<char> empty(offset - item.current_offset, 0);
        item.ofstream.write(empty.data(), empty.size());
        if (!item.ofstream) {
            LogWarn("Write failed");
            return false;
        }
    }

    item.ofstream.seekp(offset);
    if (item.ofstream.fail()) {
        LogWarn("Seek failed");
        return false;
    }

    item.ofstream.write(reinterpret_cast<const char*>(data), size);
    if (!item.ofstream) {
        LogWarn("Write failed");
        return false;
    }

    item.current_offset = std::max(item.current_offset, offset + size);
    burst_mark_received(item, offset, size);

    return true;
}

void MavlinkFtpClient::burst_mark_received(DownloadBurstItem& item, size_t offset, size_t size)
{
    if (item.missing_data.empty()) {
        return;
    }

    const size_t end = offset + size;

    std::deque<DownloadBurstItem::MissingData> remaining;
    for (const auto& missing : item.missing_data) {
        const size_t missing_end = missing.offset + missing.size;

        if (missing_end <= offset || missing.offset >= end) {
            remaining.push_back(missing);
            continue;
        }

        // What arrived can fill a hole from the front, from the back, or from the middle,
        // in which case the hole falls apart into two.
        if (missing.offset < offset) {
            remaining.push_back(
                DownloadBurstItem::MissingData{missing.offset, offset - missing.offset});
        }
        if (missing_end > end) {
            remaining.push_back(DownloadBurstItem::MissingData{end, missing_end - end});
        }
    }

    item.missing_data = std::move(remaining);
}

size_t MavlinkFtpClient::burst_next_needed_offset(const DownloadBurstItem& item)
{
    return item.missing_data.empty() ? item.current_offset : item.missing_data.front().offset;
}

void MavlinkFtpClient::request_burst_next(Work& work, DownloadBurstItem& item)
{
    if (item.missing_data.size() > MAX_MISSING_RANGES) {
        // Too many holes to keep track of, so stop this part here and have the rest of it
        // sent again from the first hole.
        request_burst(work, item, burst_next_needed_offset(item));
        return;
    }

    if (!item.missing_data.empty()) {
        // A part has ended, which means the server is idle: this is the moment to ask for
        // the holes, without a read racing a burst that is still running.
        request_next_rest(work, item);
        return;
    }

    if (item.current_offset < item.file_size) {
        request_burst(work, item, item.current_offset);
        return;
    }

    download_burst_end(work);
}

void MavlinkFtpClient::download_burst_end(Work& work)
{
    work.last_opcode = CMD_TERMINATE_SESSION;

    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;

    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    work.payload.size = 0;

    start_timer();
    send_mavlink_ftp_message(work.payload, work.target_compid);
}

void MavlinkFtpClient::request_burst(Work& work, DownloadBurstItem& item, size_t offset)
{
    UNUSED(item);

    if (_debugging) {
        LogDebug("Requesting burst from {}", offset);
    }

    work.last_opcode = CMD_BURST_READ_FILE;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = static_cast<uint32_t>(offset);

    // Fill up the whole packet.
    work.payload.size = max_data_length;

    start_timer();
    send_mavlink_ftp_message(work.payload, work.target_compid);
}

void MavlinkFtpClient::request_next_rest(Work& work, DownloadBurstItem& item)
{
    const auto& missing = item.missing_data.front();
    size_t size = std::min(missing.size, size_t(max_data_length));

    if (_debugging) {
        LogDebug("Re-requesting from {} with size {}", missing.offset, size);
    }

    work.last_opcode = CMD_READ_FILE;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = static_cast<uint32_t>(missing.offset);

    work.payload.size = static_cast<uint8_t>(size);

    start_timer();
    send_mavlink_ftp_message(work.payload, work.target_compid);
}

size_t MavlinkFtpClient::burst_bytes_transferred(DownloadBurstItem& item)
{
    return item.current_offset - std::accumulate(
                                     item.missing_data.begin(),
                                     item.missing_data.end(),
                                     size_t(0),
                                     [](size_t acc, const DownloadBurstItem::MissingData& missing) {
                                         return acc + missing.size;
                                     });
}

bool MavlinkFtpClient::upload_start(Work& work, UploadItem& item)
{
    std::error_code ec;
    const fs::path local_file_path = utf8_path(item.local_file_path);
    if (!fs::exists(local_file_path, ec)) {
        item.callback(ClientResult::FileDoesNotExist, {});
        return false;
    }

    item.ifstream.open(local_file_path, std::fstream::binary);
    if (!item.ifstream) {
        item.callback(ClientResult::FileIoError, {});
        return false;
    }

    item.file_size = fs::file_size(local_file_path, ec);
    if (ec) {
        LogWarn("Could not get file size of '{}': {}", item.local_file_path, ec.message());
        return false;
    }

    fs::path remote_file_path = utf8_path(item.remote_folder) / local_file_path.filename();
    const std::string remote_file_path_str = utf8_string(remote_file_path);

    if (remote_file_path_str.size() >= max_data_length) {
        item.callback(ClientResult::InvalidParameter, {});
        return false;
    }

    work.last_opcode = CMD_CREATE_FILE;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(
        reinterpret_cast<char*>(work.payload.data),
        remote_file_path_str.c_str(),
        max_data_length - 1);
    work.payload.size = remote_file_path_str.size() + 1;

    start_timer();
    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::upload_continue(Work& work, UploadItem& item)
{
    if (item.bytes_transferred < item.file_size) {
        work.last_opcode = CMD_WRITE_FILE;

        work.payload = {};
        work.payload.seq_number = _last_sent_seq_number++;
        work.payload.session = _session;

        work.payload.opcode = work.last_opcode;
        work.payload.offset = item.bytes_transferred;

        std::size_t bytes_to_read =
            std::min(item.file_size - item.bytes_transferred, std::size_t(max_data_length));

        item.ifstream.read(reinterpret_cast<char*>(work.payload.data), bytes_to_read);

        // Get the number of bytes actually read.
        int bytes_read = item.ifstream.gcount();

        if (!item.ifstream) {
            item.callback(ClientResult::FileIoError, {});
            return false;
        }

        work.payload.size = bytes_read;
        item.bytes_transferred += bytes_read;

        start_timer();
        send_mavlink_ftp_message(work.payload, work.target_compid);

    } else {
        // Final step
        work.last_opcode = CMD_TERMINATE_SESSION;

        work.payload = {};
        work.payload.seq_number = _last_sent_seq_number++;
        work.payload.session = _session;

        work.payload.opcode = work.last_opcode;
        work.payload.offset = 0;
        work.payload.size = 0;

        start_timer();
        send_mavlink_ftp_message(work.payload, work.target_compid);
    }

    item.callback(
        ClientResult::Next,
        ProgressData{
            static_cast<uint32_t>(item.bytes_transferred), static_cast<uint32_t>(item.file_size)});

    return true;
}

bool MavlinkFtpClient::remove_start(Work& work, RemoveItem& item)
{
    if (item.path.length() >= max_data_length) {
        item.callback(ClientResult::InvalidParameter);
        return false;
    }

    work.last_opcode = CMD_REMOVE_FILE;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(reinterpret_cast<char*>(work.payload.data), item.path.c_str(), max_data_length - 1);
    work.payload.size = item.path.length() + 1;

    start_timer();
    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::rename_start(Work& work, RenameItem& item)
{
    if (item.from_path.length() + item.to_path.length() + 1 >= max_data_length) {
        item.callback(ClientResult::InvalidParameter);
        return false;
    }

    work.last_opcode = CMD_RENAME;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(
        reinterpret_cast<char*>(work.payload.data), item.from_path.c_str(), max_data_length - 1);
    work.payload.size = item.from_path.length() + 1;
    strncpy(
        reinterpret_cast<char*>(&work.payload.data[work.payload.size]),
        item.to_path.c_str(),
        max_data_length - work.payload.size);
    work.payload.size += item.to_path.length() + 1;
    start_timer();

    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::create_dir_start(Work& work, CreateDirItem& item)
{
    if (item.path.length() + 1 >= max_data_length) {
        item.callback(ClientResult::InvalidParameter);
        return false;
    }

    work.last_opcode = CMD_CREATE_DIRECTORY;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(reinterpret_cast<char*>(work.payload.data), item.path.c_str(), max_data_length - 1);
    work.payload.size = item.path.length() + 1;
    start_timer();

    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::remove_dir_start(Work& work, RemoveDirItem& item)
{
    if (item.path.length() + 1 >= max_data_length) {
        item.callback(ClientResult::InvalidParameter);
        return false;
    }

    work.last_opcode = CMD_REMOVE_DIRECTORY;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(reinterpret_cast<char*>(work.payload.data), item.path.c_str(), max_data_length - 1);
    work.payload.size = item.path.length() + 1;
    start_timer();

    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::compare_files_start(Work& work, CompareFilesItem& item)
{
    if (item.remote_path.length() + 1 >= max_data_length) {
        item.callback(ClientResult::InvalidParameter, false);
        return false;
    }

    auto result_local = calc_local_file_crc32(item.local_path, item.local_crc);
    if (result_local != ClientResult::Success) {
        item.callback(result_local, false);
        return false;
    }

    work.last_opcode = CMD_CALC_FILE_CRC32;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    strncpy(
        reinterpret_cast<char*>(work.payload.data), item.remote_path.c_str(), max_data_length - 1);
    work.payload.size = item.remote_path.length() + 1;
    start_timer();

    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::list_dir_start(Work& work, ListDirItem& item)
{
    if (item.path.length() + 1 >= max_data_length) {
        item.callback(ClientResult::InvalidParameter, {});
        return false;
    }

    work.last_opcode = item.with_time ? CMD_LIST_DIRECTORY_WITH_TIME : CMD_LIST_DIRECTORY;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = item.offset;
    strncpy(reinterpret_cast<char*>(work.payload.data), item.path.c_str(), max_data_length - 1);
    work.payload.size = item.path.length() + 1;
    start_timer();

    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

bool MavlinkFtpClient::list_dir_continue(Work& work, ListDirItem& item, PayloadHeader* payload)
{
    if (_debugging) {
        LogDebug("List dir response received, got {} chars", (int)payload->size);
    }

    if (payload->size > max_data_length) {
        LogWarn("Received FTP payload with invalid size");
        return false;
    }

    if (payload->size == 0) {
        list_dir_finish(item);
        return false;
    }

    // Make sure there is a zero termination.
    payload->data[payload->size - 1] = '\0';

    const auto parse_u64 = [](const std::string& field) -> uint64_t {
        uint64_t value = 0;
        const auto* begin = field.data();
        const auto* end = field.data() + field.size();
        const auto [ptr, ec] = std::from_chars(begin, end, value);
        if (ec != std::errc()) {
            return 0;
        }
        return value;
    };

    size_t i = 0;
    while (i + 1 < payload->size) {
        const int entry_len = std::strlen(reinterpret_cast<char*>(&payload->data[i]));

        std::string entry;
        entry.resize(entry_len);
        std::memcpy(entry.data(), &payload->data[i], entry_len);

        i += entry_len + 1;

        ++item.offset;

        if (_debugging) {
            LogDebug("Raw entry from list_dir: '{}'", entry);
        }

        if (entry.empty()) {
            continue;
        }

        const char type_char = entry[0];

        if (type_char == 'S') {
            // Skip entry.
            continue;
        }

        if (type_char != 'F' && type_char != 'D') {
            LogErr("Unknown list_dir entry: {}", entry);
            continue;
        }

        // The entry body after the type character is tab-separated:
        // <name>[\t<size_bytes>[\t<modification_time_s>]]
        // Only CMD_LIST_DIRECTORY_WITH_TIME includes the modification time, and only files
        // include the size, so trailing fields may be absent.
        const std::string body = entry.substr(1);

        ListDirEntry list_entry{};
        list_entry.type = (type_char == 'D') ? ListDirEntry::Type::Dir : ListDirEntry::Type::File;

        size_t field_start = 0;
        int field_index = 0;
        while (field_start <= body.size()) {
            const auto tab = body.find('\t', field_start);
            const std::string field = (tab == std::string::npos) ?
                                          body.substr(field_start) :
                                          body.substr(field_start, tab - field_start);

            switch (field_index) {
                case 0:
                    list_entry.name = field;
                    break;
                case 1:
                    list_entry.size_bytes = parse_u64(field);
                    break;
                case 2:
                    list_entry.modification_time_s = parse_u64(field);
                    break;
                default:
                    break;
            }

            ++field_index;
            if (tab == std::string::npos) {
                break;
            }
            field_start = tab + 1;
        }

        item.entries.push_back(std::move(list_entry));
    }

    work.last_opcode = item.with_time ? CMD_LIST_DIRECTORY_WITH_TIME : CMD_LIST_DIRECTORY;
    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;
    work.payload.opcode = work.last_opcode;
    work.payload.offset = item.offset;
    strncpy(reinterpret_cast<char*>(work.payload.data), item.path.c_str(), max_data_length - 1);
    work.payload.size = item.path.length() + 1;
    start_timer();

    send_mavlink_ftp_message(work.payload, work.target_compid);

    return true;
}

void MavlinkFtpClient::list_dir_finish(ListDirItem& item)
{
    std::sort(
        item.entries.begin(), item.entries.end(), [](const ListDirEntry& a, const ListDirEntry& b) {
            return a.name < b.name;
        });
    item.callback(ClientResult::Success, item.entries);
}

MavlinkFtpClient::ClientResult MavlinkFtpClient::result_from_nak(PayloadHeader* payload)
{
    ServerResult sr = static_cast<ServerResult>(payload->data[0]);

    // PX4 Mavlink FTP returns "File doesn't exist" this way
    if (sr == ServerResult::ERR_FAIL_ERRNO && payload->data[1] == ENOENT) {
        sr = ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST;
    }

    return translate(sr);
}

MavlinkFtpClient::ClientResult MavlinkFtpClient::translate(ServerResult result)
{
    switch (result) {
        case ServerResult::SUCCESS:
            return ClientResult::Success;
        case ServerResult::ERR_FAIL:
            return ClientResult::ProtocolError;
        case ServerResult::ERR_FAIL_ERRNO:
            return ClientResult::ProtocolError;
        case ServerResult::ERR_INVALID_DATA_SIZE:
            return ClientResult::ProtocolError;
        case ServerResult::ERR_INVALID_SESSION:
            return ClientResult::ProtocolError;
        case ServerResult::ERR_NO_SESSIONS_AVAILABLE:
            return ClientResult::ProtocolError;
        case ServerResult::ERR_EOF:
            return ClientResult::ProtocolError;
        case ServerResult::ERR_UNKOWN_COMMAND:
            return ClientResult::Unsupported;
        case ServerResult::ERR_FAIL_FILE_EXISTS:
            return ClientResult::FileExists;
        case ServerResult::ERR_FAIL_FILE_PROTECTED:
            return ClientResult::FileProtected;
        case ServerResult::ERR_FAIL_FILE_DOES_NOT_EXIST:
            return ClientResult::FileDoesNotExist;
        case ServerResult::ERR_TIMEOUT:
            return ClientResult::Timeout;
        case ServerResult::ERR_FILE_IO_ERROR:
            return ClientResult::FileIoError;
        default:
            LogInfo("Unknown error code: {}", (int)result);
            return ClientResult::ProtocolError;
    }
}

void MavlinkFtpClient::enqueue_work(std::shared_ptr<Work> new_work)
{
    asio::post(_io_context, [this, new_work = std::move(new_work)]() {
        const bool was_empty = _work_queue.empty();
        _work_queue.push_back(new_work);
        if (was_empty) {
            do_work();
        }
    });
}

void MavlinkFtpClient::download_async(
    const std::string& remote_path,
    const std::string& local_folder,
    bool use_burst,
    DownloadCallback callback,
    std::optional<uint8_t> maybe_target_compid)
{
    if (use_burst) {
        auto item = DownloadBurstItem{};
        item.remote_path = remote_path;
        item.local_folder = local_folder;
        item.callback = callback;
        enqueue_work(std::make_shared<Work>(
            Work{std::move(item), maybe_target_compid.value_or(get_target_component_id())}));
    } else {
        auto item = DownloadItem{};
        item.remote_path = remote_path;
        item.local_folder = local_folder;
        item.callback = callback;
        enqueue_work(std::make_shared<Work>(
            Work{std::move(item), maybe_target_compid.value_or(get_target_component_id())}));
    }
}

void MavlinkFtpClient::upload_async(
    const std::string& local_file_path, const std::string& remote_folder, UploadCallback callback)
{
    auto item = UploadItem{};
    item.local_file_path = local_file_path;
    item.remote_folder = remote_folder;
    item.callback = callback;
    enqueue_work(std::make_shared<Work>(Work{std::move(item), get_target_component_id()}));
}

void MavlinkFtpClient::list_directory_async(const std::string& path, ListDirectoryCallback callback)
{
    auto item = ListDirItem{};
    item.path = path;
    item.callback = callback;
    enqueue_work(std::make_shared<Work>(Work{std::move(item), get_target_component_id()}));
}

void MavlinkFtpClient::create_directory_async(const std::string& path, ResultCallback callback)
{
    auto item = CreateDirItem{};
    item.path = path;
    item.callback = callback;
    enqueue_work(std::make_shared<Work>(Work{std::move(item), get_target_component_id()}));
}

void MavlinkFtpClient::remove_directory_async(const std::string& path, ResultCallback callback)
{
    auto item = RemoveDirItem{};
    item.path = path;
    item.callback = callback;
    enqueue_work(std::make_shared<Work>(Work{std::move(item), get_target_component_id()}));
}

void MavlinkFtpClient::remove_file_async(const std::string& path, ResultCallback callback)
{
    auto item = RemoveItem{};
    item.path = path;
    item.callback = callback;
    enqueue_work(std::make_shared<Work>(Work{std::move(item), get_target_component_id()}));
}

void MavlinkFtpClient::rename_async(
    const std::string& from_path, const std::string& to_path, ResultCallback callback)
{
    auto item = RenameItem{};
    item.from_path = from_path;
    item.to_path = to_path;
    item.callback = callback;
    enqueue_work(std::make_shared<Work>(Work{std::move(item), get_target_component_id()}));
}

void MavlinkFtpClient::are_files_identical_async(
    const std::string& local_path,
    const std::string& remote_path,
    AreFilesIdenticalCallback callback)
{
    auto item = CompareFilesItem{};
    item.local_path = local_path;
    item.remote_path = remote_path;
    item.callback = callback;
    enqueue_work(std::make_shared<Work>(Work{std::move(item), get_target_component_id()}));
}

void MavlinkFtpClient::send_mavlink_ftp_message(const PayloadHeader& payload, uint8_t target_compid)
{
    if (_debugging) {
        LogDebug(
            "FTP send: opcode: {}, seq: {} to: {}/{}",
            (int)payload.opcode,
            payload.seq_number,
            (int)_system_impl.get_system_id(),
            (int)target_compid);
    }
    _system_impl.queue_message([&](MavlinkAddress mavlink_address, uint8_t channel) {
        mavlink_message_t message;
        mavlink_msg_file_transfer_protocol_pack_chan(
            mavlink_address.system_id,
            mavlink_address.component_id,
            channel,
            &message,
            _network_id,
            _system_impl.get_system_id(),
            target_compid,
            reinterpret_cast<const uint8_t*>(&payload));
        return message;
    });
}

void MavlinkFtpClient::start_timer(std::optional<double> duration_s)
{
    _system_impl.unregister_timeout_handler(_timeout_cookie);
    _timeout_cookie = _system_impl.register_timeout_handler(
        [this]() { timeout(); },
        duration_s.value_or(_retry_timeout_s.value_or(_system_impl.timeout_s())));
}

double MavlinkFtpClient::retry_timeout_s(const Work& work) const
{
    // Back off while retries keep failing: on a half duplex radio link, retrying
    // every 500ms during a fade just spends airtime that the answer needs.
    const double base_timeout_s = _system_impl.timeout_s();
    const double max_timeout_s = MAX_RETRY_TIMEOUTS * base_timeout_s;
    double timeout_s = base_timeout_s;
    for (unsigned i = 1; i < work.consecutive_timeouts && timeout_s < max_timeout_s; ++i) {
        timeout_s *= 2.0;
    }
    return std::min(timeout_s, max_timeout_s);
}

double MavlinkFtpClient::no_progress_timeout_s() const
{
    return NO_PROGRESS_TIMEOUTS * _system_impl.timeout_s();
}

void MavlinkFtpClient::note_link_alive()
{
    // Something came back, so the link is not the problem: retry at the normal
    // rate again. The transfer has not moved on, so the give-up budget keeps
    // running.
    _retry_timeout_s.reset();
}

void MavlinkFtpClient::stop_timer()
{
    _system_impl.unregister_timeout_handler(_timeout_cookie);
}

void MavlinkFtpClient::timeout()
{
    if (_debugging) {
        LogDebug("Timeout!");
    }

    if (_work_queue.empty()) {
        return;
    }
    auto work = _work_queue.front();

    ++work->consecutive_timeouts;
    _retry_timeout_s = retry_timeout_s(*work);

    if (_debugging) {
        LogDebug(
            "Timeout number {}, {}s since progress",
            work->consecutive_timeouts,
            std::chrono::duration<double>(std::chrono::steady_clock::now() - work->last_progress)
                .count());
    }

    std::visit(
        overloaded{
            [&](DownloadItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    LogWarn(
                        "Download timed out after {} of {} bytes",
                        item.bytes_transferred,
                        item.file_size);
                    item.callback(
                        ClientResult::Timeout,
                        ProgressData{
                            static_cast<uint32_t>(item.bytes_transferred),
                            static_cast<uint32_t>(item.file_size)});
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            },
            [&](DownloadBurstItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    // current_offset is how far the burst got, so take off what we
                    // know is still missing from it.
                    const auto missing = std::accumulate(
                        item.missing_data.begin(),
                        item.missing_data.end(),
                        std::size_t{0},
                        [](std::size_t sum, const DownloadBurstItem::MissingData& data) {
                            return sum + data.size;
                        });
                    const auto received = static_cast<uint32_t>(
                        item.current_offset - std::min(item.current_offset, missing));
                    LogWarn(
                        "Burst download timed out after {} of {} bytes", received, item.file_size);
                    item.callback(ClientResult::Timeout, ProgressData{received, item.file_size});
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                if (work->last_opcode == CMD_OPEN_FILE_RO) {
                    // The file isn't even open yet, so there is nothing to be missing:
                    // ask again.
                    work->payload.seq_number = _last_sent_seq_number++;
                    start_timer();
                    send_mavlink_ftp_message(work->payload, work->target_compid);
                    return;
                }

                if (item.missing_data.empty() && item.current_offset == item.file_size) {
                    // Everything arrived, only the session teardown is outstanding and we
                    // don't need an answer for that.
                    item.ofstream.close();
                    item.callback(ClientResult::Success, {});
                    download_burst_end(*work);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }

                // A stalled burst is resumed as a burst, from the lowest offset we are
                // still missing. Asking for the rest of the file in 239 byte reads
                // instead takes a round trip per packet, and means throwing away the
                // burst packets that are still on their way.
                request_burst(*work, item, burst_next_needed_offset(item));
            },
            [&](UploadItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    LogWarn(
                        "Upload timed out after {} of {} bytes",
                        item.bytes_transferred,
                        item.file_size);
                    item.callback(
                        ClientResult::Timeout,
                        ProgressData{
                            static_cast<uint32_t>(item.bytes_transferred),
                            static_cast<uint32_t>(item.file_size)});
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            },
            [&](RemoveItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    item.callback(ClientResult::Timeout);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            },
            [&](RenameItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    item.callback(ClientResult::Timeout);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            },
            [&](CreateDirItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    item.callback(ClientResult::Timeout);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            },
            [&](RemoveDirItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    item.callback(ClientResult::Timeout);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            },
            [&](CompareFilesItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    item.callback(ClientResult::Timeout, false);
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            },
            [&](ListDirItem& item) {
                if (work->gave_up(no_progress_timeout_s())) {
                    item.callback(ClientResult::Timeout, {});
                    _work_queue.pop_front();
                    if (!_work_queue.empty()) {
                        asio::post(_io_context, [this] { do_work(); });
                    }
                    return;
                }
                if (_debugging) {
                    LogDebug("No answer, retrying in {}s", _retry_timeout_s.value_or(0.0));
                }

                work->payload.seq_number = _last_sent_seq_number++;
                start_timer();
                send_mavlink_ftp_message(work->payload, work->target_compid);
            }},
        work->item);
}

MavlinkFtpClient::ClientResult
MavlinkFtpClient::calc_local_file_crc32(const std::string& path, uint32_t& csum)
{
    std::error_code ec;
    const fs::path local_path = utf8_path(path);
    if (!fs::exists(local_path, ec)) {
        return ClientResult::FileDoesNotExist;
    }

    std::ifstream stream(local_path, std::fstream::binary);
    if (!stream) {
        return ClientResult::FileIoError;
    }

    // Read whole file in buffer size chunks
    Crc32 checksum;
    uint8_t buffer[4096];
    std::streamsize bytes_read;

    do {
        stream.read(reinterpret_cast<char*>(buffer), sizeof(buffer));
        bytes_read = stream.gcount(); // Get the number of bytes actually read
        checksum.add(reinterpret_cast<const uint8_t*>(buffer), bytes_read);
    } while (bytes_read > 0);

    csum = checksum.get();

    return ClientResult::Success;
}

void MavlinkFtpClient::terminate_session(Work& work)
{
    work.last_opcode = CMD_TERMINATE_SESSION;

    work.payload = {};
    work.payload.seq_number = _last_sent_seq_number++;
    work.payload.session = _session;

    work.payload.opcode = work.last_opcode;
    work.payload.offset = 0;
    work.payload.size = 0;

    send_mavlink_ftp_message(work.payload, work.target_compid);
}

uint8_t MavlinkFtpClient::get_our_compid()
{
    return _system_impl.get_own_component_id();
}

uint8_t MavlinkFtpClient::get_target_component_id()
{
    return _target_component_id_set ? _target_component_id : _system_impl.get_autopilot_id();
}

MavlinkFtpClient::ClientResult MavlinkFtpClient::set_target_compid(uint8_t component_id)
{
    _target_component_id = component_id;
    _target_component_id_set = true;
    return ClientResult::Success;
}

std::ostream& operator<<(std::ostream& str, MavlinkFtpClient::ClientResult const& result)
{
    switch (result) {
        default:
            // Fallthrough
        case MavlinkFtpClient::ClientResult::Unknown:
            return str << "Unknown";
        case MavlinkFtpClient::ClientResult::Success:
            return str << "Success";
        case MavlinkFtpClient::ClientResult::Next:
            return str << "Next";
        case MavlinkFtpClient::ClientResult::Timeout:
            return str << "Timeout";
        case MavlinkFtpClient::ClientResult::Busy:
            return str << "Busy";
        case MavlinkFtpClient::ClientResult::FileIoError:
            return str << "FileIoError";
        case MavlinkFtpClient::ClientResult::FileExists:
            return str << "FileExists";
        case MavlinkFtpClient::ClientResult::FileDoesNotExist:
            return str << "FileDoesNotExist";
        case MavlinkFtpClient::ClientResult::FileProtected:
            return str << "FileProtected";
        case MavlinkFtpClient::ClientResult::InvalidParameter:
            return str << "InvalidParameter";
        case MavlinkFtpClient::ClientResult::Unsupported:
            return str << "Unsupported";
        case MavlinkFtpClient::ClientResult::ProtocolError:
            return str << "ProtocolError";
        case MavlinkFtpClient::ClientResult::NoSystem:
            return str << "NoSystem";
    }
}

void MavlinkFtpClient::cancel_all_operations()
{
    if (_io_context.stopped()) {
        // io_context is stopped and its thread is dead — safe to access directly.
        stop_timer();
        _work_queue.clear();
        return;
    }
    std::promise<void> done;
    asio::post(_io_context, [this, &done]() {
        stop_timer();
        _work_queue.clear();
        done.set_value();
    });
    done.get_future().wait();
}

} // namespace mavsdk
