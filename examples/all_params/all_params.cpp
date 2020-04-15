//
// Simple test to get all params.
//
// Author: Julian Oes <julian@oes.ch>

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_passthrough/mavlink_passthrough.h>

using namespace mavsdk;
using Params = std::vector<bool>;

static void usage(const std::string& bin_name);
static void request_params(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough);
static void subscribe_to_params(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough, Params& params, std::mutex& params_mutex);



int main(int argc, char** argv)
{
    Mavsdk mavsdk;
    std::string connection_url;
    ConnectionResult connection_result;

    if (argc == 2) {
        connection_url = argv[1];
        connection_result = mavsdk.add_any_connection(connection_url);
    } else {
        usage(argv[0]);
        return 1;
    }

    if (connection_result != ConnectionResult::SUCCESS) {
        std::cout << "Connection failed: " << connection_result_str(connection_result) << std::endl;
        return 1;
    }

    std::promise<void> prom;
    std::future<void> fut = prom.get_future();
    std::cout << "Waiting to discover system..." << std::endl;
    mavsdk.register_on_discover([&prom](uint64_t /* uuid*/) {
            prom.set_value();
            });

    if (fut.wait_for(std::chrono::seconds(5)) != std::future_status::ready) {
        std::cout << "No device found, exiting." << std::endl;
        return 1;
    }

    mavsdk.register_on_discover(nullptr);

    System& system = mavsdk.system();

    std::this_thread::sleep_for(std::chrono::seconds(3));

    auto mavlink_passthrough = std::make_shared<MavlinkPassthrough>(system);


    std::mutex params_mutex{};
    Params params{};

    subscribe_to_params(mavlink_passthrough, params, params_mutex);

    const auto start = std::chrono::steady_clock::now();

    request_params(mavlink_passthrough);

    unsigned last_count = 0;
    unsigned times_equal = 0;
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        const std::lock_guard<std::mutex> lock(params_mutex);
        auto current_count = std::count(params.begin(), params.end(), true);
        std::cout << "Received " << current_count << " of " << params.size() << " (" << float(current_count) / float(params.size()) * 100.f << " %)" << std::endl;
        if (last_count == current_count) {
            if (++times_equal > 10) {
                break;
            }
        } else {
            times_equal = 0;
        }

        last_count = current_count;
    }

    const auto end = std::chrono::steady_clock::now();

    std::cout << "Elapsed: " << double(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()-1000) / 1000. << " s" << std::endl;

    return 0;
}

void usage(const std::string& bin_name)
{
    std::cout << "Usage : " << bin_name << " <connection_url>" << std::endl
              << "Connection URL format should be :" << std::endl
              << " For TCP : tcp://[server_host][:server_port]" << std::endl
              << " For UDP : udp://[bind_host][:bind_port]" << std::endl
              << " For Serial : serial:///path/to/serial/dev[:baudrate]" << std::endl
              << "For example, to connect to the simulator use URL: udp://:14540" << std::endl;
}

void request_params(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough)
{
    mavlink_message_t message;
    mavlink_msg_param_request_list_pack(
        mavlink_passthrough->get_our_sysid(),
        mavlink_passthrough->get_our_compid(),
        &message,
        mavlink_passthrough->get_target_sysid(),
        mavlink_passthrough->get_target_compid());

    mavlink_passthrough->send_message(message);
}

void subscribe_to_params(std::shared_ptr<MavlinkPassthrough> mavlink_passthrough, Params& params, std::mutex& params_mutex)
{
    static int last_index = -1;

    mavlink_passthrough->subscribe_message_async(MAVLINK_MSG_ID_PARAM_VALUE,
            [&params, &params_mutex](const mavlink_message_t& message) {
        mavlink_param_value_t param_value;
        mavlink_msg_param_value_decode(&message, &param_value);

        // Ignore the hash.
        if (int16_t(param_value.param_index) == -1) {
            return;
        }

        const std::lock_guard<std::mutex> lock(params_mutex);

        std::cout << "index: " << param_value.param_index << std::endl;
        const int diff = int(param_value.param_index) - last_index;
        if (diff > 1) {
            std::cout << "Missed " << int(diff - 1) << std::endl;
        }
        last_index = param_value.param_index;

        if (params.size() == 0) {
            params.resize(param_value.param_count, false);
        }
        params[param_value.param_index] = true;
    });
}
