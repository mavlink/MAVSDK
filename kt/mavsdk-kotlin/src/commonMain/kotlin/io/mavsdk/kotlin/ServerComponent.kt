package io.mavsdk.kotlin

/**
 * A server-side MAVLink component used by server plugins to handle incoming requests.
 *
 * Plugin accessors such as `serverComponent.cameraServer` are inherited from
 * [ServerComponentPlugins], which is generated from the proto definitions.
 */
expect class ServerComponent : ServerComponentPlugins {
    internal fun close()
}
