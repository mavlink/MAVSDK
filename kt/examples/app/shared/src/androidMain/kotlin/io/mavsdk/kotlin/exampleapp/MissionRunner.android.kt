package io.mavsdk.kotlin.exampleapp

import io.mavsdk.kotlin.ComponentType
import io.mavsdk.kotlin.Mavsdk
import io.mavsdk.kotlin.plugins.action.Action
import io.mavsdk.kotlin.plugins.mission.Mission
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.channelFlow
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.launch

/**
 * Port of `examples/cli-jvm/.../FlyMission.kt`. Kept deliberately close to the
 * original so the two stay easy to diff: `println` becomes `send`, and the
 * `runBlocking` wrapper becomes the flow's own scope.
 */
actual fun flyMission(connectionUrl: String): Flow<String> = channelFlow {
    send("=== MAVSDK-Kotlin FlyMission Example ===")

    Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
        send("Connecting to $connectionUrl...")
        mavsdk.addAnyConnection(connectionUrl)
            .onFailure { error ->
                send("✗ Connection failed: $error")
                return@use
            }

        send("Waiting for autopilot...")
        val system = mavsdk.firstAutopilot(timeoutSeconds = 10.0)
        if (system == null) {
            send("✗ No autopilot found")
            return@use
        }
        send("✓ Autopilot found!")

        val action = system.action
        val mission = system.mission
        val telemetry = system.telemetry

        // Wait until the system reports all sensors healthy.
        send("Waiting for system to be ready...")
        while (!telemetry.healthAllOk()) {
            send("  Not ready yet...")
            delay(1_000)
        }
        send("✓ System ready")

        // Build mission items
        val items = listOf(
            Mission.MissionItem(47.398170327054473, 8.5456490218639658, 10f, 5f, false, 20f, 60f, Mission.CameraAction.NONE, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398241338125118, 8.5455360114574432, 10f, 2f, true, 0f, -60f, Mission.CameraAction.TAKE_PHOTO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398139363821485, 8.5453846156597137, 10f, 5f, true, -45f, 0f, Mission.CameraAction.START_PHOTO_INTERVAL, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398058617228855, 8.5454618036746979, 10f, 2f, false, -90f, 30f, Mission.CameraAction.STOP_PHOTO_INTERVAL, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398100366082858, 8.5456969141960144, 10f, 5f, false, -45f, -30f, Mission.CameraAction.START_VIDEO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398001890458097, 8.5455576181411743, 10f, 5f, false, 0f, 0f, Mission.CameraAction.STOP_VIDEO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
        )
        send("Uploading ${items.size} mission items...")
        val uploadResult = mission.uploadMission(Mission.MissionPlan(items))
        if (uploadResult != Mission.Result.SUCCESS) {
            send("✗ Mission upload failed: $uploadResult")
            return@use
        }
        send("✓ Mission uploaded")

        // Arm
        send("Arming...")
        val armResult = action.arm()
        if (armResult != Action.Result.SUCCESS) {
            send("✗ Arm failed: $armResult")
            return@use
        }
        send("✓ Armed")

        // Subscribe to mission progress; pause at item 2.
        val pauseJob = CompletableDeferred<Unit>()
        val progressJob = launch {
            mission.subscribeMissionProgress().collect { p ->
                send("Mission progress: ${p.current} / ${p.total}")
                if (p.current >= 2) pauseJob.complete(Unit)
            }
        }

        // Start
        send("Starting mission...")
        val startResult = mission.startMission()
        if (startResult != Mission.Result.SUCCESS) {
            send("✗ Start failed: $startResult")
            progressJob.cancel()
            return@use
        }

        // Wait until item 2 reached, then pause
        pauseJob.await()
        progressJob.cancel()

        send("Pausing mission...")
        mission.pauseMission().also { send("Pause result: $it") }

        delay(5_000)

        // Continue
        send("Continuing mission...")
        mission.startMission().also { send("Start result: $it") }

        // Wait for mission to finish
        while (!mission.isMissionFinished()) delay(1_000)

        // Return to launch
        send("Commanding RTL...")
        val rtlResult = action.returnToLaunch()
        if (rtlResult != Action.Result.SUCCESS) {
            send("✗ RTL failed: $rtlResult")
        } else {
            send("✓ RTL commanded")
        }

        delay(2_000)
        while (telemetry.armed()) delay(1_000)
        send("Disarmed — done!")
    }
}.flowOn(Dispatchers.IO)
