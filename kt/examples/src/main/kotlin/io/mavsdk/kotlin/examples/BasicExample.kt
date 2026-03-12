package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.telemetry.Telemetry
import io.mavsdk.kotlin.plugins.action.Action
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * Simple example showing how to connect to a drone and query its information
 */
fun main() = runBlocking {
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
                    return@runBlocking
                }
            
            // Wait for autopilot
            println("Waiting for autopilot...")
            val system = mavsdk.firstAutopilot(timeoutSeconds = 3.0)
            if (system == null) {
                println("✗ No autopilot found within timeout")
                println("\nTip: Make sure PX4 SITL or your drone is running and")
                println("     broadcasting to UDP port 14540")
                return@runBlocking
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
            
            // Create plugins
            val telemetry = Telemetry.create(system)
            val action = Action.create(system)
            
            telemetry.use { telem ->
                action.use { act ->
                    println("\n=== Position Updates ===")
                    
                    // Subscribe to position updates
                    val positionJob = launch {
                        telem.subscribePosition()
                            .take(10) // Take first 10 updates
                            .collect { position ->
                                println("Position: Lat=${position.latitudeDeg}, " +
                                       "Lon=${position.longitudeDeg}, " +
                                       "Alt=${position.absoluteAltitudeM}m")
                            }
                    }
                    
                    // Wait 3 seconds then arm
                    println("\nWaiting 3 seconds before arming...")
                    delay(3000)
                    
                    println("\n=== Arming ===")
                    try {
                        val result = act.armAsync()
                        println("✓ Armed successfully! Result: $result")
                    } catch (e: Action.ActionException) {
                        println("✗ Arm failed: ${e.result} - ${e.message}")
                    }
                    
                    // Wait for position updates to complete
                    positionJob.join()
                }
            }
            
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
