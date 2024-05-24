#include <iostream>
#include <thread>
#include <chrono>
#include <mavsdk/mavsdk.h>
#include <mavsdk/plugins/arm_authorizer_server/arm_authorizer_server.h>
#include <chrono>

constexpr uint32_t ARM_AUTHORIZATION_DELAY_S = 5;

using mavsdk::ArmAuthorizerServer;

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cerr << "Too few input arguments. usage: $ arm_authorizer_server <connection_url>"
                  << std::endl;
        return 1;
    }

    // Create mavlink component with CompanionComputer id
    mavsdk::Mavsdk mavsdk{
        mavsdk::Mavsdk::Configuration{mavsdk::Mavsdk::ComponentType::GroundStation}};

    auto result = mavsdk.add_any_connection(argv[1]);

    if (result != mavsdk::ConnectionResult::Success) {
        std::cerr << "Could not establish connection: " << result << std::endl;
        return 2;
    }
    std::cout << "Created arm authorizer server connection" << std::endl;

    auto arm_authorizer = ArmAuthorizerServer{mavsdk.server_component()};

    // Rememember the time when execution started. Arming will be authorized after specific time
    // interval elapsed
    auto start_time = std::chrono::system_clock::now();

    arm_authorizer.subscribe_arm_authorization([&](uint32_t system_id) {
        std::cout << "Arm authorization request received. Request for system ID: " << system_id
                  << std::endl;
        auto elapsed_s = std::chrono::duration_cast<std::chrono::seconds>(
                             std::chrono::system_clock::now() - start_time)
                             .count();

        ArmAuthorizerServer::Result result;
        // Allow arm if sufficient time has passed
        if (elapsed_s > ARM_AUTHORIZATION_DELAY_S) {
            result = arm_authorizer.accept_arm_authorization(5);
        } else {
            result = arm_authorizer.reject_arm_authorization(
                true, ArmAuthorizerServer::RejectionReason::Generic, 10);
        }
        if (result == ArmAuthorizerServer::Result::Success) {
            std::cout << "Successfully sent the response" << std::endl;
        } else {
            std::cout << "Failed to send the response" << std::endl;
        }
    });

    // works as a server and never quit
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
