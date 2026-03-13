package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.calibration.Calibration
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * Example demonstrating sensor calibration using the Calibration plugin.
 *
 * Best tested against a real flight controller (e.g. Pixhawk), not simulation.
 * Prerequisites: connect via serial or UDP.
 */
fun calibrate() = runBlocking {
    println("=== MAVSDK-Kotlin Calibrate Example ===\n")

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
            println("✗ No autopilot found within timeout")
            return@runBlocking
        }
        println("✓ Autopilot found!\n")

        val cal = Calibration.create(system)
        runCalibration("accelerometer", cal.calibrateAccelerometer())
        runCalibration("gyro",          cal.calibrateGyro())
        runCalibration("magnetometer",  cal.calibrateMagnetometer())
    }
}

private suspend fun runCalibration(name: String, progress: Flow<Calibration.ProgressData>) {
    println("Calibrating $name...")
    try {
        progress.collect { data ->
            if (data.hasProgress) println("  Progress: ${"%.1f".format(data.progress * 100)}%")
            if (data.hasStatusText) println("  Instruction: ${data.statusText}")
        }
        println("✓ $name calibration succeeded!\n")
    } catch (e: Calibration.CalibrationException) {
        println("✗ $name calibration failed: ${e.result} - ${e.message}\n")
    }
}
