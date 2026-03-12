package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.action.Action
import io.mavsdk.kotlin.plugins.telemetry.Telemetry
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * Example demonstrating takeoff and landing using Action and Telemetry plugins.
 *
 * Prerequisites: PX4 SITL running and broadcasting on UDP port 14540.
 *   make px4_sitl gazebo
 */
fun takeoffAndLand() = runBlocking {
    println("=== MAVSDK-Kotlin TakeoffAndLand Example ===\n")

    val config = Configuration.createWithComponentType(ComponentType.GROUND_STATION)
    config.use { cfg ->
        Mavsdk(cfg).use { mavsdk ->
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

            action.use { act ->
                telemetry.use { telem ->
                    // Start logging position every second in the background
                    val positionJob = launch {
                        telem.subscribePosition()
                            .sample(1_000)
                            .collect { pos ->
                                println("  [pos] Lat=%.6f, Lon=%.6f, Alt=%.2fm (rel)".format(
                                    pos.latitudeDeg, pos.longitudeDeg, pos.relativeAltitudeM))
                            }
                    }

                    // Arm
                    println("\n=== Arming ===")
                    try {
                        val armResult = act.armAsync()
                        println("✓ Armed! Result: $armResult")
                    } catch (e: Action.ActionException) {
                        println("✗ Arm failed: ${e.result} - ${e.message}")
                        positionJob.cancel()
                        return@use
                    }

                    // Takeoff
                    println("\n=== Takeoff ===")
                    try {
                        val takeoffResult = act.takeoffAsync()
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
                        val landResult = act.landAsync()
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
        }
    }
}
