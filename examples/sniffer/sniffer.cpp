//
// Example how to print every message arriving.
//

#include <cstdint>
#include <future>
#include <mavsdk/mavsdk.h>
#include <iostream>
#include <thread>
#include <unordered_map>

using namespace mavsdk;
using namespace std::this_thread;
using namespace std::chrono;

void get_message_names(mavlink_message_t& message);

void usage(const std::string& bin_name)
{
    std::cerr << "Usage : " << bin_name << " <connection_url>\n"
              << "Connection URL format should be :\n"
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

    mavsdk.intercept_incoming_messages_async([](mavlink_message_t& message) {
        get_message_names(message);
        return true;

    });

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}


/// @brief Get the name of the message from the message id for Mavlink V1 messages
/// @param message 
void get_message_names(mavlink_message_t& message) {

    std::unordered_map<int, std::string> message_names = {
        {0, "HEARTBEAT"},
        {1, "SYS_STATUS"},
        {2, "SYSTEM_TIME"},
        {4, "PING"},
        {5, "CHANGE_OPERATOR_CONTROL"},
        {6, "CHANGE_OPERATOR_CONTROL_ACK"},
        {7, "AUTH_KEY"},
        {11, "SET_MODE"},
        {20, "PARAM_REQUEST_READ"},
        {21, "PARAM_REQUEST_LIST"},
        {22, "PARAM_VALUE"},
        {23, "PARAM_SET"},
        {24, "GPS_RAW_INT"},
        {25, "GPS_STATUS"},
        {26, "SCALED_IMU"},
        {27, "RAW_IMU"},
        {28, "RAW_PRESSURE"},
        {29, "SCALED_PRESSURE"},
        {30, "ATTITUDE"},
        {31, "ATTITUDE_QUATERNION"},
        {32, "LOCAL_POSITION_NED"},
        {33, "GLOBAL_POSITION_INT"},
        {34, "RC_CHANNELS_SCALED"},
        {35, "RC_CHANNELS_RAW"},
        {36, "SERVO_OUTPUT_RAW"},
        {37, "MISSION_REQUEST_PARTIAL_LIST"},
        {38, "MISSION_WRITE_PARTIAL_LIST"},
        {39, "MISSION_ITEM"},
        {40, "MISSION_REQUEST"},
        {41, "MISSION_SET_CURRENT"},
        {42, "MISSION_CURRENT"},
        {43, "MISSION_REQUEST_LIST"},
        {44, "MISSION_COUNT"},
        {45, "MISSION_CLEAR_ALL"},
        {46, "MISSION_ITEM_REACHED"},
        {47, "MISSION_ACK"},
        {48, "SET_GPS_GLOBAL_ORIGIN"},
        {49, "GPS_GLOBAL_ORIGIN"},
        {50, "PARAM_MAP_RC"},
        {51, "MISSION_REQUEST_INT"},
        {54, "SAFETY_SET_ALLOWED_AREA"},
        {55, "SAFETY_ALLOWED_AREA"},
        {61, "ATTITUDE_QUATERNION_COV"},
        {62, "NAV_CONTROLLER_OUTPUT"},
        {63, "GLOBAL_POSITION_INT_COV"},
        {64, "LOCAL_POSITION_NED_COV"},
        {65, "RC_CHANNELS"},
        {66, "REQUEST_DATA_STREAM"},
        {67, "DATA_STREAM"},
        {69, "MANUAL_CONTROL"},
        {70, "RC_CHANNELS_OVERRIDE"},
        {73, "MISSION_ITEM_INT"},
        {74, "VFR_HUD"},
        {75, "COMMAND_INT"},
        {76, "COMMAND_LONG"},
        {77, "COMMAND_ACK"},
        {80, "COMMAND_CANCEL"},
        {81, "MANUAL_SETPOINT"},
        {82, "SET_ATTITUDE_TARGET"},
        {83, "ATTITUDE_TARGET"},
        {84, "SET_POSITION_TARGET_LOCAL_NED"},
        {85, "POSITION_TARGET_LOCAL_NED"},
        {86, "SET_POSITION_TARGET_GLOBAL_INT"},
        {87, "POSITION_TARGET_GLOBAL_INT"},
        {89, "LOCAL_POSITION_NED_SYSTEM_GLOBAL_OFFSET"},
        {90, "HIL_STATE"},
        {91, "HIL_CONTROLS"},
        {92, "HIL_RC_INPUTS_RAW"},
        {93, "HIL_ACTUATOR_CONTROLS"},
        {100, "OPTICAL_FLOW"},
        {101, "GLOBAL_VISION_POSITION_ESTIMATE"},
        {102, "VISION_POSITION_ESTIMATE"},
        {103, "VISION_SPEED_ESTIMATE"},
        {104, "VICON_POSITION_ESTIMATE"},
        {105, "HIGHRES_IMU"},
        {106, "OPTICAL_FLOW_RAD"},
        {107, "HIL_SENSOR"},
        {108, "SIM_STATE"},
        {109, "RADIO_STATUS"},
        {110, "FILE_TRANSFER_PROTOCOL"},
        {111, "TIMESYNC"},
        {112, "CAMERA_TRIGGER"},
        {113, "HIL_GPS"},
        {114, "HIL_OPTICAL_FLOW"},
        {115, "HIL_STATE_QUATERNION"},
        {116, "SCALED_IMU2"},
        {117, "LOG_REQUEST_LIST"},
        {118, "LOG_ENTRY"},
        {119, "LOG_REQUEST_DATA"},
        {120, "LOG_DATA"},
        {121, "LOG_ERASE"},
        {122, "LOG_REQUEST_END"},
        {123, "GPS_INJECT_DATA"},
        {124, "GPS2_RAW"},
        {125, "POWER_STATUS"},
        {126, "SERIAL_CONTROL"},
        {127, "GPS_RTK"},
        {128, "GPS2_RTK"},
        {129, "SCALED_IMU3"},
        {130, "DATA_TRANSMISSION_HANDSHAKE"},
        {131, "ENCAPSULATED_DATA"},
        {132, "DISTANCE_SENSOR"},
        {133, "TERRAIN_REQUEST"},
        {134, "TERRAIN_DATA"},
        {135, "TERRAIN_CHECK"},
        {136, "TERRAIN_REPORT"},
        {137, "SCALED_PRESSURE2"},
        {138, "ATT_POS_MOCAP"},
        {139, "SET_ACTUATOR_CONTROL_TARGET"},
        {140, "ACTUATOR_CONTROL_TARGET"},
        {141, "ALTITUDE"},
        {142, "RESOURCE_REQUEST"},
        {143, "SCALED_PRESSURE3"},
        {144, "FOLLOW_TARGET"},
        {146, "CONTROL_SYSTEM_STATE"},
        {147, "BATTERY_STATUS"},
        {148, "AUTOPILOT_VERSION"},
        {149, "LANDING_TARGET"},
        {162, "FENCE_STATUS"},
        {192, "MAG_CAL_REPORT"},
        {225, "EFI_STATUS"},
        {230, "ESTIMATOR_STATUS"},
        {231, "WIND_COV"},
        {232, "GPS_INPUT"},
        {233, "GPS_RTCM_DATA"},
        {234, "HIGH_LATENCY"},
        {235, "HIGH_LATENCY2"},
        {241, "VIBRATION"},
        {242, "HOME_POSITION"},
        {243, "SET_HOME_POSITION"},
        {244, "MESSAGE_INTERVAL"},
        {245, "EXTENDED_SYS_STATE"},
        {246, "ADSB_VEHICLE"},
        {247, "COLLISION"},
        {248, "V2_EXTENSION"},
        {249, "MEMORY_VECT"},
        {250, "DEBUG_VECT"},
        {251, "NAMED_VALUE_FLOAT"},
        {252, "NAMED_VALUE_INT"},
        {253, "STATUSTEXT"},
        {254, "DEBUG"}};

    std::cout << "Got message[" << message.msgid << "] " << message_names[message.msgid] << '\n';
};
