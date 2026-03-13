package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.action.Action
import io.mavsdk.kotlin.plugins.telemetry.Telemetry
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * Example demonstrating takeoff and landing using Action and Telemetry plugins.
 */
fun takeoffAndLand() = runBlocking {
    println("=== MAVSDK-Kotlin TakeoffAndLand Example ===\n")

    Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
        println("Connecting to udp://:14540...")
        mavsdk.addAnyConnection("udp://:14540")
            .onSuccess { println("✓ Connection added\n") }
            .onFailure { error ->
                println("✗ Connection failed: $error")
                return@runBlocking
            }

        println("Waiting for autopilot (10s timeout)...")
        val system = mavsdk.firstAutopilot(timeoutSeconds = 10.0)
        if (system == null) {
            println("✗ No autopilot found within timeout")
            println("  Make sure PX4 SITL is running: make px4_sitl gazebo")
            return@runBlocking
        }
        println("✓ Autopilot found!\n")

        val action = Action.create(system)
        val telemetry = Telemetry.create(system)

        // Start logging position every second in the background
        val positionJob = launch {
            telemetry.subscribePosition()
                .sample(1_000)
                .collect { pos ->
                    println("  [pos] Lat=%.6f, Lon=%.6f, Alt=%.2fm (rel)".format(
                        pos.latitudeDeg, pos.longitudeDeg, pos.relativeAltitudeM))
                }
        }

        // Arm
        println("\n=== Arming ===")
        try {
            val armResult = action.arm()
            println("✓ Armed! Result: $armResult")
        } catch (e: Action.ActionException) {
            println("✗ Arm failed: ${e.result} - ${e.message}")
            positionJob.cancel()
            return@use
        }

        // Takeoff
        println("\n=== Takeoff ===")
        try {
            val takeoffResult = action.takeoff()
            println("✓ Takeoff initiated! Result: $takeoffResult")
        } catch (e: Action.ActionException) {
            println("✗ Takeoff failed: ${e.result} - ${e.message}")
            positionJob.cancel()
            return@use
        }

        // Hover for 10 seconds
        println("\nHovering for 10 seconds...")
        delay(10_000)

        // Land
        println("\n=== Landing ===")
        try {
            val landResult = action.land()
            println("✓ Landing initiated! Result: $landResult")
        } catch (e: Action.ActionException) {
            println("✗ Land failed: ${e.result} - ${e.message}")
        }

        // Wait a bit for landing to complete, then stop logging
        delay(5_000)
        positionJob.cancel()

        println("\n✓ TakeoffAndLand example complete!")
    }
}
