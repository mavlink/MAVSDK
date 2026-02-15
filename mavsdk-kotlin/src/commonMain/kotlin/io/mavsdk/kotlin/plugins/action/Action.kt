package io.mavsdk.kotlin.plugins.action

import io.mavsdk.kotlin.System
import kotlinx.coroutines.suspendCancellableCoroutine
import kotlin.coroutines.resume
import kotlin.coroutines.resumeWithException

/**
 * Action plugin for simple vehicle actions like arming, taking off, and landing
 */
class Action internal constructor(private val handle: Long) : AutoCloseable {

    /**
     * Result codes for action requests
     */
    enum class Result(val value: Int) {
        UNKNOWN(0),
        SUCCESS(1),
        NO_SYSTEM(2),
        CONNECTION_ERROR(3),
        BUSY(4),
        COMMAND_DENIED(5),
        COMMAND_DENIED_LANDED_STATE_UNKNOWN(6),
        COMMAND_DENIED_NOT_LANDED(7),
        TIMEOUT(8),
        VTOL_TRANSITION_SUPPORT_UNKNOWN(9),
        NO_VTOL_TRANSITION_SUPPORT(10),
        PARAMETER_ERROR(11),
        UNSUPPORTED(12),
        FAILED(13),
        INVALID_ARGUMENT(14);

        companion object {
            fun fromValue(value: Int): Result =
                entries.find { it.value == value } ?: UNKNOWN
        }
    }

    /**
     * Arm the vehicle (blocking)
     *
     * @return Result of the arm operation
     */
    fun arm(): Result {
        val resultValue = armBlocking()
        return Result.fromValue(resultValue)
    }

    /**
     * Arm the vehicle (suspending)
     *
     * @return Result of the arm operation
     * @throws ActionException if the operation fails
     */
    suspend fun armAsync(): Result = suspendCancellableCoroutine { continuation ->
        val callback = ArmCallback { result ->
            val actionResult = Result.fromValue(result)
            if (actionResult == Result.SUCCESS) {
                continuation.resume(actionResult)
            } else {
                continuation.resumeWithException(
                    ActionException(actionResult, "Arm failed: ${actionResult.name}")
                )
            }
        }
        armAsyncNative(callback)
    }

    private fun interface ArmCallback {
        fun invoke(result: Int)
    }

    private external fun armBlocking(): Int
    private external fun armAsyncNative(callback: ArmCallback)
    private external fun destroy()

    override fun close() {
        destroy()
    }

    /**
     * Exception thrown when an action operation fails
     */
    class ActionException(
        val result: Result,
        message: String
    ) : Exception(message)

    companion object {
        /**
         * Create Action plugin instance
         *
         * @param system The system to control
         */
        fun create(system: System): Action {
            val handle = createNative(system.getHandle())
            return Action(handle)
        }

        private external fun createNative(systemHandle: Long): Long
    }
}
