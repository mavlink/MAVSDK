# Heartbeat Watchdog

MAVSDK sends its own heartbeats whenever a system is connected or [Configuration::set_always_send_heartbeats()](../api_reference/classmavsdk_1_1_mavsdk_1_1_configuration.md) is set, and it keeps sending them for as long as the `Mavsdk` object exists — even if the application around it has hung or deadlocked.
A peer therefore cannot tell a working application from a stuck one.

The heartbeat watchdog is a deadman timer that ties those heartbeats to application liveness instead: they are only sent while the application keeps calling [Mavsdk::feed_heartbeat_watchdog()](../api_reference/classmavsdk_1_1_mavsdk.md).
If the application stops feeding it, the heartbeats stop, and the peer sees the connection drop.

::: info
This is an opt-in feature, disabled by default. Most applications do not need it — reach for it when something on the other end of the link needs to act on the application being alive, e.g. an autopilot configured to fail-safe on GCS link loss.
:::

## Usage

The watchdog can be configured at startup, or enabled and changed later at runtime:

```cpp
Mavsdk::Configuration config(ComponentType::GroundStation);
config.set_heartbeat_watchdog_timeout_s(2.0); // 0 disables it (the default)
Mavsdk mavsdk(config);

mavsdk.set_heartbeat_watchdog_timeout_s(2.0); // ... or at runtime

// From wherever the application proves it is still healthy:
mavsdk.feed_heartbeat_watchdog();
```

Both setters return `false` and keep the previous value if the timeout is invalid — it must be either 0 (disabled) or at least 1 second.

Heartbeats do not start until the watchdog has been fed at least once, and enabling the watchdog or changing its timeout stops them again until the next feed.
The watchdog never grants a free timeout period.
Once it has expired, heartbeats stay off until it is fed again, even if `set_always_send_heartbeats()` would otherwise allow them.

::: tip
A feed stays valid until the timeout elapses, including across system disconnect/reconnect and across periods where heartbeats are not being sent anyway.
Feeding does not *start* heartbeats that are off for other reasons: with `always_send_heartbeats` unset and no system connected, nothing is sent regardless of the watchdog.
:::

## How often to feed

MAVSDK sends its periodic heartbeats at 1 Hz and checks the watchdog at each of those send opportunities, so a heartbeat goes out only if the last feed was less than the timeout ago.

Feed at least *twice* per timeout period rather than exactly once.
If a feed lands slightly later than usual the next heartbeat is skipped, which leaves a gap of 2 s or more in the outgoing stream, and sustained jitter can make the peer declare the connection lost when the application is in fact fine.

::: warning
The minimum timeout is 1 s, but 1 s leaves no headroom for an application feeding at 1 Hz — the feed and the heartbeat can end up almost in phase, so a single late feed already drops a heartbeat.
If the application can only feed about once per second, use a timeout of 2 s or more.
:::

Feed from somewhere that genuinely stops if the application is unhealthy.
Feeding from a dedicated timer thread that keeps running regardless defeats the purpose: it will keep the heartbeats alive while the rest of the application is deadlocked.

## Use from language wrappers

::: info
The rest of this page applies only to the language wrappers that drive MAVSDK through *mavsdk_server* over gRPC (e.g. Swift, Java, and the gRPC-based Python wrapper).
Bindings that link `libmavsdk` directly do not go through mavsdk_server; they expose the watchdog through the C/C++ API above, and need their own binding for it.
:::

For gRPC-based wrappers the watchdog lives on the mavsdk_server side, since that is the process holding the `Mavsdk` object and sending the heartbeats.
It can be enabled either when starting the server:

```
mavsdk_server --heartbeat-watchdog-timeout 2 udpin://0.0.0.0:14540
```

or at runtime with the `SetHeartbeatWatchdogTimeout` RPC on the core service. Feeding is then done with the `FeedHeartbeatWatchdog` RPC.

Note what this arrangement does and does not cover: the watchdog tracks the liveness of whatever calls `FeedHeartbeatWatchdog`, so it detects a hung *application*, and it also covers the application losing its gRPC connection to mavsdk_server. It does not detect mavsdk_server itself hanging, because a stuck server stops sending heartbeats anyway.

::: info
The RPCs are only present in wrappers generated from a MAVSDK-Proto version that includes them. An older generated wrapper talking to a newer mavsdk_server will not have the methods.
:::
