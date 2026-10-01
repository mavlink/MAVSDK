package io.mavsdk.kotlin

import io.mavsdk.jni.NativeConfiguration
import java.util.concurrent.locks.ReentrantReadWriteLock
import kotlin.concurrent.read
import kotlin.concurrent.write

actual class Configuration private constructor(private val handle: Long) : AutoCloseable {

    // See the note on Mavsdk.lifecycle: the read lock keeps `handle` alive across a
    // native call, close() takes the write lock and so waits for in-flight ones.
    private val lifecycle = ReentrantReadWriteLock()
    @Volatile private var closed = false

    actual var systemId: Int
        get() = withOpen { NativeConfiguration.getSystemId(handle) }
        set(value) = withOpen { NativeConfiguration.setSystemId(handle, value) }

    actual var componentId: Int
        get() = withOpen { NativeConfiguration.getComponentId(handle) }
        set(value) = withOpen { NativeConfiguration.setComponentId(handle, value) }

    actual override fun close() {
        // Claimed under the write lock so that exactly one caller destroys the handle.
        // close() is reachable twice over -- from `use` and from a caller tidying up --
        // and the second call would otherwise free it again.
        lifecycle.write {
            if (closed) return
            closed = true
        }
        NativeConfiguration.destroy(handle)
    }

    actual internal fun getHandle(): Long = withOpen { handle }

    /** Runs [action] under the read lock, failing fast if this configuration is closed. */
    private inline fun <Value> withOpen(action: () -> Value): Value = lifecycle.read {
        check(!closed) { "Configuration is closed" }
        action()
    }

    actual companion object {
        init {
            NativeLibraryLoader.loadLibrary()
        }

        actual fun createWithComponentType(componentType: ComponentType): Configuration =
            Configuration(NativeConfiguration.createWithComponentType(componentType.value))

        actual fun createManual(
            systemId: Int,
            componentId: Int,
            alwaysSendHeartbeats: Boolean,
        ): Configuration =
            Configuration(
                NativeConfiguration.createManual(systemId, componentId, alwaysSendHeartbeats)
            )
    }
}
