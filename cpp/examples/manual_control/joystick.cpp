#include "joystick.h"

#include <cmath>
#include <csignal>
#include <iostream>
#include <memory>

Joystick::Joystick() {}

Joystick::~Joystick()
{
    stop_poll_loop();
    disconnect();
    deinit();
}

std::unique_ptr<Joystick> Joystick::create()
{
    std::unique_ptr<Joystick> ptr(new Joystick());

    if (ptr && ptr->init()) {
        ptr->connect();
        ptr->start_poll_loop();
        return ptr;
    } else {
        return nullptr;
    }
}

bool Joystick::init()
{
    if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
        std::cerr << "Could not initialize SDL joystick: " << SDL_GetError() << '\n';
        return false;
    }

    return true;
}

bool Joystick::connect()
{
    if (_joystick) {
        // Already connected.
        return true;
    }

    const auto num = SDL_NumJoysticks();
    if (num < 0) {
        std::cerr << "Could not get number of  joysticks: " << SDL_GetError();
        return false;
    }

    else if (num == 0) {
        std::cout << "No joystick found\n";
        return false;
    }

    else if (num == 1) {
        _joystick = SDL_JoystickOpen(0);
        set_initial_values();
        return true;
    }

    else {
        std::cout << "More than one joystick found, not sure what to use.\n";
        return false;
    }
}

void Joystick::disconnect()
{
    if (_joystick) {
        SDL_JoystickClose(_joystick);
        _joystick = nullptr;
    }
}

void Joystick::deinit()
{
    SDL_Quit();
}

void Joystick::start_poll_loop()
{
    _thread = std::thread([this]() {
        while (!_should_exit) {
            SDL_Event event;
            SDL_WaitEventTimeout(&event, 100);

            switch (event.type) {
                case SDL_JOYAXISMOTION:
                    set_axis(event.jaxis.axis, event.jaxis.value);
                    break;

                case SDL_JOYBUTTONDOWN:
                    set_button(event.jbutton.button, true);
                    break;

                case SDL_JOYBUTTONUP:
                    set_button(event.jbutton.button, false);
                    break;

                case SDL_JOYDEVICEREMOVED:
                    std::cout << "joystick removed\n";
                    disconnect();
                    break;

                case SDL_JOYDEVICEADDED:
                    // std::cout << "joystick added\n";
                    connect();
                    break;

                case SDL_QUIT:
                    disconnect();
                    deinit();
                    // Once we have deinitialized, we can raise the signal
                    // again so it gets caught outside of this joystick class.
                    raise(SIGINT);
                    return;
            }
            // print_values();
        }
    });
}

void Joystick::stop_poll_loop()
{
    _should_exit = true;
    _thread.join();
}

void Joystick::set_initial_values()
{
    _state.buttons.resize(SDL_JoystickNumButtons(_joystick));
    _state.axes.resize(SDL_JoystickNumAxes(_joystick));

    for (std::size_t i = 0; i < _state.buttons.size(); ++i) {
        set_button(i, SDL_JoystickGetButton(_joystick, i));
    }

    for (std::size_t i = 0; i < _state.axes.size(); ++i) {
        set_axis(i, SDL_JoystickGetAxis(_joystick, i));
    }
}

void Joystick::print_values()
{
    for (std::size_t i = 0; i < _state.buttons.size(); ++i) {
        std::cout << "button " << i << " -> " << _state.buttons[i] << '\n';
    }

    for (std::size_t i = 0; i < _state.axes.size(); ++i) {
        std::cout << "axis " << i << " -> " << _state.axes[i] << '\n';
    }

    std::cout << '\n';
}

void Joystick::set_button(int button, bool value)
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    _state.buttons[button] = value;
}

void Joystick::set_axis(int axis, float value)
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    // Scale and clamp between 1 and -1.
    _state.axes[axis] = std::max(std::min(value / 32767.f, 1.f), -1.f);
}

float Joystick::get_axis(unsigned axis)
{
    std::lock_guard<std::mutex> lock(_state_mutex);

    if (axis >= _state.axes.size()) {
        return NAN;
    }

    return _state.axes[axis];
}
