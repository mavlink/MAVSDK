package io.mavsdk.kotlin

import io.mavsdk.jni.NativeMavsdk
import java.util.concurrent.locks.ReentrantReadWriteLock
import kotlin.concurrent.read
import kotlin.concurrent.write
import kotlin.reflect.KClass

actual class ServerComponent internal constructor(private val handle: Long) :
    ServerComponentPlugins() {

    private class OwnedPlugin(val value: Any, val destroy: () -> Unit)

    // See the note on Mavsdk.lifecycle for why these are two separate locks.
    private val lifecycle = ReentrantReadWriteLock()
    private val cacheLock = Any()
    private val plugins = linkedMapOf<KClass<*>, OwnedPlugin>()
    @Volatile private var closed = false

    // Inherited from ServerComponentPlugins, so `override` rather than `actual`.
    override fun getHandle(): Long = handle

    override fun <Plugin : Any> plugin(
        pluginClass: KClass<Plugin>,
        create: () -> Plugin,
        destroy: (Plugin) -> Unit,
    ): Plugin = lifecycle.read {
        check(!closed) { "ServerComponent is closed" }
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
        val actions = lifecycle.write {
            if (closed) return
            closed = true
            synchronized(cacheLock) {
                val pluginActions = plugins.values.toList().asReversed().map { it.destroy }
                plugins.clear()
                pluginActions + { NativeMavsdk.destroyServerComponent(handle) }
            }
        }
        closeAll(actions)
    }
}
