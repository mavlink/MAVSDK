//
// Simple example to demonstrate how to use the component metadata interface.
//

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/component_metadata/component_metadata.h>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <future>

using namespace mavsdk;

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be:\n"
              << " For TCP : tcp://[server_host][:server_port]\n"
              << " For UDP : udp://[bind_host][:bind_port]\n"
              << " For Serial : serial:///path/to/serial/dev[:baudrate]\n"
              << "For example, to connect to the simulator use URL: udp://:14540\n";
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        usage(argv[0]);
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{Mavsdk::ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system) {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    auto component_metadata = ComponentMetadata{system.value()};
    component_metadata.request_autopilot_component();
    std::promise<bool> promise;
    auto future = promise.get_future();
    component_metadata.subscribe_metadata_available(
        [&promise](ComponentMetadata::MetadataUpdate data) {
            if (data.type != ComponentMetadata::MetadataType::AllCompleted) {
                std::cout << "Got metadata: type: " << (int)data.type << ", compid: " << data.compid
                          << std::endl;
            }
            std::string filename;
            switch (data.type) {
                case ComponentMetadata::MetadataType::Parameter:
                    filename = "parameters.json";
                    break;
                case ComponentMetadata::MetadataType::Events:
                    filename = "events.json";
                    break;
                case ComponentMetadata::MetadataType::Actuators:
                    filename = "actuators.json";
                    break;
                case ComponentMetadata::MetadataType::AllCompleted:
                    promise.set_value(true);
                    break;
            }
            if (!filename.empty()) {
                filename = std::to_string(data.compid) + "_" + filename;
                std::cout << "  Writing JSON data to " << filename << std::endl;
                std::ofstream out(filename);
                out << data.json_metadata;
                out.close();
            }
        });

    future.get();
    return 0;
}
