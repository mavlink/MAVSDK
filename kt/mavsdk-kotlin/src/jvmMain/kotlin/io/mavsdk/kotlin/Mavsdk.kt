package io.mavsdk.kotlin

import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow

actual class Mavsdk actual constructor(configuration: Configuration) : AutoCloseable {

    private val handle: Long = create(configuration.getHandle())
    private var destroyed = false

    actual external fun version(): String

    actual fun addAnyConnection(connectionUrl: String): Result<Unit> {
        val result = addAnyConnectionNative(connectionUrl)
        return if (result == ConnectionResult.SUCCESS.value) {
            Result.success(Unit)
        } else {
            Result.failure(
                MavsdkError.ConnectionError(ConnectionResult.fromValue(result))
            )
        }
    }

    actual fun addAnyConnectionWithHandle(connectionUrl: String): Result<Long> {
        val resultPair = addAnyConnectionWithHandleNative(connectionUrl)
        val resultCode = resultPair.first
        val handle = resultPair.second

        return if (resultCode == ConnectionResult.SUCCESS.value) {
            Result.success(handle)
        } else {
            Result.failure(
                MavsdkError.ConnectionError(ConnectionResult.fromValue(resultCode))
            )
        }
    }

    actual external fun removeConnection(handle: Long)
    actual external fun systemCount(): Int
    actual external fun getSystems(): List<System>
    actual external fun firstAutopilot(timeoutSeconds: Double): System?

    actual fun serverComponentHandle(instance: Int): Long =
        serverComponentHandleNative(instance)

    actual fun subscribeOnNewSystem(): Flow<System> = callbackFlow {
        val subHandle = subscribeOnNewSystemNative(NewSystemCallback { trySend(it) })
        awaitClose { unsubscribeOnNewSystem(subHandle) }
    }

    private fun interface NewSystemCallback { fun invoke(system: System) }

    private external fun create(configHandle: Long): Long
    private external fun serverComponentHandleNative(instance: Int): Long
    private external fun addAnyConnectionNative(connectionUrl: String): Int
    private external fun addAnyConnectionWithHandleNative(connectionUrl: String): Pair<Int, Long>
    private external fun subscribeOnNewSystemNative(callback: NewSystemCallback): Long
    private external fun unsubscribeOnNewSystem(handle: Long)
    private external fun destroy()

    actual override fun close() {
        if (!destroyed) {
            destroy()
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
