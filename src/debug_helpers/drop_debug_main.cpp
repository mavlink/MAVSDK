#include <iostream>
#include <unistd.h>
#include "mavsdk.h"
#include <thread>

#if DROP_DEBUG != 1
#error DROB_DEBUG needs to be set for this test to work
#endif

int main(int, const char**)
{
    mavsdk::Mavsdk mavsdk;

    mavsdk::ConnectionResult ret = mavsdk.add_udp_connection();
    if (ret != mavsdk::ConnectionResult::Success) {
        std::cout << "failed to add connection" << '\n';
        return -1;
    }

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
