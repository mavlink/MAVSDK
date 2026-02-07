package io.mavsdk.kotlin

import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flow

actual class System internal constructor(private val handle: Long) {

    actual external fun hasAutopilot(): Boolean
    actual external fun isStandalone(): Boolean
    actual external fun hasCamera(cameraId: Int): Boolean
    actual external fun hasGimbal(): Boolean
    actual external fun isConnected(): Boolean
    actual external fun getSystemId(): Int
    actual external fun getComponentIds(): IntArray

    actual fun getAutopilotType(): Autopilot {
        val value = getAutopilotTypeNative()
        return Autopilot.fromValue(value)
    }

    actual fun getVehicleType(): Vehicle {
        val value = getVehicleTypeNative()
        return Vehicle.fromValue(value)
    }

    actual fun subscribeIsConnected(): Flow<Boolean> {
        // TODO: Implement with proper callback handling
        return flow {
            // Placeholder
        }
    }

    actual external fun enableTimesync()

    private external fun getAutopilotTypeNative(): Int
    private external fun getVehicleTypeNative(): Int

    internal fun getHandle(): Long = handle

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is System) return false
        return handle == other.handle
    }

    override fun hashCode(): Int = handle.hashCode()

    override fun toString(): String = "System(systemId=${getSystemId()}, handle=$handle)"
}
