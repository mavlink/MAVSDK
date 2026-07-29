package io.mavsdk.kotlin

import io.mavsdk.jni.NativeMavsdk
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow

actual class Mavsdk actual constructor(configuration: Configuration) : AutoCloseable {

    private val handle: Long = NativeMavsdk.create(configuration.getHandle())
    private var destroyed = false

    actual fun version(): String = NativeMavsdk.version(handle)

    actual fun addAnyConnection(connectionUrl: String): Result<Unit> {
        val result = NativeMavsdk.addAnyConnection(handle, connectionUrl)
        return if (result == ConnectionResult.SUCCESS.value) {
            Result.success(Unit)
        } else {
            Result.failure(
                MavsdkError.ConnectionError(ConnectionResult.fromValue(result))
            )
        }
    }

    actual fun addAnyConnectionWithHandle(connectionUrl: String): Result<Long> {
        val nativeResult = NativeMavsdk.addAnyConnectionWithHandle(handle, connectionUrl)
        val resultCode = nativeResult[0].toInt()
        val connectionHandle = nativeResult[1]

        return if (resultCode == ConnectionResult.SUCCESS.value) {
            Result.success(connectionHandle)
        } else {
            Result.failure(
                MavsdkError.ConnectionError(ConnectionResult.fromValue(resultCode))
            )
        }
    }

    actual fun removeConnection(handle: Long) = NativeMavsdk.removeConnection(this.handle, handle)
    actual fun systemCount(): Int = NativeMavsdk.systemCount(handle)
    actual fun getSystems(): List<System> = NativeMavsdk.getSystems(handle).map(::System)
    actual fun firstAutopilot(timeoutSeconds: Double): System? =
        NativeMavsdk.firstAutopilot(handle, timeoutSeconds).takeIf { it != 0L }?.let(::System)

    actual fun serverComponentHandle(instance: Int): Long =
        NativeMavsdk.serverComponentHandle(handle, instance)

    actual fun subscribeOnNewSystem(): Flow<System> = callbackFlow {
        val subHandle = NativeMavsdk.subscribeOnNewSystem(
            handle,
            NativeMavsdk.NewSystemCallback { trySend(System(it)) }
        )
        awaitClose { NativeMavsdk.unsubscribeOnNewSystem(handle, subHandle) }
    }

    actual override fun close() {
        if (!destroyed) {
            getSystems().forEach { it.closePlugins() }
            NativeMavsdk.destroy(handle)
            destroyed = true
        }
    }

    internal fun getHandle(): Long = handle

    companion object {
        init {
            NativeLibraryLoader.loadLibrary()
        }
    }
}
