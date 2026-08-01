package io.mavsdk.kotlin

import kotlinx.coroutines.flow.Flow

/**
 * Represents a MAVLink system (e.g., drone, ground station)
 *
 * Plugin accessors such as `system.action` and `system.telemetry` are inherited from
 * [SystemPlugins], which is generated from the proto definitions.
 */
expect class System : SystemPlugins {
    /** Check if system has autopilot */
    fun hasAutopilot(): Boolean

    /** Check if system is standalone */
    fun isStandalone(): Boolean

    /**
     * Check if system has camera
     *
     * @param cameraId Camera ID to check, or -1 for any camera
     */
    fun hasCamera(cameraId: Int = -1): Boolean

    /** Check if system has gimbal */
    fun hasGimbal(): Boolean

    /** Check if system is connected */
    fun isConnected(): Boolean

    /** Get system ID */
    fun getSystemId(): Int

    /** Get list of component IDs */
    fun getComponentIds(): IntArray

    /** Get autopilot type */
    fun getAutopilotType(): Autopilot

    /** Get vehicle type */
    fun getVehicleType(): Vehicle

    /**
     * Subscribe to connection state changes
     *
     * @return Flow emitting connection state (true when connected, false when disconnected)
     */
    fun subscribeIsConnected(): Flow<Boolean>

    /** Enable time synchronization */
    fun enableTimesync()

    /** Close all plugins and release the native system handle. */
    internal fun close()
}
