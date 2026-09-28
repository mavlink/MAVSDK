# MavlinkDirect

The MavlinkDirect plugin lets you send and receive any MAVLink message, including your own custom ones, by name.
The message fields are passed as a JSON string, which makes it a good fit for Python: the `json` module from the standard library does all the work.

For the concepts, such as the JSON format and how custom XML is loaded, see the [C++ guide](../cpp/guide/mavlink_direct.md). The Python API is the same.

## Receiving messages

Subscribe to a message by name and parse its fields with `json.loads`:

```python
import asyncio
import json

from mavsdk.asyncio import Mavsdk, Configuration, ComponentType
from mavsdk.asyncio.plugins.mavlink_direct import MavlinkDirectAsync


async def main():
    configuration = Configuration.create_with_component_type(ComponentType.COMPANION_COMPUTER)
    # Our own IDs, needed to send messages. Read them before creating Mavsdk,
    # as it takes ownership of the configuration.
    system_id = configuration.system_id
    component_id = configuration.component_id
    mavsdk = Mavsdk(configuration)
    await mavsdk.add_any_connection("udpin://0.0.0.0:14540")

    drone = await mavsdk.first_autopilot(timeout_s=10.0)
    if drone is None:
        raise SystemExit("No autopilot found")

    mavlink_direct = MavlinkDirectAsync(drone)

    async for message in mavlink_direct.subscribe_message("GLOBAL_POSITION_INT"):
        fields = json.loads(message.fields_json)
        # The units are the ones from the MAVLink message definition:
        # degrees * 1e7, and millimeters above the home position.
        print(
            f"Position: {fields['lat'] / 1e7:.6f}, {fields['lon'] / 1e7:.6f}"
            f" at {fields['relative_alt'] / 1e3:.1f} m"
        )


asyncio.run(main())
```

Use an empty string `""` as the message name to receive all messages.

## Sending a custom message

To send a message that is not part of the [common.xml](https://mavlink.io/en/messages/common.html) dialect, load its XML definition first.
In this example we define our own message for a gas sensor:

```python
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

await mavlink_direct.load_custom_xml(GAS_SENSOR_XML)
```

Then the message can be sent by name, with the fields assembled as a `dict` and serialized with `json.dumps`:

```python
from mavsdk.asyncio.plugins.mavlink_direct import MavlinkMessage

fields = {
    "time_usec": 1_000_000,
    "id": 0,
    "co2": 420.0,
    "ch4": 1.9,
    "temperature": 2150,
}

message = MavlinkMessage(
    message_name="GAS_SENSOR",
    system_id=system_id,
    component_id=component_id,
    target_system_id=0,  # Does not apply for this message
    target_component_id=0,  # Does not apply for this message
    fields_json=json.dumps(fields),
)

await mavlink_direct.send_message(message)
```

Failures, such as an unknown message name or a field that does not fit, raise a `MavlinkDirectError`.

::: info
The receiving side needs to know about the message as well. If it uses MAVSDK, it has to load the same XML.
:::

## Examples

The complete example is available for the [asyncio API](https://github.com/mavlink/MAVSDK/blob/main/py/mavsdk/examples/asyncio/mavlink_direct.py) and for the [synchronous API](https://github.com/mavlink/MAVSDK/blob/main/py/mavsdk/examples/mavlink_direct.py).
