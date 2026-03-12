package io.mavsdk.kotlin.plugins.telemetry

import io.mavsdk.kotlin.System
import kotlinx.coroutines.channels.awaitClose
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.callbackFlow

/**
 * Telemetry plugin for receiving vehicle telemetry data
 */
class Telemetry internal constructor(private val handle: Long) : AutoCloseable {

    /**
     * Position in global coordinates
     */
    data class Position(
        val latitudeDeg: Double,
        val longitudeDeg: Double,
        val absoluteAltitudeM: Float,
        val relativeAltitudeM: Float
    )

    /**
     * Subscribe to position updates
     *
     * @return Flow emitting position updates
     */
    fun subscribePosition(): Flow<Position> = callbackFlow {
        val callback = PositionCallback { position ->
            trySend(position)
        }

        val subscriptionHandle = subscribePositionNative(callback)

        awaitClose {
            unsubscribePosition(subscriptionHandle)
        }
    }

    private fun interface PositionCallback {
        fun invoke(position: Position)
    }

    private external fun subscribePositionNative(callback: PositionCallback): Long
    private external fun unsubscribePosition(handle: Long)
    private external fun destroy()

    override fun close() {
        destroy()
    }

    companion object {
        /**
         * Create Telemetry plugin instance
         *
         * @param system The system to get telemetry from
         */
        fun create(system: System): Telemetry {
            val handle = createNative(system.getHandle())
            return Telemetry(handle)
        }

        private external fun createNative(systemHandle: Long): Long
    }
}
