package io.mavsdk.kotlin

import io.mavsdk.jni.NativeSystem
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow

actual class System internal constructor(private val handle: Long) {

    private val plugins = mutableListOf<AutoCloseable>()

    actual internal fun registerPlugin(plugin: AutoCloseable) {
        plugins.add(plugin)
    }

    actual internal fun closePlugins() {
        plugins.reversed().forEach { it.close() }
        plugins.clear()
    }

    actual fun hasAutopilot(): Boolean = NativeSystem.hasAutopilot(handle)

    actual fun isStandalone(): Boolean = NativeSystem.isStandalone(handle)

    actual fun hasCamera(cameraId: Int): Boolean = NativeSystem.hasCamera(handle, cameraId)

    actual fun hasGimbal(): Boolean = NativeSystem.hasGimbal(handle)

    actual fun isConnected(): Boolean = NativeSystem.isConnected(handle)

    actual fun getSystemId(): Int = NativeSystem.getSystemId(handle)

    actual fun getComponentIds(): IntArray = NativeSystem.getComponentIds(handle)

    actual fun getAutopilotType(): Autopilot {
        val value = NativeSystem.getAutopilotType(handle)
        return Autopilot.fromValue(value)
    }

    actual fun getVehicleType(): Vehicle {
        val value = NativeSystem.getVehicleType(handle)
        return Vehicle.fromValue(value)
    }

    actual fun subscribeIsConnected(): Flow<Boolean> = callbackFlow {
        val subscriptionHandle =
            NativeSystem.subscribeIsConnected(handle, NativeSystem.BooleanCallback { trySend(it) })
        awaitClose { NativeSystem.unsubscribeIsConnected(handle, subscriptionHandle) }
    }

    actual fun enableTimesync() = NativeSystem.enableTimesync(handle)

    actual internal fun getHandle(): Long = handle

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is System) return false
        return handle == other.handle
    }

    override fun hashCode(): Int = handle.hashCode()

    override fun toString(): String = "System(systemId=${getSystemId()}, handle=$handle)"
}
