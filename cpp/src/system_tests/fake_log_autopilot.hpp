#pragma once

// A minimal fake autopilot that serves in-memory log files over the MAVLink
// log protocol (LOG_REQUEST_LIST / LOG_ENTRY / LOG_REQUEST_DATA / LOG_DATA /
// LOG_REQUEST_END), so LogFiles can be exercised without a real flight stack.
//
// Knobs:
//  - packet_delay:      sleep between LOG_DATA packets (slows the transfer down)
//  - stop_after_bytes:  stop answering data requests once this many bytes were
//                       served, simulating a vehicle that disappears mid-download
//  - ignore_end:        ignore LOG_REQUEST_END and keep streaming LOG_DATA of the
//                       log being served at that moment until destroyed, so the
//                       client is guaranteed to receive late data

#include "mavsdk.hpp"
#include "plugins/mavlink_direct_server/mavlink_direct_server.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>

namespace mavsdk {

class FakeLogAutopilot {
public:
    static constexpr uint32_t DATA_LEN = 90;

    // One log per size, with ids 0, 1, ... (PX4 style, 0-based) and different content.
    FakeLogAutopilot(Mavsdk& mavsdk_autopilot, std::vector<size_t> log_sizes_bytes) :
        _server(mavsdk_autopilot.server_component()),
        _logs(make_logs(log_sizes_bytes))
    {
        _list_handle = _server.subscribe_message(
            "LOG_REQUEST_LIST",
            [this](MavlinkDirectServer::MavlinkMessage) { send_log_entries(); });

        _data_handle = _server.subscribe_message(
            "LOG_REQUEST_DATA", [this](MavlinkDirectServer::MavlinkMessage message) {
                auto json = nlohmann::json::parse(message.fields_json);
                std::lock_guard<std::mutex> lock(_mutex);
                _requests.push_back(
                    {json["id"].get<uint16_t>(),
                     json["ofs"].get<uint32_t>(),
                     json["count"].get<uint32_t>()});
                _cv.notify_one();
            });

        _end_handle = _server.subscribe_message(
            "LOG_REQUEST_END", [this](MavlinkDirectServer::MavlinkMessage) {
                ++_end_requests;
                std::lock_guard<std::mutex> lock(_mutex);
                if (ignore_end) {
                    _stream_after_end = true;
                    _stream_id = _current_id;
                    _cv.notify_one();
                    return;
                }
                _requests.clear();
                _abort_current = true;
            });

        _worker = std::thread([this]() { serve(); });
    }

    ~FakeLogAutopilot()
    {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _should_exit = true;
            _cv.notify_one();
        }
        _worker.join();
        _server.unsubscribe_message(_list_handle);
        _server.unsubscribe_message(_data_handle);
        _server.unsubscribe_message(_end_handle);
    }

    const std::vector<uint8_t>& log(uint16_t id = 0) const { return _logs.at(id); }

    std::atomic<std::chrono::microseconds> packet_delay{std::chrono::microseconds{0}};
    std::atomic<uint64_t> stop_after_bytes{UINT64_MAX};
    std::atomic<bool> ignore_end{false};

    std::atomic<uint64_t> bytes_served{0};
    // Data requests ignored because the "vehicle" stopped answering.
    std::atomic<uint64_t> data_requests_after_stop{0};
    // LOG_DATA packets sent after an ignored LOG_REQUEST_END.
    std::atomic<uint64_t> packets_after_end{0};

    void reset_counters() { _end_requests = 0; }
    unsigned end_requests() const { return _end_requests; }

private:
    struct Request {
        uint16_t id;
        uint32_t ofs;
        uint32_t count;
    };

    static std::vector<std::vector<uint8_t>> make_logs(const std::vector<size_t>& sizes)
    {
        std::vector<std::vector<uint8_t>> logs;
        for (size_t i = 0; i < sizes.size(); ++i) {
            std::vector<uint8_t> bytes(sizes[i]);
            std::mt19937 rng(static_cast<uint32_t>(42 + i));
            for (auto& b : bytes) {
                b = static_cast<uint8_t>(rng());
            }
            logs.push_back(std::move(bytes));
        }
        return logs;
    }

    void send_log_entries()
    {
        for (size_t id = 0; id < _logs.size(); ++id) {
            MavlinkDirectServer::MavlinkMessage entry;
            entry.message_name = "LOG_ENTRY";
            nlohmann::json json;
            json["id"] = id;
            json["num_logs"] = _logs.size();
            json["last_log_num"] = _logs.size() - 1;
            json["time_utc"] = 1700000000 + id;
            json["size"] = _logs[id].size();
            entry.fields_json = json.dump();
            _server.send_message(entry);
        }
    }

    void serve()
    {
        while (true) {
            Request request{};
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _cv.wait(lock, [this]() {
                    return _should_exit || !_requests.empty() || _stream_after_end;
                });
                if (_should_exit) {
                    return;
                }
                if (_requests.empty()) {
                    const auto stream_id = _stream_id;
                    lock.unlock();
                    stream_one_packet(stream_id);
                    continue;
                }
                request = _requests.front();
                _requests.pop_front();
                _abort_current = false;
                if (request.id >= _logs.size()) {
                    continue;
                }
                _current_id = request.id;
            }

            const auto& log = _logs[request.id];
            const uint64_t end =
                std::min<uint64_t>(uint64_t(request.ofs) + request.count, log.size());
            for (uint64_t ofs = request.ofs; ofs < end; ofs += DATA_LEN) {
                {
                    std::lock_guard<std::mutex> lock(_mutex);
                    if (_should_exit) {
                        return;
                    }
                    if (_abort_current) {
                        break;
                    }
                }
                if (bytes_served >= stop_after_bytes) {
                    ++data_requests_after_stop;
                    break; // Vehicle "gone": silently ignore.
                }
                send_log_data(request.id, static_cast<uint32_t>(ofs), end);
                const auto delay = packet_delay.load();
                if (delay.count() > 0) {
                    std::this_thread::sleep_for(delay);
                }
            }
        }
    }

    // Keeps sending LOG_DATA, cycling through the log, after LOG_REQUEST_END was ignored.
    void stream_one_packet(uint16_t id)
    {
        const auto size = _logs[id].size();
        if (_stream_ofs >= size) {
            _stream_ofs = 0;
        }
        send_log_data(id, _stream_ofs, size);
        _stream_ofs = (_stream_ofs + DATA_LEN < size) ? _stream_ofs + DATA_LEN : 0;
        std::this_thread::sleep_for(std::max(packet_delay.load(), std::chrono::microseconds{1000}));
    }

    void send_log_data(uint16_t id, uint32_t ofs, uint64_t end)
    {
        const uint32_t count = static_cast<uint32_t>(std::min<uint64_t>(DATA_LEN, end - ofs));
        std::vector<int> data(DATA_LEN, 0);
        for (uint32_t i = 0; i < count; ++i) {
            data[i] = _logs[id][ofs + i];
        }

        nlohmann::json json;
        json["id"] = id;
        json["ofs"] = ofs;
        json["count"] = count;
        json["data"] = data;

        MavlinkDirectServer::MavlinkMessage message;
        message.message_name = "LOG_DATA";
        message.fields_json = json.dump();
        _server.send_message(message);
        bytes_served += count;
        if (_end_requests > 0 && ignore_end) {
            ++packets_after_end;
        }
    }

    // Number of LOG_REQUEST_END received since reset_counters().
    std::atomic<unsigned> _end_requests{0};

    MavlinkDirectServer _server;
    const std::vector<std::vector<uint8_t>> _logs;

    MavlinkDirectServer::MessageHandle _list_handle{};
    MavlinkDirectServer::MessageHandle _data_handle{};
    MavlinkDirectServer::MessageHandle _end_handle{};

    std::mutex _mutex;
    std::condition_variable _cv;
    std::deque<Request> _requests;
    bool _abort_current{false};
    bool _stream_after_end{false};
    uint16_t _current_id{0}; // Log being served by the worker.
    uint16_t _stream_id{0}; // Log streamed after an ignored LOG_REQUEST_END.
    uint32_t _stream_ofs{0}; // Only touched by the worker thread.
    bool _should_exit{false};
    std::thread _worker;
};

} // namespace mavsdk
