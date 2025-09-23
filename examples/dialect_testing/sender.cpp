// Playground GroundStation

#include <chrono>
#include <iostream>
#include <thread>

#include <fstream>
#include <sstream>

#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/mavlink_direct/mavlink_direct.h>

#include <nlohmann/json.hpp>

using namespace mavsdk;

int main(int /*argc*/, char** /*argv*/)
{
    const std::string connectionUrl("udpin://0.0.0.0:18000");

    // Initialize MAVSDK with GroundStation component type
    auto config = mavsdk::Mavsdk::Configuration{mavsdk::ComponentType::GroundStation};
    mavsdk::Mavsdk mavsdk{config};

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

    // Create FDV_SET_IMAGE_PROCESSING_PARAMETERS message
    MavlinkDirect::MavlinkMessage setImageProcessingParametersMessage{};
    setImageProcessingParametersMessage.message_name = "FDV_SET_IMAGE_PROCESSING_PARAMETERS";
    setImageProcessingParametersMessage.system_id = config.get_system_id();  // Our component's system ID
    setImageProcessingParametersMessage.component_id = config.get_component_id();  // Our component's component ID
    setImageProcessingParametersMessage.target_system_id = 0;  // broadcast
    setImageProcessingParametersMessage.target_component_id = 0;  // broadcast

    int counter = 0;
    uint64_t timestampUs{ 12345 };
    uint8_t colorMapId = 1;
    while (counter < 20) {
        // Cycle through the color maps
        colorMapId++;
        if (colorMapId > 4)
            colorMapId = 1;

        nlohmann::json data;
        data["time_usec"] = timestampUs;
        data["thermal_color_map"] = colorMapId;
        setImageProcessingParametersMessage.fields_json = data.dump();

        // Send the message
        auto result = mavlinkDirect.send_message(setImageProcessingParametersMessage);
        if (result == MavlinkDirect::Result::Success) {
            std::cout << "Message colormap " << (counter + 1) << "/20 sent successfully" << std::endl;
        }
        else {
            std::cerr << "Message colormap could not be sent: " << result << std::endl;
        }

        counter++;

        // Wait for 1 second before sending the next message
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    std::cout << "Sent all 20 messages. Exiting." << std::endl;

    return 0;
}
