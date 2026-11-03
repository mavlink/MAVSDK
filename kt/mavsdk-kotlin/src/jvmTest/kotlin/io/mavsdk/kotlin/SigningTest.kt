package io.mavsdk.kotlin

import io.mavsdk.kotlin.plugins.telemetry.Telemetry
import io.mavsdk.kotlin.plugins.telemetry_server.TelemetryServer
import kotlin.test.Test
import kotlin.test.assertEquals
import kotlin.test.assertFailsWith
import kotlin.test.assertFalse
import kotlin.test.assertNotNull
import kotlin.test.assertNull
import kotlin.test.assertTrue
import kotlinx.coroutines.cancelAndJoin
import kotlinx.coroutines.coroutineScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.launch
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeoutOrNull

/**
 * End-to-end test for MAVLink 2 message signing, see EndToEndTest for the general setup.
 *
 * The autopilot publishes a position, which only gets through if both use the same key.
 */
class SigningTest {

    @Test
    fun `signing can be switched on and off`() {
        Mavsdk(ComponentType.AUTOPILOT).use { mavsdk ->
            assertFalse(mavsdk.isSigningEnabled())
            mavsdk.enableSigning(KEY)
            assertTrue(mavsdk.isSigningEnabled())
            mavsdk.disableSigning()
            assertFalse(mavsdk.isSigningEnabled())
        }
    }

    @Test
    fun `a key of the wrong length is refused`() {
        Mavsdk(ComponentType.AUTOPILOT).use { mavsdk ->
            assertFailsWith<IllegalArgumentException> { mavsdk.enableSigning(ByteArray(16)) }
            assertFalse(mavsdk.isSigningEnabled())
        }
    }

    @Test
    fun `a signed position arrives with the same key`() = runBlocking {
        val received = exchangePosition(port = 17055, groundStationKey = KEY, autopilotKey = KEY)
        assertNotNull(received, "the signed position did not arrive")
        assertEquals(POSITION.latitudeDeg, received.latitudeDeg, 1e-6)
    }

    @Test
    fun `an autopilot signing with another key is not discovered`() {
        Mavsdk(ComponentType.AUTOPILOT).use { autopilot ->
            autopilot.enableSigning(OTHER_KEY)
            Mavsdk(ComponentType.GROUND_STATION).use { groundStation ->
                groundStation.enableSigning(KEY)
                groundStation.addAnyConnectionWithHandle("udpin://0.0.0.0:17056").getOrThrow()
                autopilot.addAnyConnectionWithHandle("udpout://127.0.0.1:17056").getOrThrow()

                assertNull(groundStation.firstAutopilot(timeoutSeconds = NOT_DISCOVERED_TIMEOUT_S))
            }
        }
    }

    private suspend fun exchangePosition(
        port: Int,
        groundStationKey: ByteArray,
        autopilotKey: ByteArray,
    ): Telemetry.Position? = coroutineScope {
        Mavsdk(ComponentType.AUTOPILOT).use { autopilot ->
            autopilot.enableSigning(autopilotKey)
            val telemetryServer = autopilot.serverComponent().telemetryServer

            Mavsdk(ComponentType.GROUND_STATION).use { groundStation ->
                groundStation.enableSigning(groundStationKey)
                groundStation.addAnyConnectionWithHandle("udpin://0.0.0.0:$port").getOrThrow()
                autopilot.addAnyConnectionWithHandle("udpout://127.0.0.1:$port").getOrThrow()

                val system = groundStation.firstAutopilot(timeoutSeconds = DISCOVERY_TIMEOUT_S)
                assertNotNull(system, "the autopilot was not discovered")

                val publisher = launch {
                    while (true) {
                        telemetryServer.publishPosition(POSITION, VELOCITY, HEADING)
                        delay(PUBLISH_INTERVAL_MS)
                    }
                }

                val received =
                    withTimeoutOrNull(EXCHANGE_TIMEOUT_MS) {
                        system.telemetry.subscribePosition().first()
                    }
                publisher.cancelAndJoin()
                received
            }
        }
    }

    private companion object {
        const val DISCOVERY_TIMEOUT_S = 10.0
        const val NOT_DISCOVERED_TIMEOUT_S = 3.0
        const val EXCHANGE_TIMEOUT_MS = 3_000L
        const val PUBLISH_INTERVAL_MS = 100L

        val KEY = ByteArray(32) { it.toByte() }
        val OTHER_KEY = ByteArray(32) { (it + 1).toByte() }

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
