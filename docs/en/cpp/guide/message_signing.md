# MAVLink Message Signing

MAVSDK can sign the MAVLink 2 messages it sends and check the signature of the ones it receives, as described in [MAVLink Message Signing](https://mavlink.io/en/guide/message_signing.html).
This makes sure that only systems with the shared secret key can command a vehicle or feed data to the application.

::: info
Signing authenticates messages, it does not encrypt them: anyone listening can still read them.
:::

## Usage

Signing is disabled by default. It is enabled with a 32 byte secret key, which is shared by all systems on the network:

```cpp
std::array<uint8_t, Mavsdk::SIGNING_KEY_LENGTH> secret_key = load_key_from_somewhere();
mavsdk.enable_signing(secret_key);

// ...

mavsdk.disable_signing();
```

Once enabled, for all connections:

- All outgoing MAVLink 2 messages are signed. MAVLink 1 messages can't be signed and are sent as they are.
- Incoming messages without a valid signature are dropped, including signed messages that are replayed.
  The exceptions are `HEARTBEAT`, `RADIO_STATUS`, `ADSB_VEHICLE` and `COLLISION`, which are accepted unsigned, the same as PX4 does.
  `HEARTBEAT` is needed to discover a system, and the others come from radios and ADS-B receivers which can't sign.

Disabling signing again sends messages unsigned and accepts all incoming messages.

MAVSDK does not store the key, so the application has to enable signing again on each start.

## Setting up the key on the vehicle

The vehicle has to know the same key. How it gets there is up to the application: it can be sent with a [SETUP_SIGNING](https://mavlink.io/en/messages/common.html#SETUP_SIGNING) message, e.g. using [MavlinkDirect](mavlink_direct.md), over a secure link such as USB, before enabling signing in MAVSDK.
PX4 stores the key and signs from then on.

::: warning
Once the vehicle signs, it drops unsigned commands. If the key is lost, it has to be reset on the vehicle directly.
:::

## Language bindings

The same functions are available in the native bindings:

- C: `mavsdk_enable_signing()`, `mavsdk_disable_signing()`, `mavsdk_is_signing_enabled()`
- Python: `Mavsdk.enable_signing(secret_key: bytes)`, `disable_signing()`, `is_signing_enabled()`
- Kotlin: `Mavsdk.enableSigning(secretKey: ByteArray)`, `disableSigning()`, `isSigningEnabled()`

Signing is not available through *mavsdk_server*.
