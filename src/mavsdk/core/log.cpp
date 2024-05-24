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

std::ostream& operator<<(std::ostream& os, std::byte b)
{
    return os << std::bitset<8>(std::to_integer<int>(b));
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

} // namespace mavsdk
