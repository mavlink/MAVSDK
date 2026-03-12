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
 *   make px4_sitl gazebo
 */
fun flyMission() = runBlocking {
    println("=== MAVSDK-Kotlin FlyMission Example ===\n")

    val config = Configuration.createWithComponentType(ComponentType.GROUND_STATION)
    config.use { cfg ->
        Mavsdk(cfg).use { mavsdk ->
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

            val action   = Action.create(system)
            val mission  = Mission.create(system)
            val telemetry = Telemetry.create(system)

            action.use { act ->
                mission.use { msn ->
                    telemetry.use { telem ->

                        // Wait until the system reports all sensors healthy.
                        println("Waiting for system to be ready...")
                        while (!telem.healthAllOk()) {
                            println("  Not ready yet...")
                            delay(1_000)
                        }
                        println("✓ System ready\n")

                        // -- Build mission items --
                        // NOTE: MissionPlan construction with items is not yet supported
                        // in the Kotlin JNI layer. The items defined here are illustrative.
                        val items = listOf(
                            Mission.MissionItem(47.398170327054473, 8.5456490218639658, 10f, 5f, false,  20f,  60f, 0, 0f, 1.0, 1f, 0f, 0f, 0),
                            Mission.MissionItem(47.398241338125118, 8.5455360114574432, 10f, 2f, true,    0f, -60f, 1, 0f, 1.0, 1f, 0f, 0f, 0),
                            Mission.MissionItem(47.398139363821485, 8.5453846156597137, 10f, 5f, true,  -45f,   0f, 2, 0f, 1.0, 1f, 0f, 0f, 0),
                            Mission.MissionItem(47.398058617228855, 8.5454618036746979, 10f, 2f, false, -90f,  30f, 3, 0f, 1.0, 1f, 0f, 0f, 0),
                            Mission.MissionItem(47.398100366082858, 8.5456969141960144, 10f, 5f, false, -45f, -30f, 4, 0f, 1.0, 1f, 0f, 0f, 0),
                            Mission.MissionItem(47.398001890458097, 8.5455576181411743, 10f, 5f, false,   0f,   0f, 5, 0f, 1.0, 1f, 0f, 0f, 0),
                        )
                        println("Uploading ${items.size} mission items...")
                        val uploadResult = msn.uploadMissionAsync(Mission.MissionPlan(items))
                        if (uploadResult != Mission.Result.SUCCESS) {
                            println("✗ Mission upload failed: $uploadResult")
                            return@use
                        }
                        println("✓ Mission uploaded\n")

                        // Arm
                        println("Arming...")
                        try {
                            act.armAsync()
                            println("✓ Armed\n")
                        } catch (e: Action.ActionException) {
                            println("✗ Arm failed: ${e.result}")
                            return@use
                        }

                        // Subscribe to mission progress; pause at item 2.
                        val pauseJob = CompletableDeferred<Unit>()
                        val progressJob = launch {
                            msn.subscribeMissionProgress().collect { p ->
                                println("Mission progress: ${p.current} / ${p.total}")
                                if (p.current >= 2) pauseJob.complete(Unit)
                            }
                        }

                        // Start
                        println("Starting mission...")
                        val startResult = msn.startMissionAsync()
                        if (startResult != Mission.Result.SUCCESS) {
                            println("✗ Start failed: $startResult")
                            progressJob.cancel()
                            return@use
                        }

                        // Wait until item 2 reached, then pause
                        pauseJob.await()
                        progressJob.cancel()

                        println("\nPausing mission...")
                        msn.pauseMissionAsync().also { println("Pause result: $it") }

                        delay(5_000)

                        // Continue
                        println("Continuing mission...")
                        msn.startMissionAsync().also { println("Start result: $it") }

                        // Wait for mission to finish
                        while (!msn.isMissionFinished()) delay(1_000)

                        // Return to launch
                        println("\nCommanding RTL...")
                        try {
                            act.returnToLaunchAsync()
                            println("✓ RTL commanded")
                        } catch (e: Action.ActionException) {
                            println("✗ RTL failed: ${e.result}")
                        }

                        delay(2_000)
                        while (telem.armed()) delay(1_000)
                        println("Disarmed — done!")
                    }
                }
            }
        }
    }
}
