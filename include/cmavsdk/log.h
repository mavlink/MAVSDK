#ifndef CMAVSDK_LOG_H
#define CMAVSDK_LOG_H

#include "mavsdk_export.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Log levels for MAVSDK
 */
typedef enum {
    MAVSDK_LOG_LEVEL_DEBUG = 0,
    MAVSDK_LOG_LEVEL_INFO = 1,
    MAVSDK_LOG_LEVEL_WARN = 2,
    MAVSDK_LOG_LEVEL_ERROR = 3
} mavsdk_log_level_t;

/**
 * @brief Callback function for log messages
 * 
 * @param level The log level
 * @param message The log message
 * @param file The source file where the log originated (can be NULL if not available)
 * @param line The line number in the source file (can be 0 if not available)
 * @param user_data User-provided context pointer
 * 
 * @return true to prevent default logging to stdout, false to keep default logging
 */
typedef bool (*mavsdk_log_callback_t)(
    mavsdk_log_level_t level, 
    const char* message, 
    const char* file, 
    int line, 
    void* user_data);

/**
 * @brief Subscribe to MAVSDK log messages
 * 
 * @param callback The callback function to call for each log message
 * @param user_data User context pointer that will be passed to the callback
 * 
 * @note Only one log callback can be active at a time. Subsequent calls will replace the previous callback.
 * @note The callback will be called from various threads, so it must be thread-safe.
 */
CMAVSDK_EXPORT void mavsdk_log_subscribe(mavsdk_log_callback_t callback, void* user_data);

/**
 * @brief Unsubscribe from log messages
 * 
 * This will remove the current log callback and restore default logging behavior.
 */
CMAVSDK_EXPORT void mavsdk_log_unsubscribe();

#ifdef __cplusplus
}
#endif

#endif // CMAVSDK_LOG_H
