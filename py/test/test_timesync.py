"""System test: time synchronization state with the ``mavsdk`` bindings.

A MAVSDK instance does not answer TIMESYNC until it is synced itself, so the
autopilot here is a raw UDP socket that answers with a clock starting at boot,
like PX4 does.
"""

import asyncio
import socket
import struct
import threading
import time

from mavsdk import ComponentType, Configuration, Mavsdk
from mavsdk import asyncio as mavsdk_asyncio

DISCOVERY_TIMEOUT_S = 5.0
SYNC_TIMEOUT_S = 3.0
# Within half the round trip, plus some slack for the scheduler.
OFFSET_TOLERANCE_NS = 10_000_000

HEARTBEAT_ID, HEARTBEAT_CRC_EXTRA = 0, 50
TIMESYNC_ID, TIMESYNC_CRC_EXTRA = 111, 34


def crc_x25(data):
    crc = 0xFFFF
    for byte in data:
        tmp = byte ^ (crc & 0xFF)
        tmp = (tmp ^ (tmp << 4)) & 0xFF
        crc = ((crc >> 8) ^ (tmp << 8) ^ (tmp << 3) ^ (tmp >> 4)) & 0xFFFF
    return crc


class FakeAutopilot:
    def __init__(self, groundstation_port):
        self._boot_ns = time.time_ns() - 100 * 1_000_000_000
        self._groundstation = ("127.0.0.1", groundstation_port)
        self._socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self._socket.bind(("127.0.0.1", 0))
        self._socket.settimeout(0.05)
        self._seq = 0
        self._running = True
        self._thread = threading.Thread(target=self._run, daemon=True)
        self._thread.start()

    @property
    def expected_offset_ns(self):
        """Autopilot time minus local time."""
        return -self._boot_ns

    def stop(self):
        self._running = False
        self._thread.join()
        self._socket.close()

    def _send(self, msgid, crc_extra, payload):
        header = struct.pack(
            "<BBBBBB", len(payload), 0, 0, self._seq, 1, 1
        ) + struct.pack("<I", msgid)[:3]
        crc = crc_x25(header + payload + bytes([crc_extra]))
        self._socket.sendto(
            b"\xfd" + header + payload + struct.pack("<H", crc), self._groundstation
        )
        self._seq = (self._seq + 1) % 256

    def _run(self):
        next_heartbeat = 0.0
        while self._running:
            if time.monotonic() >= next_heartbeat:
                # custom_mode, type quadrotor, autopilot PX4, base_mode, state active, version
                self._send(
                    HEARTBEAT_ID, HEARTBEAT_CRC_EXTRA, struct.pack("<IBBBBB", 0, 2, 12, 0, 4, 3)
                )
                next_heartbeat = time.monotonic() + 0.2
            try:
                data, _ = self._socket.recvfrom(2048)
            except socket.timeout:
                continue
            self._handle(data)

    def _handle(self, data):
        # One message per datagram, unsigned MAVLink 2 only.
        if len(data) < 12 or data[0] != 0xFD:
            return
        length = data[1]
        sysid, compid = data[5], data[6]
        msgid = int.from_bytes(data[7:10], "little")
        if msgid != TIMESYNC_ID:
            return
        # Trailing zeros are truncated in MAVLink 2.
        payload = data[10 : 10 + length].ljust(18, b"\0")
        tc1, ts1 = struct.unpack("<qq", payload[:16])
        if tc1 != 0:
            return
        now_ns = time.time_ns() - self._boot_ns
        self._send(
            TIMESYNC_ID,
            TIMESYNC_CRC_EXTRA,
            struct.pack("<qqBB", now_ns, ts1, sysid, compid),
        )


def make_groundstation(port):
    groundstation = Mavsdk(
        Configuration.create_with_component_type(ComponentType.GROUND_STATION)
    )
    groundstation.add_any_connection(f"udpin://127.0.0.1:{port}")
    return groundstation


def test_timesync_status():
    groundstation = make_groundstation(17050)
    autopilot = FakeAutopilot(17050)
    try:
        system = groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
        assert system is not None

        status = system.timesync_status()
        assert not status.synced
        assert status.offset_ns == 0

        synced = threading.Event()
        received = []

        def on_status(status, _user_data):
            received.append(status)
            if status.synced:
                synced.set()

        handle = system.subscribe_timesync_status(on_status)
        system.enable_timesync()
        assert synced.wait(SYNC_TIMEOUT_S)
        system.unsubscribe_timesync_status(handle)

        reported = next(status for status in received if status.synced)
        assert 0 < reported.rtt_ns < 10_000_000
        assert abs(reported.offset_ns - autopilot.expected_offset_ns) < OFFSET_TOLERANCE_NS

        status = system.timesync_status()
        assert status.synced
        assert abs(status.offset_ns - autopilot.expected_offset_ns) < OFFSET_TOLERANCE_NS
    finally:
        autopilot.stop()
        groundstation.destroy()


def test_timesync_status_async():
    async def scenario():
        groundstation = mavsdk_asyncio.Mavsdk(
            mavsdk_asyncio.Configuration.create_with_component_type(
                mavsdk_asyncio.ComponentType.GROUND_STATION
            )
        )
        await groundstation.add_any_connection("udpin://127.0.0.1:17051")
        autopilot = FakeAutopilot(17051)
        try:
            system = await groundstation.first_autopilot(DISCOVERY_TIMEOUT_S)
            assert system is not None

            updates = system.timesync_status_updates()
            # Subscribe before enabling, so that the first answer isn't missed.
            first = asyncio.ensure_future(updates.__anext__())
            await asyncio.sleep(0)
            await system.enable_timesync()

            status = await asyncio.wait_for(first, SYNC_TIMEOUT_S)
            assert status.synced
            assert (
                abs(status.offset_ns - autopilot.expected_offset_ns)
                < OFFSET_TOLERANCE_NS
            )
            await updates.aclose()

            assert (await system.timesync_status()).synced
        finally:
            autopilot.stop()
            groundstation.destroy()

    asyncio.run(scenario())
