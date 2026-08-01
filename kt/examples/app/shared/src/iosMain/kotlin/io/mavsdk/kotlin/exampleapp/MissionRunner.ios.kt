package io.mavsdk.kotlin.exampleapp

import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flow

/**
 * Placeholder until `mavsdk-kotlin` gains an iOS target. See `KOTLIN_IOS_PLAN.md`:
 * iOS will bind the C API through cinterop rather than JNI, so this file is what
 * gets a real implementation at the end of that work.
 *
 * The map itself is common code and works here already -- it just never receives
 * a [MissionEvent.Telemetry] to put a marker at.
 */
actual fun flyMission(connectionUrl: String): Flow<MissionEvent> = flow {
    emit(MissionEvent.Log("MAVSDK is not available on iOS yet."))
    emit(MissionEvent.Log("mavsdk-kotlin currently ships jvm and android targets only."))
    emit(MissionEvent.Log("Tracking: KOTLIN_IOS_PLAN.md"))
}
