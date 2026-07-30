package io.mavsdk.kotlin

/** Autopilot types */
enum class Autopilot(val value: Int) {
    UNKNOWN(0),
    PX4(1),
    ARDUPILOT(2),
    GENERIC(3);

    companion object {
        fun fromValue(value: Int): Autopilot = entries.find { it.value == value } ?: UNKNOWN
    }
}
