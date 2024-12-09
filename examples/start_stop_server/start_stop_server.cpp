//
// Example how to use C API of mavsdk_server.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/mavsdk_server/mavsdk_server_api.h>
#include <iostream>
#include <csignal>

using namespace mavsdk;

static struct MavsdkServer* mavsdk_server;

void signal_handler(int sig)
{
    std::cout << "Received SIGUSR1 signal" << std::endl;

    mavsdk_server_stop(mavsdk_server);
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "Connection string required as argument" << std::endl;
        return 1;
    }

    signal(SIGUSR1, signal_handler);

    mavsdk_server_init(&mavsdk_server);
    mavsdk_server_run(mavsdk_server, argv[1], 50051);

    mavsdk_server_destroy(mavsdk_server);

    return 0;
}
