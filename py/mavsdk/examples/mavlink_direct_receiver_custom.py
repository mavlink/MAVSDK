# Example to use MavlinkDirect to receive a custom message (GAS_SENSOR) which
# is not part of MAVLink, by loading its XML definition at runtime.
#
# Use together with the mavlink_direct_sender_custom example.
#
# MavlinkDirect represents the message fields as JSON, so we use the json
# module from the standard library to read them.

import json
import time

from mavsdk import Mavsdk, Configuration, ComponentType
from mavsdk.plugins.mavlink_direct import MavlinkDirect

# Same definition as on the sending side. We need it to make sense of the message.
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


def on_gas_sensor(message, user_data=None):
    fields = json.loads(message.fields_json)
    # The temperature is in cdegC, as given by the message definition.
    print(
        f"GAS_SENSOR {fields['id']} received: "
        f"{fields['co2']:.1f} ppm CO2, {fields['ch4']:.1f} ppm CH4, "
        f"{fields['temperature'] / 100:.1f} degC"
    )


def main():
    # Set up as ground station
    mavsdk = Mavsdk(Configuration.create_with_component_type(ComponentType.GROUND_STATION))
    mavsdk.add_any_connection("udpin://0.0.0.0:14550")

    print("Waiting for an autopilot...")
    drone = mavsdk.first_autopilot(timeout_s=10.0)
    if drone is None:
        print("No autopilot found")
        return

    mavlink_direct = MavlinkDirect(drone)

    mavlink_direct.load_custom_xml(GAS_SENSOR_XML)
    print("Custom XML loaded successfully")

    mavlink_direct.subscribe_message("GAS_SENSOR", on_gas_sensor)

    print("Waiting for GAS_SENSOR messages. Press Ctrl+C to exit...")
    while True:
        time.sleep(1)


if __name__ == "__main__":
    main()
