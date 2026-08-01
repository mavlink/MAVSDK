package io.mavsdk.kotlin

import io.mavsdk.jni.NativeSystem
import java.util.concurrent.locks.ReentrantReadWriteLock
import kotlin.concurrent.read
import kotlin.concurrent.write
import kotlin.reflect.KClass
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow

// `systemId` is captured at construction so toString() stays usable after close();
// getSystemId() still reads it back natively for callers who want the live value.
actual class System internal constructor(private val handle: Long, private val systemId: Int) :
    SystemPlugins() {

    private class OwnedPlugin(val value: Any, val destroy: () -> Unit)

    // See the note on Mavsdk.lifecycle: `lifecycle` keeps `handle` alive across a
    // native call and lets independent calls run concurrently, while `cacheLock`
    // guards only the collections and is never held across a native call.
    private val lifecycle = ReentrantReadWriteLock()
    private val cacheLock = Any()
    private val plugins = linkedMapOf<KClass<*>, OwnedPlugin>()
    private val subscriptions = linkedSetOf<Long>()
    @Volatile private var closed = false

    // Inherited from SystemPlugins, so `override` rather than `actual`: only members
    // declared in the expect class itself are actualised.
    override fun <Plugin : Any> plugin(
        pluginClass: KClass<Plugin>,
        create: () -> Plugin,
        destroy: (Plugin) -> Unit,
    ): Plugin = withOpen {
        synchronized(cacheLock) {
            @Suppress("UNCHECKED_CAST")
            plugins[pluginClass]?.let {
                return@synchronized it.value as Plugin
            }

            create().also { plugin ->
                plugins[pluginClass] = OwnedPlugin(plugin) { destroy(plugin) }
            }
        }
    }

    actual internal fun close() {
        // The write lock waits for in-flight operations, so nothing is using `handle`
        // by the time the teardown below runs.
        val actions = lifecycle.write {
            if (closed) return
            closed = true
            synchronized(cacheLock) {
                val subscriptionActions =
                    subscriptions.toList().asReversed().map {
                        { NativeSystem.unsubscribeIsConnected(handle, it) }
                    }
                val pluginActions = plugins.values.toList().asReversed().map { it.destroy }
                subscriptions.clear()
                plugins.clear()
                // Plugins hold their own shared_ptr copy of the C++ System, so
                // destroying them before the system is ordering hygiene, not a
                // lifetime requirement.
                subscriptionActions + pluginActions + { NativeSystem.destroy(handle) }
            }
        }
        closeAll(actions)
    }

    actual fun hasAutopilot(): Boolean = withOpen { NativeSystem.hasAutopilot(handle) }

    actual fun isStandalone(): Boolean = withOpen { NativeSystem.isStandalone(handle) }

    actual fun hasCamera(cameraId: Int): Boolean = withOpen {
        NativeSystem.hasCamera(handle, cameraId)
    }

    actual fun hasGimbal(): Boolean = withOpen { NativeSystem.hasGimbal(handle) }

    actual fun isConnected(): Boolean = withOpen { NativeSystem.isConnected(handle) }

    actual fun getSystemId(): Int = withOpen { NativeSystem.getSystemId(handle) }

    actual fun getComponentIds(): IntArray = withOpen { NativeSystem.getComponentIds(handle) }

    actual fun getAutopilotType(): Autopilot = withOpen {
        val value = NativeSystem.getAutopilotType(handle)
        Autopilot.fromValue(value)
    }

    actual fun getVehicleType(): Vehicle = withOpen {
        val value = NativeSystem.getVehicleType(handle)
        Vehicle.fromValue(value)
    }

    actual fun subscribeIsConnected(): Flow<Boolean> = callbackFlow {
        val subscriptionHandle = withOpen {
            NativeSystem.subscribeIsConnected(handle, NativeSystem.BooleanCallback { trySend(it) })
                .also { synchronized(cacheLock) { subscriptions.add(it) } }
        }
        awaitClose {
            val unsubscribe = synchronized(cacheLock) { subscriptions.remove(subscriptionHandle) }
            // Under the read lock, so close() cannot destroy the handle underneath us.
            if (unsubscribe)
                withOpenOrIgnore { NativeSystem.unsubscribeIsConnected(handle, subscriptionHandle) }
        }
    }

    actual fun enableTimesync() = withOpen { NativeSystem.enableTimesync(handle) }

    override fun getHandle(): Long = handle

    /** Runs [action] under the read lock, failing fast if this system is closed. */
    private inline fun <Value> withOpen(action: () -> Value): Value = lifecycle.read {
        check(!closed) { "System is closed" }
        action()
    }

    /** Runs [action] under the read lock, skipping it entirely if already closed. */
    private inline fun withOpenOrIgnore(action: () -> Unit) {
        lifecycle.read { if (!closed) action() }
    }

    override fun equals(other: Any?): Boolean {
        if (this === other) return true
        if (other !is System) return false
        return handle == other.handle
    }

    override fun hashCode(): Int = handle.hashCode()

    // Deliberately does not call getSystemId(): toString() must stay safe to call on a
    // closed system, because debuggers and log statements reach for it precisely when
    // things are being torn down.
    override fun toString(): String =
        if (closed) "System(systemId=$systemId, handle=$handle, closed)"
        else "System(systemId=$systemId, handle=$handle)"
}
