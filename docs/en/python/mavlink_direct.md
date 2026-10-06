# MavlinkDirect

The MavlinkDirect plugin lets you send and receive any MAVLink message, including your own custom ones, by name.
The message fields are passed as a JSON string, which makes it a good fit for Python: the `json` module from the standard library does all the work.

For the concepts, such as the JSON format and how custom XML is loaded, see the [C++ guide](../cpp/guide/mavlink_direct.md). The Python API is the same.

## Loading a custom message

To send or receive a message that is not part of the [common.xml](https://mavlink.io/en/messages/common.html) dialect, load its XML definition first.
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

Both the sending and the receiving side need to load the same XML.

## Sending messages

Once loaded, the message can be sent by name, with the fields assembled as a `dict` and serialized with `json.dumps`:

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
    system_id=configuration.system_id,
    component_id=configuration.component_id,
    target_system_id=0,  # Does not apply for this message
    target_component_id=0,  # Does not apply for this message
    fields_json=json.dumps(fields),
)

await mavlink_direct.send_message(message)
```

Failures, such as an unknown message name or a field that does not fit, raise a `MavlinkDirectError`.

## Receiving messages

On the receiving side, subscribe to the message by name and parse its fields with `json.loads`:

```python
async for message in mavlink_direct.subscribe_message("GAS_SENSOR"):
    fields = json.loads(message.fields_json)
    # The temperature is in cdegC, as given by the message definition.
    print(
        f"GAS_SENSOR {fields['id']} received: "
        f"{fields['co2']:.1f} ppm CO2, {fields['ch4']:.1f} ppm CH4, "
        f"{fields['temperature'] / 100:.1f} degC"
    )
```

Use an empty string `""` as the message name to receive all messages.

## Examples

The complete examples are available for the asyncio API ([sender](https://github.com/mavlink/MAVSDK/blob/main/py/mavsdk/examples/asyncio/mavlink_direct_sender_custom.py), [receiver](https://github.com/mavlink/MAVSDK/blob/main/py/mavsdk/examples/asyncio/mavlink_direct_receiver_custom.py)) and for the synchronous API ([sender](https://github.com/mavlink/MAVSDK/blob/main/py/mavsdk/examples/mavlink_direct_sender_custom.py), [receiver](https://github.com/mavlink/MAVSDK/blob/main/py/mavsdk/examples/mavlink_direct_receiver_custom.py)).
