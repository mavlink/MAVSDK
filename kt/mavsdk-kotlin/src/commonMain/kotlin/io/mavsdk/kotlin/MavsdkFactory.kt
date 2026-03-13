package io.mavsdk.kotlin

/**
 * Factory function that creates a [Mavsdk] instance from a [ComponentType],
 * handling [Configuration] construction and teardown internally.
 *
 * The returned [Mavsdk] must still be closed by the caller (e.g. via [use]).
 *
 * Usage:
 * ```kotlin
 * Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
 *     mavsdk.addAnyConnection("udp://:14540")
 *     ...
 * }
 * ```
 */
fun Mavsdk(componentType: ComponentType): Mavsdk {
    val config = Configuration.createWithComponentType(componentType)
    val mavsdk = Mavsdk(config)
    config.close()
    return mavsdk
}
