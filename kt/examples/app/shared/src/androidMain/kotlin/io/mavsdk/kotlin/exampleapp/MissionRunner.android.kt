package io.mavsdk.kotlin.exampleapp

import io.mavsdk.kotlin.ComponentType
import io.mavsdk.kotlin.Mavsdk
import io.mavsdk.kotlin.plugins.action.Action
import io.mavsdk.kotlin.plugins.mission.Mission
import kotlinx.coroutines.CompletableDeferred
import kotlinx.coroutines.cancelChildren
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.channels.ProducerScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.channelFlow
import kotlinx.coroutines.flow.flowOn
import kotlinx.coroutines.launch

private suspend fun ProducerScope<MissionEvent>.log(line: String) =
    send(MissionEvent.Log(line))

/**
 * Port of `examples/cli-jvm/.../FlyMission.kt`. Kept deliberately close to the
 * original so the two stay easy to diff: `println` becomes `log`, and the
 * `runBlocking` wrapper becomes the flow's own scope.
 */
actual fun flyMission(connectionUrl: String): Flow<MissionEvent> = channelFlow {
    log("=== MAVSDK-Kotlin FlyMission Example ===")

    Mavsdk(ComponentType.GROUND_STATION).use { mavsdk ->
        log("Connecting to $connectionUrl...")
        mavsdk.addAnyConnection(connectionUrl)
            .onFailure { error ->
                log("✗ Connection failed: $error")
                return@use
            }

        log("Waiting for autopilot...")
        val system = mavsdk.firstAutopilot(timeoutSeconds = 10.0)
        if (system == null) {
            log("✗ No autopilot found")
            return@use
        }
        log("✓ Autopilot found!")

        val action = system.action
        val mission = system.mission
        val telemetry = system.telemetry

        // Feed the map from the same connection as the mission. Heading is its own
        // stream, so keep the latest and attach it to each position; both jobs die
        // with the flow's scope when collection stops.
        val heading = MutableStateFlow<Float?>(null)
        launch { telemetry.subscribeHeading().collect { heading.value = it.headingDeg.toFloat() } }
        launch {
            telemetry.subscribePosition().collect {
                send(
                    MissionEvent.Telemetry(
                        it.latitudeDeg,
                        it.longitudeDeg,
                        it.relativeAltitudeM,
                        heading.value,
                    )
                )
            }
        }

        // Wait until the system reports all sensors healthy.
        log("Waiting for system to be ready...")
        while (!telemetry.healthAllOk()) {
            log("  Not ready yet...")
            delay(1_000)
        }
        log("✓ System ready")

        // Build mission items
        val items = listOf(
            Mission.MissionItem(47.398170327054473, 8.5456490218639658, 10f, 5f, false, 20f, 60f, Mission.CameraAction.NONE, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398241338125118, 8.5455360114574432, 10f, 2f, true, 0f, -60f, Mission.CameraAction.TAKE_PHOTO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398139363821485, 8.5453846156597137, 10f, 5f, true, -45f, 0f, Mission.CameraAction.START_PHOTO_INTERVAL, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398058617228855, 8.5454618036746979, 10f, 2f, false, -90f, 30f, Mission.CameraAction.STOP_PHOTO_INTERVAL, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398100366082858, 8.5456969141960144, 10f, 5f, false, -45f, -30f, Mission.CameraAction.START_VIDEO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
            Mission.MissionItem(47.398001890458097, 8.5455576181411743, 10f, 5f, false, 0f, 0f, Mission.CameraAction.STOP_VIDEO, 0f, 1.0, 1f, 0f, 0f, Mission.VehicleAction.NONE),
        )
        log("Uploading ${items.size} mission items...")
        val uploadResult = mission.uploadMission(Mission.MissionPlan(items))
        if (uploadResult != Mission.Result.SUCCESS) {
            log("✗ Mission upload failed: $uploadResult")
            return@use
        }
        log("✓ Mission uploaded")

        // Arm
        log("Arming...")
        val armResult = action.arm()
        if (armResult != Action.Result.SUCCESS) {
            log("✗ Arm failed: $armResult")
            return@use
        }
        log("✓ Armed")

        // Subscribe to mission progress; pause at item 2.
        val pauseJob = CompletableDeferred<Unit>()
        launch {
            mission.subscribeMissionProgress().collect { p ->
                log("Mission progress: ${p.current} / ${p.total}")
                if (p.current >= 2) pauseJob.complete(Unit)
            }
        }

        // Start
        log("Starting mission...")
        val startResult = mission.startMission()
        if (startResult != Mission.Result.SUCCESS) {
            log("✗ Start failed: $startResult")
            return@use
        }

        // Wait until item 2 reached, then pause. Deliberately no cancel here: the
        // subscription has to stay alive to keep reporting the second half of the
        // mission. complete() is idempotent, so later events are harmless.
        pauseJob.await()

        log("Pausing mission...")
        mission.pauseMission().also { log("Pause result: $it") }

        delay(5_000)

        // Continue
        log("Continuing mission...")
        mission.startMission().also { log("Start result: $it") }

        // Wait for mission to finish
        while (!mission.isMissionFinished()) delay(1_000)

        // Return to launch
        log("Commanding RTL...")
        val rtlResult = action.returnToLaunch()
        if (rtlResult != Action.Result.SUCCESS) {
            log("✗ RTL failed: $rtlResult")
        } else {
            log("✓ RTL commanded")
        }

        delay(2_000)
        while (telemetry.armed()) delay(1_000)
        log("Disarmed — done!")
    }

    // The subscriptions launched above never complete on their own, and channelFlow
    // waits for its children, so without this the flow never closes and the caller
    // is stuck collecting. Reached by the early return@use paths too.
    coroutineContext.cancelChildren()
}.flowOn(Dispatchers.IO)
