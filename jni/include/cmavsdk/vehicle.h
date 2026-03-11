#ifndef CMAVSDK_VEHICLE_H
#define CMAVSDK_VEHICLE_H

#include "mavsdk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    MAVSDK_VEHICLE_UNKNOWN = 0,
    MAVSDK_VEHICLE_GENERIC = 1,
    MAVSDK_VEHICLE_FIXED_WING = 2,
    MAVSDK_VEHICLE_MULTICOPTER = 3,
    MAVSDK_VEHICLE_ROVER = 4,
    MAVSDK_VEHICLE_SUBMARINE = 5
} mavsdk_vehicle_t;

#ifdef __cplusplus
}
#endif

#endif // CMAVSDK_VEHICLE_H
