# Logging
MAVSDK core and plugins output some useful log messages during their work.
By default, the messages are printed to `stdout`. You may want to override this
behavior, e.g. redirect messages to an external logging system, or disable
printing messages unless they are important enough.

## Usage
It is possible to customize logging with a user-defined callback function:
```c++
#include <mavsdk/log_callback.h>

// ...

mavsdk::log::subscribe([](mavsdk::log::Level level,   // message severity level
                          const std::string& message, // message text
                          const std::string& file,    // source file from which the message was sent
                          int line) {                 // line number in the source file
  // process the log message in a way you like
  my_nice_log(level, message);
  
  // returning true from the callback disables printing the message to stdout
  return level < mavsdk::log::Level::Warn;
});
```

In this example all log messages will be passed to `my_nice_log`, and all messages
having level `Warn` or `Err` will be printed to stdout as well.
Possible levels are:
```c++
enum class Level : int { Debug = 0, Info = 1, Warn = 2, Err = 3 };
```
Currently, only one callback is supported. Calling `mavsdk::log::subscribe`
the second time will overwrite previous callback. To unsubscribe (revert to default
logging behaviour) call subscribe with a `nullptr` callback:
```c++
mavsdk::log::subscribe(nullptr);
```

## Important notes
Please note that callback can be called from different threads. Synchronization
is not handled on MAVSDK's side for performance reasons. You may want to
implement synchronization yourself if your logging system isn't thread
safe already.

Avoid doing long duration heavy tasks in the callback, it should return as
quickly as possible.
