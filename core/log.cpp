#include "log.h"

#if defined(WINDOWS)
#include "Windows.h"
#define WIN_COLOR_RED 4
#define WIN_COLOR_GREEN 10
#define WIN_COLOR_YELLOW 14
#define WIN_COLOR_BLUE 1
#define WIN_COLOR_GRAY 8
#define WIN_COLOR_RESET 7
#else
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_GRAY    "\x1b[37m"
#define ANSI_COLOR_RESET   "\x1b[0m"
#endif

namespace dronecore {


void set_color(Color color) {
#if defined(WINDOWS)
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    switch (color) {
        case Color::RED:
            SetConsoleTextAttribute(handle, WIN_COLOR_RED);
            break;
        case Color::GREEN:
            SetConsoleTextAttribute(handle, WIN_COLOR_GREEN);
            break;
        case Color::YELLOW:
            SetConsoleTextAttribute(handle, WIN_COLOR_YELLOW);
            break;
        case Color::BLUE:
            SetConsoleTextAttribute(handle, WIN_COLOR_BLUE);
            break;
        case Color::GRAY:
            SetConsoleTextAttribute(handle, WIN_COLOR_GRAY);
            break;
        case Color::RESET:
            SetConsoleTextAttribute(handle, WIN_COLOR_RESET);
            break;
    }
#else
    switch (color) {
        case Color::RED:
            std::cout << ANSI_COLOR_RED;
            break;
        case Color::GREEN:
            std::cout << ANSI_COLOR_GREEN;
            break;
        case Color::YELLOW:
            std::cout << ANSI_COLOR_YELLOW;
            break;
        case Color::BLUE:
            std::cout << ANSI_COLOR_BLUE;
            break;
        case Color::GRAY:
            std::cout << ANSI_COLOR_GRAY;
            break;
        case Color::RESET:
            std::cout << ANSI_COLOR_RESET;
            break;
    }
#endif
}

} // namespace dronecore
