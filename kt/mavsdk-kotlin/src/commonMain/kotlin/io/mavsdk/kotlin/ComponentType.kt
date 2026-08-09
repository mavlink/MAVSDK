package io.mavsdk.kotlin

/** MAVLink component types */
enum class ComponentType(val value: Int) {
    AUTOPILOT(0),
    GROUND_STATION(1),
    COMPANION_COMPUTER(2),
    CAMERA(3),
    GIMBAL(4),
    REMOTE_ID(5),
    CUSTOM(6);

    companion object {
        fun fromValue(value: Int): ComponentType = entries.find { it.value == value } ?: CUSTOM
    }
}
