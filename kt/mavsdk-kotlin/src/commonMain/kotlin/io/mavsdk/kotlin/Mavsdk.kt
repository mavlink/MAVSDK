package io.mavsdk.kotlin

import kotlinx.coroutines.flow.Flow

/**
 * Main MAVSDK class for connecting to and managing MAVLink systems
 * 
 * Example usage:
 * ```kotlin
 * Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
 *     mavsdk.addAnyConnection("udp://:14540")
 *         .onSuccess { println("Connected!") }
 *         .onFailure { error -> println("Connection failed: $error") }
 *
 *     val system = mavsdk.firstAutopilot() ?: return@use
 *     val action = Action.create(system)  // closed automatically when mavsdk closes
 *     action.armAsync()
 * }
 * ```
 */
expect class Mavsdk(configuration: Configuration) : AutoCloseable {
    /**
     * Get MAVSDK version string
     */
    fun version(): String

    /**
     * Add a connection
     * 
     * @param connectionUrl Connection URL (e.g., "udp://:14540", "serial:///dev/ttyUSB0:57600")
     * @return Result with ConnectionResult
     */
    fun addAnyConnection(connectionUrl: String): Result<Unit>

    /**
     * Add a connection and get handle
     * 
     * @param connectionUrl Connection URL
     * @return Result with connection handle
     */
    fun addAnyConnectionWithHandle(connectionUrl: String): Result<Long>

    /**
     * Remove a connection
     * 
     * @param handle Connection handle from addAnyConnectionWithHandle
     */
    fun removeConnection(handle: Long)

    /**
     * Get number of discovered systems
     */
    fun systemCount(): Int

    /**
     * Get all discovered systems
     */
    fun getSystems(): List<System>

    /**
     * Wait for and return first autopilot system
     * 
     * @param timeoutSeconds Timeout in seconds (default: 3.0)
     * @return First autopilot system, or null if timeout
     */
    fun firstAutopilot(timeoutSeconds: Double = 3.0): System?

    /**
     * Subscribe to new system discoveries
     * 
     * @return Flow emitting newly discovered systems
     */
    fun subscribeOnNewSystem(): Flow<System>

    /**
     * Get the server component handle for creating server-side plugins
     * (CameraServer, ActionServer, TelemetryServer, etc.)
     *
     * @param instance Server component instance number (default: 1)
     * @return Raw server component handle to pass to server plugin create()
     */
    fun serverComponentHandle(instance: Int = 1): Long

    override fun close()
}
