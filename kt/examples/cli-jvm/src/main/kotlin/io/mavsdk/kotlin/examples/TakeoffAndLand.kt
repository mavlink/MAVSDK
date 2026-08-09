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

        val action = system.action
        val telemetry = system.telemetry

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
        val armResult = action.arm()
        if (armResult != Action.Result.SUCCESS) {
            println("✗ Arm failed: $armResult")
            positionJob.cancel()
            return@use
        }
        println("✓ Armed!")

        // Takeoff
        println("\n=== Takeoff ===")
        val takeoffResult = action.takeoff()
        if (takeoffResult != Action.Result.SUCCESS) {
            println("✗ Takeoff failed: $takeoffResult")
            positionJob.cancel()
            return@use
        }
        println("✓ Takeoff initiated!")

        // Hover for 10 seconds
        println("\nHovering for 10 seconds...")
        delay(10_000)

        // Land
        println("\n=== Landing ===")
        val landResult = action.land()
        if (landResult != Action.Result.SUCCESS) {
            println("✗ Land failed: $landResult")
        } else {
            println("✓ Landing initiated!")
        }

        // Wait a bit for landing to complete, then stop logging
        delay(5_000)
        positionJob.cancel()

        println("\n✓ TakeoffAndLand example complete!")
    }
}
