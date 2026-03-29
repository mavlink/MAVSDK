#include "log.h"
#include "unused.h"

#include <mutex>

#if defined(WINDOWS)
#include <windows.h>
#define WIN_COLOR_RED 4
#define WIN_COLOR_GREEN 10
#define WIN_COLOR_YELLOW 14
#define WIN_COLOR_BLUE 1
#define WIN_COLOR_GRAY 8
#define WIN_COLOR_RESET 7
#else
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_YELLOW "\x1b[33m"
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_GRAY "\x1b[37m"
#define ANSI_COLOR_RESET "\x1b[0m"
#endif

namespace mavsdk {

static std::mutex callback_mutex_{};
static log::Callback callback_{nullptr};

// Dedicated mutex for logging operations - moved from header to avoid inlining issues
static std::mutex log_mutex_{};

std::mutex& get_log_mutex()
{
    return log_mutex_;
}

log::Callback& log::get_callback()
{
    std::lock_guard<std::mutex> lock(callback_mutex_);
    return callback_;
}

void log::subscribe(const log::Callback& callback)
{
    std::lock_guard<std::mutex> lock(callback_mutex_);
    callback_ = callback;
}

void set_color(Color color)
{
#if defined(WINDOWS)
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (color) {
        case Color::Red:
            SetConsoleTextAttribute(handle, WIN_COLOR_RED);
            break;
        case Color::Green:
            SetConsoleTextAttribute(handle, WIN_COLOR_GREEN);
            break;
        case Color::Yellow:
            SetConsoleTextAttribute(handle, WIN_COLOR_YELLOW);
            break;
        case Color::Blue:
            SetConsoleTextAttribute(handle, WIN_COLOR_BLUE);
            break;
        case Color::Gray:
            SetConsoleTextAttribute(handle, WIN_COLOR_GRAY);
            break;
        case Color::Reset:
            SetConsoleTextAttribute(handle, WIN_COLOR_RESET);
            break;
    }
#elif defined(ANDROID) || defined(IOS)
    UNUSED(color);
#else
    switch (color) {
        case Color::Red:
            std::cout << ANSI_COLOR_RED;
            break;
        case Color::Green:
            std::cout << ANSI_COLOR_GREEN;
            break;
        case Color::Yellow:
            std::cout << ANSI_COLOR_YELLOW;
            break;
        case Color::Blue:
            std::cout << ANSI_COLOR_BLUE;
            break;
        case Color::Gray:
            std::cout << ANSI_COLOR_GRAY;
            break;
        case Color::Reset:
            std::cout << ANSI_COLOR_RESET;
            break;
    }
#endif
}

void emit_log(log::Level level, const std::string& message, const char* filename, int line)
{
    if (log::get_callback() &&
        log::get_callback()(level, message, filename, line)) {
        return;
    }

#if defined(ANDROID)
    switch (level) {
        case log::Level::Debug:
            __android_log_print(ANDROID_LOG_DEBUG, "Mavsdk", "%s", message.c_str());
            break;
        case log::Level::Info:
            __android_log_print(ANDROID_LOG_INFO, "Mavsdk", "%s", message.c_str());
            break;
        case log::Level::Warn:
            __android_log_print(ANDROID_LOG_WARN, "Mavsdk", "%s", message.c_str());
            break;
        case log::Level::Err:
            __android_log_print(ANDROID_LOG_ERROR, "Mavsdk", "%s", message.c_str());
            break;
    }
    (void)filename;
    (void)line;
#else
    switch (level) {
        case log::Level::Debug:
            set_color(Color::Green);
            break;
        case log::Level::Info:
            set_color(Color::Blue);
            break;
        case log::Level::Warn:
            set_color(Color::Yellow);
            break;
        case log::Level::Err:
            set_color(Color::Red);
            break;
    }

    time_t rawtime;
    time(&rawtime);
    struct tm* timeinfo = localtime(&rawtime);
    char time_buffer[10]{}; // We need 8 characters + \0
    strftime(time_buffer, sizeof(time_buffer), "%I:%M:%S", timeinfo);
    std::cout << "[" << time_buffer;

    switch (level) {
        case log::Level::Debug:
            std::cout << "|Debug] ";
            break;
        case log::Level::Info:
            std::cout << "|Info ] ";
            break;
        case log::Level::Warn:
            std::cout << "|Warn ] ";
            break;
        case log::Level::Err:
            std::cout << "|Error] ";
            break;
    }

    set_color(Color::Reset);

    std::cout << message;
    std::cout << " (" << filename << ":" << std::dec << line << ")";
    std::cout << std::endl;
#endif
}

} // namespace mavsdk
