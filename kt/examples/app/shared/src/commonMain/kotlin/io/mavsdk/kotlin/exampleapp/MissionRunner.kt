package io.mavsdk.kotlin.exampleapp

import kotlinx.coroutines.flow.Flow

/**
 * Runs the same waypoint mission as the `FlyMission` example in `examples/cli-jvm`,
 * emitting one log line per step instead of printing to stdout.
 *
 * The work happens while the flow is collected, off the main thread: cancelling
 * the collection cancels the run and closes the MAVSDK instance.
 *
 * This is an `expect` rather than common code because `mavsdk-kotlin` currently
 * has no iOS target — see `KOTLIN_IOS_PLAN.md`. Once it does, the Android
 * implementation should move to `commonMain` unchanged.
 *
 * @param connectionUrl e.g. `udp://:14540` to listen locally, or
 *   `udpout://<host-ip>:14540` to reach a SITL running on another machine.
 */
expect fun flyMission(connectionUrl: String): Flow<String>
