//
// This is an example how to desctruct Mavsdk when a system has disconnected
// and restart it right after.
//

#include <future>
#include <iostream>
#include <memory>
#include <string>

#include <mavsdk/mavsdk.h>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

bool connect(Mavsdk* mavsdk, std::string connection_url)
{
    auto prom = std::promise<void>();
    auto fut = prom.get_future();

    Mavsdk::NewSystemHandle handle = mavsdk->subscribe_on_new_system([&mavsdk, &handle, &prom]() {
        const auto system = mavsdk->systems().at(0);

        if (system->is_connected()) {
            mavsdk->unsubscribe_on_new_system(handle);
            prom.set_value();
        }
    });
    auto ret = mavsdk->add_any_connection(connection_url);
    fut.wait();

    return ret == ConnectionResult::Success;
}

void wait_for_disconnect(Mavsdk* mavsdk)
{
    auto system = mavsdk->systems()[0];

    auto prom = std::promise<void>();
    auto fut = prom.get_future();
    System::IsConnectedHandle handle =
        Handle<bool>(system->subscribe_is_connected([&system, &prom, &handle](bool connected) {
            if (!connected) {
                std::cout << "Disconnection detected" << std::endl;
                system->unsubscribe_is_connected(handle);
                prom.set_value();
            }
        }));

    fut.wait();
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    const std::string connection_url = argv[1];

    std::unique_ptr<Mavsdk> mavsdk;

    mavsdk = std::make_unique<Mavsdk>();

    while (true) {
        if (connect(mavsdk.get(), connection_url)) {
            std::cout << "Connection successful" << std::endl;
        } else {
            break;
        }

        wait_for_disconnect(mavsdk.get());

        // Destruct and construct Mavsdk.
        mavsdk = std::make_unique<Mavsdk>();
    }

    return 0;
}
