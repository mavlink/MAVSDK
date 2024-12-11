#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <csignal>
#include <map>
#include <optional>

using namespace mavsdk;

// Constants
constexpr size_t MAX_QUEUE_SIZE = 1000;
constexpr int PROXY_SYSID_PX4 = 127;
constexpr int PROXY_SYSID_QGC = 128;
constexpr int TIMEOUT_SECONDS = 10;

#define PX4_NAME "PX4"
#define QGC_NAME "QGC"

// Global variables
std::atomic<bool> running{true};
std::map<std::string, std::queue<mavlink_message_t>> message_queues;
std::map<std::string, std::unique_ptr<std::mutex>> queue_mutexes;
std::map<std::string, std::unique_ptr<std::condition_variable>> queue_cvs;
std::map<std::string, std::shared_ptr<MavlinkPassthrough>> passthrough_map;
std::mutex passthrough_map_mutex;

// Thread-safe logging
void log_message(const std::string& level, const std::string& message) {
    static std::mutex log_mutex;
    std::lock_guard<std::mutex> lock(log_mutex);
    std::cout << "[" << level << "] " << message << std::endl;
}

// Graceful shutdown on SIGINT
void signal_handler(int signal) {
    running = false;
    for (auto& [_, cv] : queue_cvs) {
        cv->notify_all();
    }
}

void intercept_outgoing_messages(Mavsdk& mavsdk, int proxy_sys_px4, int proxy_sys_qgc) {
    mavsdk.intercept_outgoing_messages_async([proxy_sys_px4, proxy_sys_qgc](mavlink_message_t& message) {
        if (message.sysid == proxy_sys_px4 || message.sysid == proxy_sys_qgc) {
            return false;
        }
        return true;
    });
}

// Enqueues a MAVLink message for processing
void enqueue_message(const std::string& source_name, const mavlink_message_t& message) {
    const std::string target_name = (source_name == PX4_NAME) ? QGC_NAME : PX4_NAME;
    std::lock_guard<std::mutex> lock(*queue_mutexes[target_name]);
    auto& queue = message_queues[target_name];

    if (queue.size() >= MAX_QUEUE_SIZE) {
        log_message("WARNING", "Queue full for " + target_name + ". Dropping message ID: " + std::to_string(message.msgid));
        return;
    }

    queue.push(message);
    queue_cvs[target_name]->notify_one();
}

// Processes messages for a specific target
void message_processor(const std::string& target_name) {
    while (running) {
        mavlink_message_t message;
        {
            std::unique_lock<std::mutex> lock(*queue_mutexes[target_name]);
            queue_cvs[target_name]->wait(lock, [&]() {
                return !message_queues[target_name].empty() || !running;
            });

            if (!running) break;

            message = message_queues[target_name].front();
            message_queues[target_name].pop();
        }

        std::shared_ptr<MavlinkPassthrough> passthrough;
        {
            std::lock_guard<std::mutex> lock(passthrough_map_mutex);
            passthrough = passthrough_map[target_name];
        }

        if (!passthrough) {
            log_message("ERROR", "Passthrough not found for target: " + target_name);
            continue;
        }

        auto result = passthrough->queue_message([&](MavlinkAddress, uint8_t) { return message; });
        if (result != MavlinkPassthrough::Result::Success) {
            log_message("ERROR", "Failed to forward message ID: " + std::to_string(message.msgid) + " to " + target_name);
        }
    }
}

// Waits for a system to connect
std::optional<std::shared_ptr<System>> wait_for_system(Mavsdk& mavsdk) {
    for (int i = 0; i < TIMEOUT_SECONDS; ++i) {
        if (!mavsdk.systems().empty()) {
            return mavsdk.systems().front();
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return std::nullopt;
}

int main(int argc, char** argv) {
    signal(SIGINT, signal_handler);

    if (argc != 3) {
        log_message("ERROR", "Usage: " + std::string(argv[0]) + " <url_qgc> <url_px4>");
        return 1;
    }

    Mavsdk mavsdk_px4(Mavsdk::Configuration(PROXY_SYSID_PX4, 1, true));
    Mavsdk mavsdk_qgc(Mavsdk::Configuration(PROXY_SYSID_QGC, 1, true));

    auto px4_conn_result = mavsdk_px4.add_any_connection(argv[2]);
    auto qgc_conn_result = mavsdk_qgc.add_any_connection(argv[1]);

    if (px4_conn_result != ConnectionResult::Success || qgc_conn_result != ConnectionResult::Success) {
        log_message("ERROR", "Failed to connect to the provided URLs.");
        return 1;
    }

    log_message("INFO", "Waiting for systems to connect...");

    intercept_outgoing_messages(mavsdk_px4, PROXY_SYSID_PX4, PROXY_SYSID_QGC);
    //intercept_outgoing_messages(mavsdk_qgc, PROXY_SYSID_PX4, PROXY_SYSID_QGC);

    auto px4_system = wait_for_system(mavsdk_px4);
    auto qgc_system = wait_for_system(mavsdk_qgc);

    if (!px4_system || !qgc_system) {
        log_message("ERROR", "Failed to connect to one or both systems within the timeout.");
        return 1;
    }

    {
        std::lock_guard<std::mutex> lock(passthrough_map_mutex);
        passthrough_map[PX4_NAME] = std::make_shared<MavlinkPassthrough>(px4_system.value());
        passthrough_map[QGC_NAME] = std::make_shared<MavlinkPassthrough>(qgc_system.value());
    }

    message_queues[PX4_NAME] = std::queue<mavlink_message_t>();
    message_queues[QGC_NAME] = std::queue<mavlink_message_t>();
    queue_mutexes[PX4_NAME] = std::make_unique<std::mutex>();
    queue_mutexes[QGC_NAME] = std::make_unique<std::mutex>();
    queue_cvs[PX4_NAME] = std::make_unique<std::condition_variable>();
    queue_cvs[QGC_NAME] = std::make_unique<std::condition_variable>();

    for (uint16_t msg_id = 0; msg_id < 500; ++msg_id) {
        passthrough_map[PX4_NAME]->subscribe_message(msg_id, [&](const mavlink_message_t& message) {
            //std::thread([=]() { enqueue_message(PX4_NAME, message); }).detach();
            enqueue_message(PX4_NAME, message);
        });
        passthrough_map[QGC_NAME]->subscribe_message(msg_id, [&](const mavlink_message_t& message) {
            //std::thread([=]() { enqueue_message(QGC_NAME, message); }).detach();
            enqueue_message(QGC_NAME, message);
        });
    }

    std::thread px4_processor_thread(message_processor, PX4_NAME);
    std::thread qgc_processor_thread(message_processor, QGC_NAME);

    log_message("INFO", "Proxy running. Press Ctrl+C to stop.");

    px4_processor_thread.join();
    qgc_processor_thread.join();

    log_message("INFO", "Proxy terminated.");

    return 0;
}
