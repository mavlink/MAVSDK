package io.mavsdk.kotlin

import io.mavsdk.jni.NativeConfiguration

actual class Configuration private constructor(private val handle: Long) : AutoCloseable {

    actual var systemId: Int
        get() = NativeConfiguration.getSystemId(handle)
        set(value) = NativeConfiguration.setSystemId(handle, value)

    actual var componentId: Int
        get() = NativeConfiguration.getComponentId(handle)
        set(value) = NativeConfiguration.setComponentId(handle, value)

    actual override fun close() {
        NativeConfiguration.destroy(handle)
    }

    actual internal fun getHandle(): Long = handle

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
