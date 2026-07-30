package io.mavsdk.kotlin.exampleapp

import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flow

/**
 * Placeholder until `mavsdk-kotlin` gains an iOS target. See `KOTLIN_IOS_PLAN.md`:
 * iOS will bind the C API through cinterop rather than JNI, so this file is what
 * gets a real implementation at the end of that work.
 */
actual fun flyMission(connectionUrl: String): Flow<String> = flow {
    emit("MAVSDK is not available on iOS yet.")
    emit("mavsdk-kotlin currently ships jvm and android targets only.")
    emit("Tracking: KOTLIN_IOS_PLAN.md")
}
