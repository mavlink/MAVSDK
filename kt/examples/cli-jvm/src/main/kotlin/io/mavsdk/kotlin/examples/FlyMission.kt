package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.action.Action
import io.mavsdk.kotlin.plugins.mission.Mission
import io.mavsdk.kotlin.plugins.telemetry.Telemetry
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * Example demonstrating waypoint missions using Action, Mission, and Telemetry plugins.
 *
 * Prerequisites: PX4 SITL running and broadcasting on UDP port 14540.
 */
fun flyMission() = runBlocking {
    println("=== MAVSDK-Kotlin FlyMission Example ===\n")

    Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
        println("Connecting to udp://:14540...")
        mavsdk.addAnyConnection("udp://:14540")
            .onFailure { error ->
                println("✗ Connection failed: $error")
                return@runBlocking
            }

        println("Waiting for autopilot...")
        val system = mavsdk.firstAutopilot(timeoutSeconds = 10.0)
        if (system == null) {
            println("✗ No autopilot found")
            return@runBlocking
        }
        println("✓ Autopilot found!\n")

        val action = system.action
        val mission = system.mission
        val telemetry = system.telemetry

        // Wait until the system reports all sensors healthy.
        println("Waiting for system to be ready...")
        while (!telemetry.healthAllOk()) {
            println("  Not ready yet...")
            delay(1_000)
        }
        println("✓ System ready\n")

        // Build mission items
        val items = listOf(
            Mission.MissionItem(47.398170327054473, 8.5456490218639658, 10f, 5f, false,  20f,  60f, Mission.CameraAction.NONE, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398241338125118, 8.5455360114574432, 10f, 2f, true,    0f, -60f, Mission.CameraAction.TAKE_PHOTO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398139363821485, 8.5453846156597137, 10f, 5f, true,  -45f,   0f, Mission.CameraAction.START_PHOTO_INTERVAL, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398058617228855, 8.5454618036746979, 10f, 2f, false, -90f,  30f, Mission.CameraAction.STOP_PHOTO_INTERVAL, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398100366082858, 8.5456969141960144, 10f, 5f, false, -45f, -30f, Mission.CameraAction.START_VIDEO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398001890458097, 8.5455576181411743, 10f, 5f, false,   0f,   0f, Mission.CameraAction.STOP_VIDEO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
        )
        println("Uploading ${items.size} mission items...")
        val uploadResult = mission.uploadMission(Mission.MissionPlan(items))
        if (uploadResult != Mission.Result.SUCCESS) {
            println("✗ Mission upload failed: $uploadResult")
            return@use
        }
        println("✓ Mission uploaded\n")

        // Arm
        println("Arming...")
        val armResult = action.arm()
        if (armResult != Action.Result.SUCCESS) {
            println("✗ Arm failed: $armResult")
            return@use
        }
        println("✓ Armed\n")

        // Subscribe to mission progress; pause at item 2.
        val pauseJob = CompletableDeferred<Unit>()
        val progressJob = launch {
            mission.subscribeMissionProgress().collect { p ->
                println("Mission progress: ${p.current} / ${p.total}")
                if (p.current >= 2) pauseJob.complete(Unit)
            }
        }

        // Start
        println("Starting mission...")
        val startResult = mission.startMission()
        if (startResult != Mission.Result.SUCCESS) {
            println("✗ Start failed: $startResult")
            progressJob.cancel()
            return@use
        }

        // Wait until item 2 reached, then pause. No cancel here: the subscription
        // has to stay alive to keep reporting the second half of the mission, and
        // complete() is idempotent so later events are harmless.
        pauseJob.await()

        println("\nPausing mission...")
        mission.pauseMission().also { println("Pause result: $it") }

        delay(5_000)

        // Continue
        println("Continuing mission...")
        mission.startMission().also { println("Start result: $it") }

        // Wait for mission to finish
        while (!mission.isMissionFinished()) delay(1_000)

        // Return to launch
        println("\nCommanding RTL...")
        val rtlResult = action.returnToLaunch()
        if (rtlResult != Action.Result.SUCCESS) {
            println("✗ RTL failed: $rtlResult")
        } else {
            println("✓ RTL commanded")
        }

        delay(2_000)
        while (telemetry.armed()) delay(1_000)
        println("Disarmed — done!")

        // Only now: runBlocking waits for this child, so it has to stop before the
        // example can exit.
        progressJob.cancel()
    }
}
