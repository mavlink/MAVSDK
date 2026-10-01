package io.mavsdk.kotlin

import io.mavsdk.kotlin.plugins.telemetry_server.TelemetryServer
import kotlin.test.Test
import kotlin.test.assertEquals
import kotlin.test.assertNotNull
import kotlinx.coroutines.cancelAndJoin
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeout

/**
 * End-to-end test for the Kotlin bindings.
 *
 * Two MAVSDK instances are brought up in-process, one as a ground station and one as an autopilot,
 * linked over a local UDP connection, and a real MAVLink exchange is driven between them. Nothing
 * is mocked: the assertion is on what the ground station actually received.
 *
 * This is the Kotlin counterpart of py/test/test_mission_raw_upload.py, and like it, it is meant to
 * be readable top to bottom as a template for new tests. Two things it does on purpose:
 *
 * - **Register server plugins before the connection is added.** MAVSDK advertises component
 *   capabilities in the first AUTOPILOT_VERSION exchange, so a server plugin created after the link
 *   is up may miss the window.
 * - **Assert on what the other side received**, not just on a result code.
 *
 * UDP ports are hard-coded, as in cpp/src/system_tests and py/test. Pick one that no other test
 * uses.
 */
class EndToEndTest {

    @Test
    fun `a position published by the autopilot arrives at the ground station`() = runBlocking {
        Mavsdk(ComponentType.AUTOPILOT).use { autopilot ->
            val telemetryServer = autopilot.serverComponent().telemetryServer

            Mavsdk(ComponentType.GROUND_STATION).use { groundStation ->
                groundStation.addAnyConnectionWithHandle("udpin://0.0.0.0:17050").getOrThrow()
                autopilot.addAnyConnectionWithHandle("udpout://127.0.0.1:17050").getOrThrow()

                val system = groundStation.firstAutopilot(timeoutSeconds = DISCOVERY_TIMEOUT_S)
                assertNotNull(system, "the autopilot was not discovered")
                assertEquals(1, system.getSystemId())

                // Published until the subscription sees one: the ground station only
                // receives what is sent after it has subscribed.
                val publisher = launch {
                    while (true) {
                        telemetryServer.publishPosition(POSITION, VELOCITY, HEADING)
                        delay(PUBLISH_INTERVAL_MS)
                    }
                }

                val received =
                    withTimeout(EXCHANGE_TIMEOUT_MS) {
                        system.telemetry.subscribePosition().first()
                    }
                publisher.cancelAndJoin()

                assertEquals(POSITION.latitudeDeg, received.latitudeDeg, 1e-6)
                assertEquals(POSITION.longitudeDeg, received.longitudeDeg, 1e-6)
                assertEquals(POSITION.relativeAltitudeM, received.relativeAltitudeM, 1e-3f)
            }
        }
    }

    private companion object {
        const val DISCOVERY_TIMEOUT_S = 10.0
        const val EXCHANGE_TIMEOUT_MS = 10_000L
        const val PUBLISH_INTERVAL_MS = 100L

        val POSITION =
            TelemetryServer.Position(
                latitudeDeg = 47.398,
                longitudeDeg = 8.546,
                absoluteAltitudeM = 500.0f,
                relativeAltitudeM = 10.0f,
            )
        val VELOCITY = TelemetryServer.VelocityNed(northMS = 0.0f, eastMS = 0.0f, downMS = 0.0f)
        val HEADING = TelemetryServer.Heading(headingDeg = 90.0)
    }
}
