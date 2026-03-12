package io.mavsdk.kotlin.examples

import io.mavsdk.kotlin.*
import io.mavsdk.kotlin.plugins.camera_server.CameraServer
import kotlinx.coroutines.*
import kotlinx.coroutines.flow.*

/**
 * Example demonstrating a simulated camera server using the CameraServer plugin.
 *
 * Pairs with CameraClient.kt — run this server first.
 *
 * Prerequisites: nothing — this process IS the server. Start it, then run CameraClient.
 */
fun cameraServer() = runBlocking {
    println("=== MAVSDK-Kotlin CameraServer Example ===\n")

    // Act as a camera component, not a ground station.
    val config = Configuration.createWithComponentType(ComponentType.CAMERA)
    config.use { cfg ->
        Mavsdk(cfg).use { mavsdk ->
            // Bind to the camera port so clients can connect.
            println("Binding to udpin://0.0.0.0:14030...")
            mavsdk.addAnyConnection("udpin://0.0.0.0:14030")
                .onFailure { error ->
                    println("✗ Connection failed: $error")
                    return@runBlocking
                }
            println("✓ Listening\n")

            CameraServer.create(mavsdk).use { server ->

                // Register all operation handlers before activating.
                subscribeOperations(server)

                // Set initial capture state.
                server.setInProgress(false)

                // Publish camera information — this activates the plugin.
                val info = CameraServer.Information(
                    vendorName                = "MAVSDK",
                    modelName                 = "Example Camera Server",
                    firmwareVersion           = "1.0.0",
                    focalLengthMm             = 3.0f,
                    horizontalSensorSizeMm    = 3.68f,
                    verticalSensorSizeMm      = 2.76f,
                    horizontalResolutionPx    = 3280,
                    verticalResolutionPx      = 2464,
                    lensId                    = 0,
                    definitionFileVersion     = 0,
                    definitionFileUri         = "",
                    imageInVideoModeSupported = false,
                    videoInImageModeSupported = false,
                )
                val result = server.setInformation(info)
                if (result != CameraServer.Result.SUCCESS) {
                    println("✗ Failed to set camera info: $result")
                    return@runBlocking
                }
                println("✓ Camera active — ready for clients\n")

                // Run indefinitely as a server.
                println("Serving... (press Ctrl+C to stop)")
                delay(Long.MAX_VALUE)
            }
        }
    }
}

private fun CoroutineScope.subscribeOperations(server: CameraServer) {
    var imageCount = 0
    var isRecordingVideo = false
    var isCaptureInProgress = false
    var videoStartMs = 0L

    launch {
        server.subscribeTakePhoto().collect { index ->
            server.setInProgress(true)
            isCaptureInProgress = true
            println("Taking picture ($index)...")
            delay(500) // simulate capture

            val nowMs = java.lang.System.currentTimeMillis()
            imageCount++
            server.setInProgress(false)

            server.respondTakePhoto(
                CameraServer.CameraFeedback.OK,
                CameraServer.CaptureInfo(
                    position             = CameraServer.Position(0.0, 0.0, 0f, 0f),
                    attitudeQuaternion   = CameraServer.Quaternion(1f, 0f, 0f, 0f),
                    timeUtcUs            = nowMs * 1_000L,
                    isSuccess            = true,
                    index                = index,
                    fileUrl              = "",
                )
            )
            isCaptureInProgress = false
        }
    }

    launch {
        server.subscribeStartVideo().collect {
            println("Start video record")
            isRecordingVideo = true
            videoStartMs = java.lang.System.currentTimeMillis()
            server.respondStartVideo(CameraServer.CameraFeedback.OK)
        }
    }

    launch {
        server.subscribeStopVideo().collect {
            println("Stop video record")
            isRecordingVideo = false
            server.respondStopVideo(CameraServer.CameraFeedback.OK)
        }
    }

    launch {
        server.subscribeStartVideoStreaming().collect { streamId ->
            println("Start video streaming $streamId")
            server.respondStartVideoStreaming(CameraServer.CameraFeedback.OK)
        }
    }

    launch {
        server.subscribeStopVideoStreaming().collect { streamId ->
            println("Stop video streaming $streamId")
            server.respondStopVideoStreaming(CameraServer.CameraFeedback.OK)
        }
    }

    launch {
        server.subscribeSetMode().collect { mode ->
            println("Set camera mode $mode")
            server.respondSetMode(CameraServer.CameraFeedback.OK)
        }
    }

    launch {
        server.subscribeStorageInformation().collect {
            val totalMib = 4 * 1024 * 1024f
            server.respondStorageInformation(
                CameraServer.CameraFeedback.OK,
                CameraServer.StorageInformation(
                    usedStorageMib      = 100f,
                    availableStorageMib = totalMib - 100f,
                    totalStorageMib     = totalMib,
                    storageStatus       = 2, // FORMATTED
                    storageId           = 1,
                    storageType         = 3, // MICROSD
                    readSpeedMibS       = 0f,
                    writeSpeedMibS      = 0f,
                )
            )
        }
    }

    launch {
        server.subscribeCaptureStatus().collect {
            val recordingSecs = if (isRecordingVideo)
                (java.lang.System.currentTimeMillis() - videoStartMs) / 1000f else 0f
            server.respondCaptureStatus(
                CameraServer.CameraFeedback.OK,
                CameraServer.CaptureStatus(
                    imageIntervalS       = 0f,
                    recordingTimeS       = recordingSecs,
                    availableCapacityMib = 0f,
                    imageStatus          = if (isCaptureInProgress) 1 else 0,
                    videoStatus          = if (isRecordingVideo) 1 else 0,
                    imageCount           = imageCount,
                )
            )
        }
    }

    launch {
        server.subscribeFormatStorage().collect { storageId ->
            println("Format storage id=$storageId")
            server.respondFormatStorage(CameraServer.CameraFeedback.OK)
        }
    }

    launch {
        server.subscribeResetSettings().collect {
            println("Reset camera settings")
            server.respondResetSettings(CameraServer.CameraFeedback.OK)
        }
    }
}
