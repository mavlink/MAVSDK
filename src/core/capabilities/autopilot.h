#pragma once

#include "../node_impl.h"

namespace mavsdk {

class NodeImpl;

class Autopilot {
public:
    enum class FlightMode {
        UNKNOWN = 0,
        READY,
        TAKEOFF,
        HOLD,
        MISSION,
        RETURN_TO_LAUNCH,
        LAND,
        OFFBOARD,
        FOLLOW_ME,
        MANUAL,
        ALTCTL,
        POSCTL,
        ACRO,
        RATTITUDE,
        STABIKIZED,
    };

    class Info {
    public:
        uint64_t get_capabilities();
        uint32_t get_firmware_version();
        uint32_t get_middleware_version();
        uint32_t get_os_version();
        uint32_t get_board_version();
        uint8_t* get_firmware_custom_version();
        uint8_t* get_middleware_custom_version();
        uint8_t* get_os_custom_version();
        uint16_t get_vendor_id();
        uint16_t get_product_id();
        uint64_t get_uid();
        uint8_t* get_uid2();

    private:
        uint64_t _capabilities;
        uint32_t _fw_version;
        uint32_t _mw_version;
        uint32_t _os_version;
        uint32_t _board_version;
        uint8_t* _fw_custom_version;
        uint8_t* _mw_custom_version;
        uint8_t* _os_custom_version;
        uint16_t _vendor_id;
        uint16_t _product_id;
        uint64_t _uid;
        uint8_t* _uid2;
    };

    typedef std::function<void(Info)> autopilot_info_callback_t;
    typedef std::function<void(MAVLinkCommands::Result, float)> command_result_callback_t;
    virtual bool has_capability_autopilot() const;
    virtual void get_autopilot_info_async(const autopilot_info_callback_t callback);

    virtual void set_flight_mode_async(
            FlightMode mode,
            command_result_callback_t callback);

    virtual ~Autopilot();
};

} // namespace mavsdk
