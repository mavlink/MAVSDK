package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.camera.Camera
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * Example demonstrating camera control from a ground-station client.
 *
 * Pairs with CameraServer.kt — run the server first, then this client.
 *
 * Prerequisites: camera server running and reachable on UDP port 14030.
 */
fun cameraClient() = runBlocking {
    println("=== MAVSDK-Kotlin CameraClient Example ===\n")

    Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
        println("Connecting to udpout://127.0.0.1:14030...")
        mavsdk.addAnyConnection("udpout://127.0.0.1:14030")
            .onFailure { error ->
                println("✗ Connection failed: $error")
                return@runBlocking
            }
        println("✓ Connected\n")

        // Wait for a system that has a camera.
        println("Waiting for camera system...")
        val system = mavsdk.subscribeOnNewSystem()
            .filter { it.hasCamera() }
            .first()
        println("✓ Camera system discovered!\n")

        val camera = Camera.create(system)

        // Subscribe to storage updates in the background.
        val storageJob = launch {
            camera.subscribeStorage().collect { update ->
                println("[storage] total=${update.storage.totalStorageMib} MiB  " +
                        "used=${update.storage.usedStorageMib} MiB  " +
                        "available=${update.storage.availableStorageMib} MiB")
            }
        }

        val cameraList = withTimeoutOrNull(5_000) {
            camera.subscribeCameraList()
                .filter { it.cameras.isNotEmpty() }
                .first()
        }
        if (cameraList != null) {
            println("Camera list received (${cameraList.cameras.size} camera(s)):")
            cameraList.cameras.forEach { cam ->
                println("  componentId=${cam.componentId}")
            }
        } else {
            println("No camera list received within timeout.")
        }
        val componentId = cameraList?.cameras?.firstOrNull()?.componentId ?: 100
        println("Using componentId=$componentId\n")

        doCameraOperations(camera, componentId)

        // Keep running to receive storage updates.
        println("\nListening for storage updates (press Ctrl+C to stop)...")
        delay(Long.MAX_VALUE)

        storageJob.cancel()
    }
}

private suspend fun doCameraOperations(camera: Camera, componentId: Int) {
    // Photo mode
    camera.setMode(componentId, Camera.Mode.PHOTO).also {
        println("Set photo mode: $it")
    }
    camera.takePhoto(componentId).also {
        println("Take photo: $it")
    }

    // Video mode
    camera.setMode(componentId, Camera.Mode.VIDEO).also {
        println("Set video mode: $it")
    }
    camera.startVideo(componentId).also {
        println("Start video: $it")
    }
    camera.stopVideo(componentId).also {
        println("Stop video: $it")
    }

    // Video streaming
    camera.startVideoStreaming(componentId, 0).also {
        println("Start video streaming: $it")
    }
    camera.stopVideoStreaming(componentId, 0).also {
        println("Stop video streaming: $it")
    }

    // Storage / settings
    camera.formatStorage(componentId, 0).also {
        println("Format storage: $it")
    }
    camera.resetSettings(componentId).also {
        println("Reset settings: $it")
    }
}
