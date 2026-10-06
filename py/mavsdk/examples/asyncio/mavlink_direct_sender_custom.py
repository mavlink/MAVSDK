# Example to use MavlinkDirect to send a custom message (GAS_SENSOR) which
# is not part of MAVLink, by loading its XML definition at runtime.
#
# Use together with the mavlink_direct_receiver_custom example.
#
# MavlinkDirect represents the message fields as JSON, so we use the json
# module from the standard library to write them.

import asyncio
import json
import time

from mavsdk.asyncio import Mavsdk, Configuration, ComponentType
from mavsdk.asyncio.plugins.mavlink_direct import MavlinkDirectAsync, MavlinkMessage

# Our own message for a gas sensor. It is not part of any MAVLink dialect, so
# MAVSDK does not know about it until we load its definition.
# Note that the receiving side needs to know the definition as well to make sense of it.
GAS_SENSOR_XML = """
<mavlink>
    <messages>
        <message id="44000" name="GAS_SENSOR">
            <description>Gas concentration measured by a sensor.</description>
            <field type="uint64_t" name="time_usec" units="us">Timestamp (time since system boot).</field>
            <field type="uint8_t" name="id" instance="true">Sensor ID.</field>
            <field type="float" name="co2" units="ppm">CO2 concentration.</field>
            <field type="float" name="ch4" units="ppm">Methane concentration.</field>
            <field type="int16_t" name="temperature" units="cdegC">Sensor temperature.</field>
        </message>
    </messages>
</mavlink>
"""


async def main():
    # Set up as companion computer
    configuration = Configuration.create_with_component_type(ComponentType.COMPANION_COMPUTER)
    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection("udpin://0.0.0.0:14540")

    print("Waiting for an autopilot...")
    drone = await mavsdk.first_autopilot(timeout_s=10.0)
    if drone is None:
        print("No autopilot found")
        return

    mavlink_direct = MavlinkDirectAsync(drone)

    await mavlink_direct.load_custom_xml(GAS_SENSOR_XML)
    print("Custom XML loaded successfully")

    start_time = time.monotonic()

    for counter in range(20):
        # Made-up values that change a bit over time
        fields = {
            "time_usec": int((time.monotonic() - start_time) * 1e6),
            "id": 0,
            "co2": 420.0 + counter * 5.0,
            "ch4": 1.9 + counter * 0.1,
            "temperature": 2150 + counter * 10,  # 21.5 degC and rising
        }

        message = MavlinkMessage(
            message_name="GAS_SENSOR",
            system_id=configuration.system_id,
            component_id=configuration.component_id,
            target_system_id=0,  # Does not apply for this message
            target_component_id=0,  # Does not apply for this message
            fields_json=json.dumps(fields),
        )

        await mavlink_direct.send_message(message)
        print(
            f"GAS_SENSOR message {counter + 1}/20 sent: "
            f"{fields['co2']:.1f} ppm CO2, {fields['ch4']:.1f} ppm CH4"
        )

        await asyncio.sleep(1)

    print("Sent all 20 GAS_SENSOR messages. Exiting.")


if __name__ == "__main__":
    asyncio.run(main())
