#include <iostream>
#include <unistd.h>
#include <gtest/gtest.h>
#include "dronelink.h"

using namespace dronelink;

void print_mode(dronelink::Telemetry::FlightMode flight_mode);
static Telemetry::FlightMode _flight_mode = Telemetry::FlightMode::UNKNOWN;

TEST(Telemetry, FlightModes)
{
    DroneLink dl;

    DroneLink::ConnectionResult ret = dl.add_udp_connection();
    ASSERT_EQ(ret, DroneLink::ConnectionResult::SUCCESS);

    usleep(1500000);

    std::vector<uint64_t> uuids = dl.device_uuids();

    ASSERT_EQ(uuids.size(), 1);

    uint64_t uuid = uuids.at(0);

    Device &device = dl.device(uuid);

    device.telemetry().flight_mode_async(std::bind(&print_mode, std::placeholders::_1));

    device.action().arm();
    usleep(2000000);
    device.action().takeoff();
    usleep(2000000);
    ASSERT_EQ(_flight_mode, Telemetry::FlightMode::TAKEOFF);
    device.action().land();
    usleep(2000000);
    ASSERT_EQ(_flight_mode, Telemetry::FlightMode::LAND);

}

void print_mode(Telemetry::FlightMode flight_mode)
{
    std::cout << "Got FlightMode: " << int(flight_mode) << std::endl;
    _flight_mode = flight_mode;
}

