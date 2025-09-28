#include "log.h"
#include <mavsdk/log_callback.h>
#include <string>

using namespace mavsdk;

// Internal wrapper to handle C++ callback with user data
struct LogCallbackWrapper {
    mavsdk_log_callback_t user_callback;
    void* user_data;
};

static LogCallbackWrapper* current_log_wrapper = nullptr;

// C++ callback that bridges to the C callback
static bool log_callback_bridge(log::Level level, const std::string& message, const std::string& file, int line) {
    if (!current_log_wrapper || !current_log_wrapper->user_callback) {
        return false; // Keep default logging
    }

    // Convert C++ log level to C log level
    mavsdk_log_level_t c_level;
    switch (level) {
        case log::Level::Debug:
            c_level = MAVSDK_LOG_LEVEL_DEBUG;
            break;
        case log::Level::Info:
            c_level = MAVSDK_LOG_LEVEL_INFO;
            break;
        case log::Level::Warn:
            c_level = MAVSDK_LOG_LEVEL_WARN;
            break;
        case log::Level::Err:
            c_level = MAVSDK_LOG_LEVEL_ERROR;
            break;
        default:
            c_level = MAVSDK_LOG_LEVEL_INFO;
            break;
    }

    // Convert file to C string (can be empty)
    const char* c_file = file.empty() ? nullptr : file.c_str();

    return current_log_wrapper->user_callback(c_level, message.c_str(), c_file, line, current_log_wrapper->user_data);
}

extern "C" {

void mavsdk_log_subscribe(mavsdk_log_callback_t callback, void* user_data) {
    // Clean up any existing subscription
    mavsdk_log_unsubscribe();

    if (callback) {
        current_log_wrapper = new LogCallbackWrapper{callback, user_data};
        log::subscribe(log_callback_bridge);
    }
}

void mavsdk_log_unsubscribe() {
    if (current_log_wrapper) {
        // Unsubscribe by setting an empty callback
        log::subscribe(nullptr);
        delete current_log_wrapper;
        current_log_wrapper = nullptr;
    }
}

} // extern "C"
