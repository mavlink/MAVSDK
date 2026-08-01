package io.mavsdk.kotlin

import io.mavsdk.jni.NativeMavsdk
import io.mavsdk.jni.NativeSystem
import java.util.concurrent.locks.ReentrantReadWriteLock
import kotlin.concurrent.read
import kotlin.concurrent.write
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow

actual class Mavsdk actual constructor(configuration: Configuration) : AutoCloseable {

    private val handle: Long = NativeMavsdk.create(configuration.getHandle())

    // Two locks with distinct jobs, and they must not be conflated.
    //
    // `lifecycle` keeps the native handle alive for the duration of a call: every
    // operation takes the read lock, close() takes the write lock. Because acquiring
    // the write lock waits for in-flight readers, no native call can be running when
    // `destroyed` flips -- that is what makes use-after-free impossible.
    //
    // `cacheLock` guards only the maps below, and is never held across a native call.
    //
    // Readers must not exclude each other. MAVSDK dispatches every user callback on a
    // single thread (MavsdkImpl::process_user_callbacks_thread), and first_autopilot()
    // waits on a promise that its own on-new-system callback fulfils. If a blocking
    // call like firstAutopilot() held an exclusive lock, the subscribeOnNewSystem()
    // callback would block that one dispatch thread, first_autopilot()'s callback
    // would never run, and it would spin until its timeout and return null.
    private val lifecycle = ReentrantReadWriteLock()
    private val cacheLock = Any()
    private val systems = linkedMapOf<Int, System>()
    private val serverComponents = linkedMapOf<Int, ServerComponent>()
    private val subscriptions = linkedSetOf<Long>()
    @Volatile private var destroyed = false

    actual fun version(): String = withOpen { NativeMavsdk.version(handle) }

    actual fun addAnyConnection(connectionUrl: String): Result<Unit> = withOpen {
        val result = NativeMavsdk.addAnyConnection(handle, connectionUrl)
        if (result == ConnectionResult.SUCCESS.value) {
            Result.success(Unit)
        } else {
            Result.failure(MavsdkError.ConnectionError(ConnectionResult.fromValue(result)))
        }
    }

    actual fun addAnyConnectionWithHandle(connectionUrl: String): Result<Long> = withOpen {
        val nativeResult = NativeMavsdk.addAnyConnectionWithHandle(handle, connectionUrl)
        val resultCode = nativeResult[0].toInt()
        val connectionHandle = nativeResult[1]

        if (resultCode == ConnectionResult.SUCCESS.value) {
            Result.success(connectionHandle)
        } else {
            Result.failure(MavsdkError.ConnectionError(ConnectionResult.fromValue(resultCode)))
        }
    }

    actual fun removeConnection(handle: Long) = withOpen {
        NativeMavsdk.removeConnection(this.handle, handle)
    }

    actual fun systemCount(): Int = withOpen { NativeMavsdk.systemCount(handle) }

    actual fun getSystems(): List<System> = withOpen {
        NativeMavsdk.getSystems(handle).map(::adoptSystem)
    }

    actual fun firstAutopilot(timeoutSeconds: Double): System? = withOpen {
        NativeMavsdk.firstAutopilot(handle, timeoutSeconds).takeIf { it != 0L }?.let(::adoptSystem)
    }

    actual fun serverComponent(instance: Int): ServerComponent = withOpen {
        synchronized(cacheLock) {
            serverComponents.getOrPut(instance) {
                val componentHandle = NativeMavsdk.serverComponentHandle(handle, instance)
                check(componentHandle != 0L) {
                    "MAVSDK returned no server component for instance $instance"
                }
                ServerComponent(componentHandle)
            }
        }
    }

    actual fun subscribeOnNewSystem(): Flow<System> = callbackFlow {
        val subHandle = withOpen {
            NativeMavsdk.subscribeOnNewSystem(
                    handle,
                    // Runs on MAVSDK's single user-callback thread. getSystems() takes
                    // the read lock, so it must never contend with another operation
                    // holding an exclusive lock -- see the note on `lifecycle`.
                    NativeMavsdk.NewSystemCallback {
                        if (!destroyed) getSystems().lastOrNull()?.let { trySend(it) }
                    },
                )
                .also { synchronized(cacheLock) { subscriptions.add(it) } }
        }
        awaitClose {
            val unsubscribe = synchronized(cacheLock) { subscriptions.remove(subHandle) }
            // Under the read lock, so close() cannot destroy the handle underneath us.
            // If close() got there first it already unsubscribed, so this is a no-op.
            if (unsubscribe)
                withOpenOrIgnore { NativeMavsdk.unsubscribeOnNewSystem(handle, subHandle) }
        }
    }

    actual override fun close() {
        // The write lock waits for every in-flight operation to finish, so once
        // `destroyed` is set no native call can still be running against `handle`.
        val actions = lifecycle.write {
            if (destroyed) return
            destroyed = true
            synchronized(cacheLock) {
                val subscriptionActions =
                    subscriptions.toList().asReversed().map {
                        { NativeMavsdk.unsubscribeOnNewSystem(handle, it) }
                    }
                val componentActions =
                    serverComponents.values.toList().asReversed().map { { it.close() } }
                val systemActions = systems.values.toList().asReversed().map { { it.close() } }
                subscriptions.clear()
                serverComponents.clear()
                systems.clear()
                subscriptionActions +
                    componentActions +
                    systemActions +
                    {
                        NativeMavsdk.destroy(handle)
                    }
            }
        }
        // Run the teardown outside both locks: `destroyed` is already set, so every
        // other entry point fails fast rather than racing us.
        closeAll(actions)
    }

    // Only ever called from inside withOpen, so the read lock is already held and the
    // handle cannot be destroyed underneath us.
    private fun adoptSystem(systemHandle: Long): System {
        val systemId = NativeSystem.getSystemId(systemHandle)
        return synchronized(cacheLock) {
            // mavsdk_get_systems hands out a fresh shared_ptr per call, so a handle for
            // a system we already wrap is a duplicate reference that has to be released.
            systems[systemId]?.also { NativeSystem.destroy(systemHandle) }
                ?: System(systemHandle, systemId).also { systems[systemId] = it }
        }
    }

    /** Runs [action] under the read lock, failing fast if this instance is closed. */
    private inline fun <Value> withOpen(action: () -> Value): Value = lifecycle.read {
        check(!destroyed) { "Mavsdk is closed" }
        action()
    }

    /** Runs [action] under the read lock, skipping it entirely if already closed. */
    private inline fun withOpenOrIgnore(action: () -> Unit) {
        lifecycle.read { if (!destroyed) action() }
    }

    internal fun getHandle(): Long = handle

    companion object {
        init {
            NativeLibraryLoader.loadLibrary()
        }
    }
}
