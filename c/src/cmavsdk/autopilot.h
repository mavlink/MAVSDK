#ifndef CMAVSDK_AUTOPILOT_H
#define CMAVSDK_AUTOPILOT_H

#include "mavsdk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MAVSDK_AUTOPILOT_UNKNOWN = 0,
    MAVSDK_AUTOPILOT_PX4 = 1,
    MAVSDK_AUTOPILOT_ARDUPILOT = 2,
    MAVSDK_AUTOPILOT_GENERIC = 3
} mavsdk_autopilot_t;

#ifdef __cplusplus
}
#endif

#endif // CMAVSDK_AUTOPILOT_H
