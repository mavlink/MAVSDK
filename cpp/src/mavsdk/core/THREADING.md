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
- `SystemImpl::_plugin_impls` is io-thread-only, and so is every `enable()`/`disable()` call.
  See **Plugin lifetime** below -- this one is load-bearing, because the entries are raw
  pointers to objects that user code owns and destroys.

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

- `MavsdkImpl::schedule_timers_poll()` — runs `TimeoutHandler::run_once()` and
  `CallEveryHandler::run_once()`. **Deadline-driven**: it asks both handlers for their next
  deadline and arms for that, so a timeout fires at its deadline rather than at the next
  tick after it. `add()` and `refresh()` call back into `MavsdkImpl::wake_timers_poll()`
  when they move the earliest deadline earlier, which re-arms from whatever thread they were
  called on. The wait is capped at `MAX_TIMERS_POLL_WAIT` so that a wakeup we somehow miss
  can only make a timer late, never stall it.
- `SystemImpl::schedule_ping()` — the 5 s ping, as a `CallEveryHandler` entry.
- `Timesync` registers its own `CallEveryHandler` entry in `enable()`.

Nothing polls any more. The protocol handlers used to be driven by two fixed-cadence
`do_work()` chains (`ServerComponentImpl` at 10 ms, `SystemImpl` at 10/100 ms) that ran
whether or not their queues had anything in them. Each work queue now runs its own
`do_work()` when something happens to it instead:

- **On enqueue** — every queue already did this (`if (was_empty) do_work();` inside the
  posted enqueue). `MavlinkCommandSender` is the exception and kicks unconditionally,
  because its `do_work()` walks the whole queue rather than just the front item.
- **On completion** — the FTP client, the parameter client and the parameter server already
  posted `do_work()` wherever they retired an item.

Two places did not, and needed the poll to make progress:

- `MavlinkMissionTransferClient` / `...Server` only retire the front item once it reports
  `is_done()`, and nothing told them when that became true. `WorkItem::set_done()` now
  notifies the owner. It runs with the item's `_mutex` held (every caller reaches it through
  `callback_and_reset()`, which holds it), so the notification must only *post* — never
  block, and never call `do_work()` inline, since retiring the item drops the last reference
  to the object we are standing on.
- `MavlinkCommandSender::do_work()` skips a queued command while another with the same
  command id is in flight, and nothing re-ran it when the first one finished. Both
  `receive_command_ack()` and `receive_timeout()` now post one.

**If you add a work queue, or a path that retires an item, it has to kick `do_work()`
itself.** There is no periodic sweep left to cover for a missing one — the symptom is work
that sits in the queue forever.

## Plugin lifetime

Plugins are the one place where the io thread holds a bare pointer to an object *user code*
owns: `SystemImpl::_plugin_impls` is a `std::vector<PluginImplBase*>`, and the plugin is
destroyed on whatever thread the user drops it on. Meanwhile the io thread calls `enable()` on
every entry when the system connects and `disable()` when it times out.

So the registry is io-thread-only, and the two ends are asymmetric on purpose:

- `register_plugin()` **posts** the insertion (and the `if (_connected) enable()` that goes
  with it). Posting rather than waiting means a plugin constructor can never block on the io
  thread. Doing the `_connected` test there, rather than on the caller's thread, is what makes
  it atomic against `set_connected()`/`set_disconnected()` — otherwise a plugin registering
  just as the system connects gets `enable()`d twice, or not at all.
- `unregister_plugin()` **posts and waits** for the removal, and only then calls `disable()`
  and `deinit()`. Once the removal has run, the io thread cannot reach the plugin and any
  `enable()`/`disable()` it had started has finished, because the removal is serialized behind
  them on that same thread.

Both of those matter. Before they were in place, ThreadSanitizer reported two races on a
plugin's own members in `system_tests/plugin_lifetime_churn.cpp`: `register_plugin()` calling
`enable()` on the caller's thread against the io thread's `disable()`, and the io thread's
`enable()` against a user thread inside the plugin's destructor. The second is a
use-after-free waiting to happen.

**If you add anything else the io thread reaches through a raw pointer into user-owned memory,
it needs the same treatment.**

## User callbacks: two classes

Everything the user sees arrives through one bounded queue drained by the user-callback
thread, and *blocking API calls depend on it*. `Action::arm()` waits on a promise that is
only satisfied when the command-result callback runs, so a result that gets dropped is not a
gap in data -- it is `arm()` never returning.

So the queue distinguishes two kinds of work:

| | Use | Dropped? |
|---|---|---|
| `call_user_callback()` | Anything somebody may be waiting for: command results, mission and parameter completions, one-shot requests. | **Never.** These are one per user action, so they cannot run the queue away. |
| `call_user_callback_droppable()` | A subscription delivering a stream, where the newest value is what matters. | Yes, once `MAX_DROPPABLE_USER_CALLBACKS` of them are queued. |

Must-deliver is the default, so a new call site is safe until somebody deliberately opts into
dropping. When the droppable bound is reached, the **oldest** droppable entry is evicted
rather than the new one refused, so a subscriber that cannot keep up sees fresh data with
gaps instead of falling further and further behind.

Drops are counted and reported at most every few seconds with a running total, rather than
per drop -- whatever causes drops causes a great many of them.

`system_tests/callback_queue_overflow.cpp` pins the property that matters: a slow subscriber
must not stop a blocking call from returning.

## Object lifetime

`System`, `ServerComponent` and every plugin hold a reference back into `MavsdkImpl`, so none
of them may outlive the `Mavsdk` instance they came from. That is not just a rule about
*using* them afterwards: `~SystemImpl` itself reaches back in (`remove_call_every_blocking()`,
`unregister_timeout_handler_blocking()`), so a leaked `System` is a use-after-free even if it
is never touched again.

Since there is no harmless version of getting it wrong, `~MavsdkImpl` looks for outstanding
references and aborts with an explanation rather than leaving a segfault to surface elsewhere
later — see `MavsdkImpl::abort_if_references_outlive_us()` and
`system_tests/mavsdk_outlived.cpp`. Note that a couple of those references are our own
(`_default_server_component` points at an entry that is also in `_server_components`), so the
check compares against a baseline rather than against 1.

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

Per-connection and per-system locks (`_send_mutex`, `_remote_mutex`, `_components_mutex`, …)
are local to their object and do not participate in the order above.

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
