package io.mavsdk.kotlin

import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress
import java.net.InetSocketAddress
import java.net.SocketTimeoutException
import java.nio.ByteBuffer
import java.nio.ByteOrder
import kotlin.math.abs
import kotlin.test.Test
import kotlin.test.assertFalse
import kotlin.test.assertNotNull
import kotlin.test.assertTrue
import kotlinx.coroutines.CoroutineStart
import kotlinx.coroutines.async
import kotlinx.coroutines.delay
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withTimeoutOrNull

/**
 * Test for the time synchronization state.
 *
 * A MAVSDK instance does not answer TIMESYNC until it is synced itself, so the autopilot here is a
 * raw UDP socket that answers with a clock starting at boot, like PX4 does.
 */
class TimesyncTest {

    @Test
    fun `the offset to the autopilot clock is reported`(): Unit = runBlocking {
        Mavsdk(ComponentType.GROUND_STATION).use { groundStation ->
            groundStation.addAnyConnectionWithHandle("udpin://127.0.0.1:17060").getOrThrow()
            FakeAutopilot(17060).use { autopilot ->
                val system = groundStation.firstAutopilot(timeoutSeconds = DISCOVERY_TIMEOUT_S)
                assertNotNull(system, "the fake autopilot was not discovered")
                assertFalse(system.getTimesyncStatus().synced)

                // Subscribed before enabling, as the first request goes out right away. The
                // callbackFlow only subscribes once its producer runs.
                val synced =
                    async(start = CoroutineStart.UNDISPATCHED) {
                        withTimeoutOrNull(SYNC_TIMEOUT_MS) {
                            system.subscribeTimesyncStatus().first { it.synced }
                        }
                    }
                delay(SUBSCRIBE_DELAY_MS)
                system.enableTimesync()
                val status = synced.await()
                assertNotNull(status, "no synced state was reported")
                assertTrue(abs(status.offsetNs - autopilot.expectedOffsetNs) < OFFSET_TOLERANCE_NS)
                assertTrue(system.getTimesyncStatus().synced)
            }
        }
    }

    @Test
    fun `closing with an active subscription releases it`(): Unit = runBlocking {
        Mavsdk(ComponentType.GROUND_STATION).use { groundStation ->
            groundStation.addAnyConnectionWithHandle("udpin://127.0.0.1:17061").getOrThrow()
            FakeAutopilot(17061).use {
                val system = groundStation.firstAutopilot(timeoutSeconds = DISCOVERY_TIMEOUT_S)
                assertNotNull(system, "the fake autopilot was not discovered")
                // Closed while the flow is still subscribed.
                val first =
                    async(start = CoroutineStart.UNDISPATCHED) {
                        withTimeoutOrNull(SYNC_TIMEOUT_MS) {
                            system.subscribeTimesyncStatus().first {
                                groundStation.close()
                                true
                            }
                        }
                    }
                delay(SUBSCRIBE_DELAY_MS)
                system.enableTimesync()
                val status = first.await()
                assertNotNull(status, "no state was reported")
            }
        }
    }

    private class FakeAutopilot(groundStationPort: Int) : AutoCloseable {
        private val bootNs = java.lang.System.currentTimeMillis() * 1_000_000L - 100_000_000_000L
        private val groundStation =
            InetSocketAddress(InetAddress.getLoopbackAddress(), groundStationPort)
        private val socket =
            DatagramSocket(InetSocketAddress(InetAddress.getLoopbackAddress(), 0)).apply {
                soTimeout = 50
            }
        private var seq = 0
        @Volatile private var running = true
        private val thread = Thread(::run).apply { start() }

        /** Autopilot time minus local time. */
        val expectedOffsetNs = -bootNs

        override fun close() {
            running = false
            thread.join()
            socket.close()
        }

        private fun nowNs() = java.lang.System.currentTimeMillis() * 1_000_000L - bootNs

        private fun run() {
            var nextHeartbeat = 0L
            val buffer = ByteArray(2048)
            while (running) {
                if (java.lang.System.nanoTime() >= nextHeartbeat) {
                    // custom_mode, type quadrotor, autopilot PX4, base_mode, state active, version
                    val payload = le(9).putInt(0).put(2).put(12).put(0).put(4).put(3)
                    send(HEARTBEAT_ID, HEARTBEAT_CRC_EXTRA, payload.array())
                    nextHeartbeat = java.lang.System.nanoTime() + 200_000_000L
                }
                val packet = DatagramPacket(buffer, buffer.size)
                try {
                    socket.receive(packet)
                } catch (_: SocketTimeoutException) {
                    continue
                }
                handle(buffer.copyOf(packet.length))
            }
        }

        private fun handle(data: ByteArray) {
            // One message per datagram, unsigned MAVLink 2 only.
            if (data.size < 12 || data[0] != 0xFD.toByte()) return
            val length = data[1].toInt() and 0xFF
            val msgId =
                (data[7].toInt() and 0xFF) or
                    ((data[8].toInt() and 0xFF) shl 8) or
                    ((data[9].toInt() and 0xFF) shl 16)
            if (msgId != TIMESYNC_ID) return
            // Trailing zeros are truncated in MAVLink 2.
            val request = le(18).put(data, 10, minOf(length, 18)).rewind() as ByteBuffer
            val tc1 = request.getLong()
            val ts1 = request.getLong()
            if (tc1 != 0L) return
            val reply = le(18).putLong(nowNs()).putLong(ts1).put(data[5]).put(data[6])
            send(TIMESYNC_ID, TIMESYNC_CRC_EXTRA, reply.array())
        }

        private fun send(msgId: Int, crcExtra: Int, payload: ByteArray) {
            val frame =
                le(12 + payload.size)
                    .put(0xFD.toByte())
                    .put(payload.size.toByte())
                    .put(0)
                    .put(0)
                    .put(seq.toByte())
                    .put(1)
                    .put(1)
                    .put(msgId.toByte())
                    .put((msgId shr 8).toByte())
                    .put((msgId shr 16).toByte())
                    .put(payload)
            val crc = crcX25(frame.array().copyOfRange(1, 10 + payload.size) + crcExtra.toByte())
            frame.putShort(crc.toShort())
            socket.send(DatagramPacket(frame.array(), frame.capacity(), groundStation))
            seq = (seq + 1) and 0xFF
        }

        private fun le(size: Int) = ByteBuffer.allocate(size).order(ByteOrder.LITTLE_ENDIAN)

        private fun crcX25(data: ByteArray): Int {
            var crc = 0xFFFF
            for (byte in data) {
                var tmp = (byte.toInt() and 0xFF) xor (crc and 0xFF)
                tmp = (tmp xor (tmp shl 4)) and 0xFF
                crc = ((crc shr 8) xor (tmp shl 8) xor (tmp shl 3) xor (tmp shr 4)) and 0xFFFF
            }
            return crc
        }
    }

    private companion object {
        const val DISCOVERY_TIMEOUT_S = 5.0
        const val SYNC_TIMEOUT_MS = 3_000L
        const val SUBSCRIBE_DELAY_MS = 200L
        // Within half the round trip, plus some slack for the scheduler and the millisecond clock.
        const val OFFSET_TOLERANCE_NS = 10_000_000L

        const val HEARTBEAT_ID = 0
        const val HEARTBEAT_CRC_EXTRA = 50
        const val TIMESYNC_ID = 111
        const val TIMESYNC_CRC_EXTRA = 34
    }
}
