package io.mavsdk.kotlin

/**
 * Vehicle types
 */
enum class Vehicle(val value: Int) {
    UNKNOWN(0),
    GENERIC(1),
    FIXED_WING(2),
    MULTICOPTER(3),
    ROVER(4),
    SUBMARINE(5);

    companion object {
        fun fromValue(value: Int): Vehicle =
            entries.find { it.value == value } ?: UNKNOWN
    }
}
