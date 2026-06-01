# Troubleshooting

## User callbacks {#user_callbacks}

All callbacks back to the user of the library are called from one thread. The callbacks are stored in a queue. If the user does not return quickly enough from the callback, it can happen that the user callback queue fills up.

If a callback takes more than one second, the user sees the warning:
```
[02:56:26|Warn ] Callback took more than 1 second to run.
See: https://mavsdk.mavlink.io/main/en/cpp/troubleshooting.html#user_callbacks (system_impl.cpp:327)
```

At 10 queued callbacks, the user sees the warning:
```
[02:56:26|Warn ] User callback queue too slow.
See: https://mavsdk.mavlink.io/main/en/cpp/troubleshooting.html#user_callbacks (system_impl.cpp:1213)
```
At 100 queued callbacks, the user sees the error message:
```
[02:56:35|Error] User callback queue overflown
See: https://mavsdk.mavlink.io/main/en/cpp/troubleshooting.html#user_callbacks (system_impl.cpp:1218)
```

### How to debug this?

To determine which callback is blocking the queue, you can set the environment variable `MAVSDK_CALLBACK_DEBUGGING` to `1`, which will print more debug information and abort the program as soon as the problem occurs.

```
MAVSDK_CALLBACK_DEBUGGING=1 ./my_executable_using_mavsdk
```
or:
```
export MAVSDK_CALLBACK_DEBUGGING=1
./my_executable_using_mavsdk
```

You should then see something like this hinting at which callback might be responsible:
```
[03:00:02|Warn ] Callback called from telemetry_impl.cpp:533 took more than 1 second to run. (system_impl.cpp:320)
[2]    1261673 abort (core dumped)  MAVSDK_CALLBACK_DEBUGGING=1  ..."
```

Note that this does not point to the blocking code, but only to the place where your blocking callback was called.

### How to avoid this?

The rule is to spend as little time and CPU in the callbacks as possible.
For instance, inside the callbacks you should not wait on other events or sleep.

If you really want to do something that takes longer inside a callback, the workaround is to just spawn that activity on another thread, e.g. using `std::thread`:

```
void my_callback(Telemetry::Position position)
{
    std::thread([position]() {
        std::this_thread_sleep_for(std::chrono::seconds(3));
        my_delayed_action(position);
    }).detach();
}
```

### Why did this use to work?

Before introducing the callback queue, we used a thread pool of 3 threads to call user callbacks.
The thread pool was written to avoid everything from stalling if a user blocked in one of the callbacks.
It generally worked provided the user did not block in more than 3 callbacks at any given time.

However, it had also some disadvantages:

- Due to the thread pool multiple threads could call a function at any given time.
  This means that one callback can be called from at least two threads at the same time which is quite likely if the callback is called at a high rate.
  This can create race conditions on the user side unless a mutex is used to prevent this.
  However, it's not very intuitive that a mutex is needed for that, and mutexes in general should be avoided, if possible, for performance reasons.
  Such race conditions also showed up as CI test failures, see: [MAVSDK#1010](https://github.com/mavlink/MAVSDK/issues/1010), [MAVSDK#1045](https://github.com/mavlink/MAVSDK/issues/1045).
- For developers the old thread pool behaviour was unclear and unpredictable.
  Basically, without code inspection, it was difficult to know if blocking in a callback is safe or not.
  When trying with one blocking callback, it would work, however, this can create a false sense of security as with 3 blocking callbacks it would suddenly lock up.
- Previously, when this happened, it was difficult to debug because it would either lead to a crash on destruction, or lock up at which point the backtrace of all threads need to be looked at using gdb.
