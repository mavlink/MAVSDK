# Threading in libmavsdk core

This is the one place that states the rules the core relies on. The per-file comments explain
individual decisions; this explains the shape they add up to. If you are adding code to
`core/` or to a plugin impl, read this first.

## The threads

| Thread | Created in | What runs on it |
|---|---|---|
| **io thread** | `MavsdkImpl` ctor (`mavsdk_impl.cpp`) | `asio::io_context::run()`. All socket and serial I/O, MAVLink parsing, message routing, plugin protocol state machines, and every internal timer. |
| **user-callback thread** | `MavsdkImpl` ctor | Drains `LockedQueue<UserCallback>` and invokes user-facing subscription callbacks. Replaced by a user-supplied executor if `Mavsdk::set_callback_executor()` is used. |
| **HTTP loader thread** | `HttpLoader` | Component-metadata downloads only. Talks to the rest of the SDK through callbacks. |
| user threads | — | Anything calling the public API. Blocking API calls park here on a `std::future`. |

Everything else — including the FTP burst read, which used to have a thread of its own — runs
on the io thread.

## The core invariant

> Shared mutable state is only touched on the io thread. Everything else posts onto it.

That is what removes most of the locking. Concretely:

- `MavlinkMessageHandler::_table` is only touched on the io thread. `register_*` and
  `unregister_*` post their mutation, so there is no lock at all, ordering falls out of post
  FIFO, and it is safe to (un)register from inside a callback.
- `MavlinkParameterSubscription` uses the same pattern for its subscription list.
- `CallbackListImpl` posts mutations without waiting (so they are safe to call under a lock)
  and posts-and-waits for reads (so arguments stay alive), with an inline fast path when
  already on the io thread.
- The work queues in `MavlinkCommandSender`, the mission transfer classes, the FTP client and
  the parameter clients are io-thread-only; enqueuing from elsewhere is posted.

`MavlinkMessageHandler` and `MavlinkParameterSubscription` each have a debug-only
`note_*_thread()` that records the accessing `std::thread::id` and asserts if it ever changes.
That catches invariant violations that an `asio::executor::running_in_this_thread()` assert
would miss: with header-only Asio and hidden visibility, that thread-local is instantiated
per DSO, so it is not trustworthy from inside libmavsdk when a test binary drives the
io_context from outside. `MavsdkImpl::on_io_thread()` compares against the io thread's own
recorded id and is reliable; use that when you need the check.

## post vs. dispatch

- **`asio::post`** when order matters relative to work already queued, or when you must not
  run inline. `MavsdkImpl::send_message()` posts on purpose: dispatching would let a send
  made from the io thread jump ahead of user-thread sends already in the queue.
- **`asio::dispatch`** when running inline is fine and the caller is usually already on the io
  thread. The receive path (`MavsdkImpl::receive_message()` /
  `receive_libmav_message()`) dispatches, which costs nothing for socket and serial
  connections and still posts for anything arriving on another thread.

## Unregistering: which variant guarantees what

Two variants exist everywhere, and the difference matters:

| Variant | Guarantee | Use it when |
|---|---|---|
| `unregister_all()`, `TimeoutHandler::remove()`, `CallEveryHandler::remove()` | The callback will not be *started* again. One that is running right now keeps running. | You are inside a callback, or you hold a lock the callback also takes. |
| `unregister_all_blocking()`, `remove_blocking()`, `unsubscribe_blocking()` | Once it returns, the callback is neither running nor going to run. | The owner is about to be destroyed. |

**A destructor, `deinit()` or `disable()` must use the blocking variant.** The non-blocking one
leaves a window in which the io thread dispatches into a half-destroyed object.

The blocking variants wait, so they must not be called while holding a lock the callback
itself takes. Where a teardown path needs the lock anyway, read what you need under the lock,
release it, and do the blocking removal afterwards — `LogFilesImpl::deinit()`,
`LogStreamingImpl::deinit()` and `~MavlinkRequestMessage()` all do exactly that. Removing an
entry from within its own callback does not wait on itself, so that case is fine.

`MavlinkMessageHandler::unregister_all_blocking()` additionally must not be called from
inside a message callback, because it removes directly and would invalidate the dispatch loop
in `process_message()`. Use the posted `unregister_all()` there.

## Timers

Everything time-based is driven from the io thread:

- `MavsdkImpl::schedule_timers_poll()` — every 5 ms, runs `TimeoutHandler::run_once()` and
  `CallEveryHandler::run_once()`.
- `MavsdkImpl::schedule_do_work()` — every 10 ms, `ServerComponentImpl::do_work()` for every
  server component.
- `SystemImpl::schedule_system_work()` — every 10 ms when connected, 100 ms otherwise; runs
  `do_work()` on the command sender, timesync, mission transfer, FTP and parameter clients,
  plus the 5 s ping.

These are fixed-cadence polls rather than deadline-driven timers, so timeout resolution is
capped at the 5 ms tick and an idle instance still wakes up regularly. That is a known
trade-off, not an invariant.

## Locks that remain

Locks are for state genuinely reachable from user threads. The order is documented where the
members are declared in `mavsdk_impl.hpp`; the summary:

- `_configuration_update_mutex` is the outermost lock and is never taken by the io thread.
- `_mutex` (connections and systems) is taken before `_server_components_mutex`.
- `_configuration_mutex` and `HeartbeatWatchdog::_mutex` are leaves: never held while taking
  another lock.
- `_our_system_id` and friends are cached as atomics so the getters need no lock. Otherwise
  `send_heartbeats()` would take `_server_components_mutex` then `_mutex`, inverting the io
  thread's order.

Per-connection and per-system locks (`_send_mutex`, `_remote_mutex`, `_components_mutex`,
`_plugin_impls_mutex`, …) are local to their object and do not participate in the order above.

## Teardown

The order in `MavsdkImpl` is deliberate and load-bearing:

1. `_io_context` and its work guard are the **first** members declared, so they are destroyed
   **last** and every other member can still safely post during its own destruction.
2. `~MavsdkImpl` stops the io_context and joins the io thread *before* stopping the callback
   queue and clearing systems and connections.
3. `remove_connection()` extracts the `unique_ptr` under `_mutex` and destroys it *outside*
   the lock, so a user thread cannot hold `_mutex` while waiting on an io thread that wants
   `_mutex`.
4. Each connection's `stop()` posts the close onto the io thread and waits (serialising it
   against in-flight socket access), then posts a second empty handler and waits for that too,
   so the `operation_aborted` completions have drained before members are destroyed.
   Because it waits on the io thread, `stop()` must not be called *from* it — there are
   asserts for this.

## Rules of thumb

- Do not touch io-thread-owned state from a user thread. Post the mutation instead.
- Do not call a blocking MAVSDK API from inside a MAVSDK callback: the callback thread is the
  one that would have to deliver the result.
- Do not hold a lock across I/O or across a post-and-wait.
- If a destructor stops callbacks, it uses a blocking unregister — and if it needs a lock to
  find out *what* to unregister, it does that first and unregisters after releasing it.
- A `sleep_for()` is not a synchronisation primitive. If you feel the need for one, the
  blocking variant of whatever you just called is what you actually wanted.
