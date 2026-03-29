//
// Example how to use C API of mavsdk_server.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/mavsdk_server/mavsdk_server_api.h>
#include <atomic>
#include <iostream>
#include <chrono>
#include <csignal>
#include <thread>

using namespace mavsdk;

static std::atomic<bool> _should_stop{false};

void signal_handler(int sig)
{
    std::cout << "Received signal " << sig << std::endl;
    _should_stop.store(true);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Connection string required as argument" << std::endl;
        return 1;
    }

    signal(SIGINT, signal_handler);

    MavsdkServer* mavsdk_server;

    mavsdk_server_init(&mavsdk_server);

    // This returns when a system has been discovered.
    int ret = mavsdk_server_run(mavsdk_server, argv[1], 50051);
    if (ret != 0) {
        std::cout << "mavsdk_server_run failed: " << ret << std::endl;
        mavsdk_server_destroy(mavsdk_server);
        return ret;
    }

    while (!_should_stop.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    mavsdk_server_stop(mavsdk_server);
    mavsdk_server_destroy(mavsdk_server);

    return 0;
}
