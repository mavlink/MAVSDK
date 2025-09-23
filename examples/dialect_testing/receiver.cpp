// Playground Onboard

#include <chrono>
#include <iostream>
#include <thread>

#include <fstream>
#include <sstream>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>

using namespace mavsdk;

int main(int /*argc*/, char** /*argv*/)
{
    // Connect via serial device
    const std::string connectionUrl("udpout://127.0.0.1:18000");

    // Initialize MAVSDK with onboard computer component type
    mavsdk::Mavsdk mavsdk{mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::CompanionComputer}};

    // Add connection
    mavsdk::ConnectionResult connection_result = mavsdk.add_any_connection(connectionUrl);
    if (connection_result != mavsdk::ConnectionResult::Success) {
        std::cerr << "Connection failed: " << connection_result << std::endl;
        return 1;
    }

    // Wait for the system to connect
    std::cout << "Waiting 1 second for system to connect..." << std::endl;
    while (mavsdk.systems().size() == 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Get the connected system
    auto system = mavsdk.systems().at(0);
    if (!system->is_connected()) {
        std::cerr << "System not connected" << std::endl;
        return 1;
    }

    // Instantiate the plugin
    auto mavlinkDirect = mavsdk::MavlinkDirect{system};

    // Setup the MavlinkDirect to load my mavlink dialect.
    // The dialect is exactly the same as the one used for building the PX4 firmware
    std::ifstream file("DialectTesting.xml");
    if (file.is_open()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string dialectXml = buffer.str();
        file.close();

        mavsdk::MavlinkDirect::Result mavlinkDirectResult = mavlinkDirect.load_custom_xml(dialectXml);
        if (mavlinkDirectResult == mavsdk::MavlinkDirect::Result::Success)
            std::cout << "Successfully loaded mavlink DialectTesting via xml" << std::endl;
        else
            std::cout << "Could not load mavlink DialectTesting via xml" << std::endl;
    }
    else {
        std::cout << "Failed to open Mavlink DialectTesting XML file" << std::endl;
    }

    // Subscribe to FDV_SET_IMAGE_PROCESSING_PARAMETERS messages
    auto handle = mavlinkDirect.subscribe_message(
        "FDV_SET_IMAGE_PROCESSING_PARAMETERS",
        [](MavlinkDirect::MavlinkMessage message) {
            std::cout << "Received image processing parameters message: " << message.fields_json << std::endl;

            // And this could now be parsed by jsoncpp or nlohmann/json:
            // Json::Value json;
            // Json::Reader reader;
            // if (reader.parse(message.fields_json, json)) {
            //     auto lat = json["lat"].asInt() / 1e7;  // Convert from degrees * 1e7
            //     auto lon = json["lon"].asInt() / 1e7;
            //     std::cout << "Position: " << lat << ", " << lon << std::endl;
            // }
        }
        );

    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
