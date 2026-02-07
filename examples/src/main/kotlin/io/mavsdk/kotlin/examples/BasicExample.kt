package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*

/**
 * Simple example showing how to connect to a drone and query its information
 */
fun main() {
    println("=== MAVSDK-Kotlin Example ===\n")

    // Create configuration for a ground station
    val config = Configuration.createWithComponentType(ComponentType.GROUND_STATION)

    config.use { cfg ->
        println("Created configuration:")
        println("  System ID: ${cfg.systemId}")
        println("  Component ID: ${cfg.componentId}\n")

        // Create MAVSDK instance
        Mavsdk(cfg).use { mavsdk ->
            println("MAVSDK version: ${mavsdk.version()}\n")

            // Add connection
            val connectionUrl = "udp://:14540"
            println("Connecting to $connectionUrl...")

            mavsdk.addAnyConnection(connectionUrl)
                .onSuccess {
                    println("✓ Connection added successfully\n")
                }
                .onFailure { error ->
                    println("✗ Connection failed: $error")
                    return@use
                }

            // Wait for autopilot
            println("Waiting for autopilot...")
            val system = mavsdk.firstAutopilot(timeoutSeconds = 3.0)

            if (system == null) {
                println("✗ No autopilot found within timeout")
                println("\nTip: Make sure PX4 SITL or your drone is running and")
                println("     broadcasting to UDP port 14540")
                return@use
            }

            println("✓ Found autopilot!\n")

            // Display system information
            println("=== System Information ===")
            println("System ID: ${system.getSystemId()}")
            println("Connected: ${system.isConnected()}")
            println("Has autopilot: ${system.hasAutopilot()}")
            println("Is standalone: ${system.isStandalone()}")
            println("Has camera: ${system.hasCamera()}")
            println("Has gimbal: ${system.hasGimbal()}")
            println("Autopilot type: ${system.getAutopilotType()}")
            println("Vehicle type: ${system.getVehicleType()}")

            val componentIds = system.getComponentIds()
            println("Component IDs: ${componentIds.joinToString(", ")}")

            println("\n=== All Systems ===")
            val allSystems = mavsdk.getSystems()
            println("Total systems discovered: ${allSystems.size}")

            for (sys in allSystems) {
                println("  - System ${sys.getSystemId()}: " +
                        "${sys.getAutopilotType()} / ${sys.getVehicleType()}")
            }

            println("\n✓ Example completed successfully!")
        }
    }
}
