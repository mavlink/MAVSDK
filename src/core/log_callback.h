#pragma once

#include <string>
#include <functional>

namespace mavsdk::log {

// defined numeric values can be useful for filtering message by severity
typedef enum {
  Debug = 0,
  Info = 1,
  Warn = 2,
  Err = 3
} Level;

/** @brief User-defined callback for logging. Returning true from this callback
 * prevents default mavsdk`s logging to stdout. Returning false keeps it.
 */
typedef std::function<bool(Level level, const std::string &message,
                           const std::string &file, int line)> Callback;

extern Callback callback;

}
