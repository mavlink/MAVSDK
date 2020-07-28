#pragma once

#include <atomic>
#include <memory>
#include <mutex>
#include <vector>
#include <thread>
#include <SDL2/SDL.h>

class Joystick {
public:
    static std::unique_ptr<Joystick> create();

    ~Joystick();
    float get_axis(unsigned axis);

private:
    std::mutex _state_mutex;
    struct State {
        std::vector<bool> buttons;
        std::vector<float> axes;
    } _state;

    Joystick();
    bool init();
    void deinit();
    bool connect();
    void disconnect();
    void start_poll_loop();
    void stop_poll_loop();
    void set_initial_values();
    void set_button(int button, bool value);
    void set_axis(int axis, float value);
    void print_values();

    SDL_Joystick* _joystick = nullptr;

    std::thread _thread{};
    std::atomic<bool> _should_exit{false};
};
