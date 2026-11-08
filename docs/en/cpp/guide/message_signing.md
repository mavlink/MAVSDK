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
  This includes `HEARTBEAT`, so a system that doesn't sign with the same key is not discovered at all.
  It also includes `RADIO_STATUS` injected by radios that can't sign, such as SiK radios.

- Dropped messages are not forwarded either, so with [message forwarding](connections.md#forwarding-mavlink-between-connections) MAVSDK only passes on correctly signed messages.
- Dropped messages are logged as a warning with the reason (e.g. bad signature, not signed, timestamp too old), at most once every 5 seconds.
  Only the unsigned `RADIO_STATUS` of radios is logged at debug level, as it's expected.

Disabling signing again sends messages unsigned and accepts all incoming messages.

MAVSDK does not store the key, so the application has to enable signing again on each start.

## Timestamps and replay protection

Each signed message carries a timestamp, and MAVSDK only accepts messages from a system whose timestamps keep increasing.
MAVSDK signs with the current system time, so the computer it runs on needs a correct clock.

A system that MAVSDK hasn't heard from yet is accepted if its timestamp is less than one minute behind MAVSDK's.
If the vehicle's signing timestamp is further behind, MAVSDK drops its messages at first, and logs that the timestamp is behind.
It sends the vehicle heartbeats meanwhile: a vehicle usually moves its timestamp forward to the newest one it receives, after which it is accepted.
Such a message might also be a replay, so over UDP the sender is not added as a destination until its messages are accepted: it only gets a heartbeat back, at most once a second.
With PX4 this takes about a second.

What MAVSDK has seen is kept in memory only.
After a restart, after `disable_signing()`, or after enabling signing with a different key, a message recorded within the last minute could be replayed once.
Calling `enable_signing()` again with the same key while signing is enabled keeps what it has seen.

## Setting up the key on the vehicle

The vehicle has to know the same key. How it gets there is up to the application: it can be sent with a [SETUP_SIGNING](https://mavlink.io/en/messages/common.html#SETUP_SIGNING) message, e.g. using [MavlinkDirect](mavlink_direct.md), over a secure link such as USB, before enabling signing in MAVSDK.
PX4 stores the key and signs from then on, starting at the `initial_timestamp` of `SETUP_SIGNING`, so set that to the current time.
A `SETUP_SIGNING` with an all-zero key disables signing on PX4 again.

::: info
PX4 accepts some messages unsigned even when signing: `HEARTBEAT`, `RADIO_STATUS`, `ADSB_VEHICLE` and `COLLISION`.
MAVSDK does not make these exceptions.
:::

::: warning
Once the vehicle signs, it drops unsigned commands. If the key is lost, it has to be reset on the vehicle directly.
:::

## Language bindings

The same functions are available in the native bindings:

- C: `mavsdk_enable_signing()`, `mavsdk_disable_signing()`, `mavsdk_is_signing_enabled()`
- Python: `Mavsdk.enable_signing(secret_key: bytes)`, `disable_signing()`, `is_signing_enabled()`
- Kotlin: `Mavsdk.enableSigning(secretKey: ByteArray)`, `disableSigning()`, `isSigningEnabled()`

Signing is not available through *mavsdk_server*.
