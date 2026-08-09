package io.mavsdk.kotlin.exampleapp

import kotlinx.coroutines.flow.Flow

/** Something the running mission wants the UI to show. */
sealed interface MissionEvent {
    /** A progress line, appended to the log. */
    data class Log(val line: String) : MissionEvent

    /**
     * Where the drone is now, for the map marker.
     *
     * [relativeAltitudeM] is height above the launch point rather than above sea
     * level, which is the one that means something to a pilot watching a flight.
     *
     * [headingDeg] is null until the vehicle reports one: position and heading are
     * separate MAVLink streams, and position usually arrives first.
     */
    data class Telemetry(
        val latitude: Double,
        val longitude: Double,
        val relativeAltitudeM: Float,
        val headingDeg: Float?,
    ) : MissionEvent
}

/**
 * Runs the same waypoint mission as the `FlyMission` example in `examples/cli-jvm`,
 * emitting progress and position instead of printing to stdout.
 *
 * The work happens while the flow is collected, off the main thread: cancelling
 * the collection cancels the run and closes the MAVSDK instance. Position comes
 * from the same connection as the mission, so one collection drives both the log
 * and the map.
 *
 * This is an `expect` rather than common code because the implementation needs
 * `mavsdk-kotlin`, which currently has an android variant but no iOS one — see
 * `KOTLIN_IOS_PLAN.md`. Once it has one, the Android implementation should move
 * to `commonMain` unchanged.
 *
 * @param connectionUrl e.g. `udpin://0.0.0.0:14540` to listen locally, or
 *   `udpout://<host-ip>:<host-port>` to reach a drone elsewhere.
 */
expect fun flyMission(connectionUrl: String): Flow<MissionEvent>
