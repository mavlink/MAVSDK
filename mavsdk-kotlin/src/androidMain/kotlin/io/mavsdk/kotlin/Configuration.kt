package io.mavsdk.kotlin

actual class Configuration private constructor(private val handle: Long) : AutoCloseable {

    actual var systemId: Int
        get() = getSystemIdNative()
        set(value) = setSystemIdNative(value)

    actual var componentId: Int
        get() = getComponentIdNative()
        set(value) = setComponentIdNative(value)

    private external fun getSystemIdNative(): Int
    private external fun setSystemIdNative(value: Int)
    private external fun getComponentIdNative(): Int
    private external fun setComponentIdNative(value: Int)
    private external fun destroy()

    actual override fun close() {
        destroy()
    }

    actual internal fun getHandle(): Long = handle

    actual companion object {
        init {
            NativeLibraryLoader.loadLibrary()
        }

        @JvmStatic
        actual external fun createWithComponentType(componentType: ComponentType): Configuration

        @JvmStatic
        actual external fun createManual(
            systemId: Int,
            componentId: Int,
            alwaysSendHeartbeats: Boolean
        ): Configuration
    }
}
