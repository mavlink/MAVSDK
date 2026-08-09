package io.mavsdk.kotlin

/** Configuration for MAVSDK instance */
expect class Configuration : AutoCloseable {
    /** System ID for this MAVSDK instance */
    var systemId: Int

    /** Component ID for this MAVSDK instance */
    var componentId: Int

    override fun close()

    internal fun getHandle(): Long

    companion object {
        /**
         * Create configuration with component type
         *
         * @param componentType Type of component
         */
        fun createWithComponentType(componentType: ComponentType): Configuration

        /**
         * Create manual configuration
         *
         * @param systemId System ID
         * @param componentId Component ID
         * @param alwaysSendHeartbeats Whether to always send heartbeats
         */
        fun createManual(
            systemId: Int,
            componentId: Int,
            alwaysSendHeartbeats: Boolean,
        ): Configuration
    }
}
