#pragma once

#include <string>
#include <functional>

namespace mavsdk::log {

// defined numeric values can be useful for filtering message by severity
enum class Level : int { Debug = 0, Info = 1, Warn = 2, Err = 3 };

/** @brief User-defined callback for logging. Returning true from this callback
 * prevents default mavsdk`s logging to stdout. Returning false keeps it.
 */
using Callback =
    std::function<bool(Level level, const std::string& message, const std::string& file, int line)>;

extern Callback& get_callback();
extern void subscribe(const Callback& callback);

} // namespace mavsdk::log
