#include <iostream>
#include <unistd.h>
#include "integration_test_helper.h"
#include "dronelink.h"

using namespace dronelink;

void print_mode(Telemetry::FlightMode flight_mode);
static Telemetry::FlightMode _flight_mode = Telemetry::FlightMode::UNKNOWN;

TEST_F(SitlTest, TelemetryFlightModes)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);
    sleep(2);

    Device &device = dl.device();
    device.telemetry().flight_mode_async(std::bind(&print_mode, std::placeholders::_1));

    while (!device.telemetry().health_all_ok()) {
        std::cout << "waiting for device to be ready" << std::endl;
        sleep(1);
    }

    device.action().arm();
    sleep(2);
    device.action().takeoff();
    sleep(2);
    ASSERT_EQ(_flight_mode, Telemetry::FlightMode::TAKEOFF);
    device.action().land();
    sleep(2);
    ASSERT_EQ(_flight_mode, Telemetry::FlightMode::LAND);
}

void print_mode(Telemetry::FlightMode flight_mode)
{
    std::cout << "Got FlightMode: " << int(flight_mode) << std::endl;
    _flight_mode = flight_mode;
}

