package io.mavsdk.kotlin

/** Message forwarding options */
enum class ForwardingOption(val value: Int) {
    OFF(0),
    ON(1);

    companion object {
        fun fromValue(value: Int): ForwardingOption = entries.find { it.value == value } ?: OFF
    }
}
