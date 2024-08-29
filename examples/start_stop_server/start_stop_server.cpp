//
// Example how to use C API of mavsdk_server.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/mavsdk_server/mavsdk_server_api.h>
#include <iostream>
#include <csignal>

using namespace mavsdk;


static struct MavsdkServer* mavsdk_server;


void signal_handler(int sig) {
    std::cout << "Received SIGUSR1 signal" << std::endl;

    mavsdk_server_stop(mavsdk_server);
}


int main(int, char**)
{
    signal(SIGUSR1, signal_handler);

    mavsdk_server_init(&mavsdk_server);
    mavsdk_server_run(mavsdk_server, "serial:///dev/serial/dev/by-id/usb-Auterion_PX4_FMU_v6C.x_0-if00", 50051);

    mavsdk_server_destroy(mavsdk_server);

    return 0;
}
