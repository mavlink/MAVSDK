package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.ComponentType
import io.mavsdk.kotlin.Mavsdk
import io.mavsdk.kotlin.plugins.action.Action
import io.mavsdk.kotlin.plugins.telemetry.Telemetry
import kotlinx.coroutines.cancelAndJoin
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.collect
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking

fun telemetry() = runBlocking {
    println("=== MAVSDK-Kotlin Telemetry Example ===\n")

    Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
        mavsdk.addAnyConnection("udpin://0.0.0.0:14540")
            .onFailure {
                println("Connection failed: $it")
                return@runBlocking
            }

        println("Waiting for a drone...")
        val system = mavsdk.firstAutopilot(timeoutSeconds = 10.0) ?: run {
            println("No valid autopilot found")
            return@runBlocking
        }

        val action = Action.create(system)
        val telemetry = Telemetry.create(system)
        val positionJob = launch {
            telemetry.subscribePosition().collect { position ->
                println(
                    "Position update: Lat=%.6f, Lon=%.6f, Alt=%.1fm".format(
                        position.latitudeDeg,
                        position.longitudeDeg,
                        position.relativeAltitudeM,
                    )
                )
            }
        }

        println("Arming...")
        check(action.arm() == Action.Result.SUCCESS) { "Arming failed" }

        println("Taking off...")
        check(action.takeoff() == Action.Result.SUCCESS) { "Takeoff failed" }

        println("Flying for 10 seconds...")
        delay(10_000)

        val currentPosition = telemetry.position()
        println("Current position:")
        println("  Lat: %.6f".format(currentPosition.latitudeDeg))
        println("  Lon: %.6f".format(currentPosition.longitudeDeg))
        println("  Alt: %.1fm".format(currentPosition.relativeAltitudeM))

        println("Landing...")
        check(action.land() == Action.Result.SUCCESS) { "Landing failed" }

        println("Waiting for landing completion...")
        telemetry.subscribeLandedState().first { state ->
            println("Landed state: $state")
            state == Telemetry.LandedState.ON_GROUND
        }

        positionJob.cancelAndJoin()
        println("Flight complete")
    }
}
