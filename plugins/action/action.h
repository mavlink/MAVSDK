#pragma once

#include <functional>

namespace dronelink {

class ActionImpl;

class Action
{
public:
    explicit Action(ActionImpl *impl);
    ~Action();

    enum class Result {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        COMMAND_DENIED_LANDED_STATE_UNKNOWN,
        COMMAND_DENIED_NOT_LANDED,
        TIMEOUT,
        UNKNOWN
    };

    static const char *result_str(Result);

    typedef std::function<void(Result)> result_callback_t;

    Result arm() const;
    Result disarm() const;
    Result kill() const;
    Result takeoff() const;
    Result land() const;
    Result return_to_launch() const;

    void arm_async(result_callback_t callback);
    void disarm_async(result_callback_t callback);
    void kill_async(result_callback_t callback);
    void takeoff_async(result_callback_t callback);
    void land_async(result_callback_t callback);
    void return_to_launch_async(result_callback_t callback);

    void set_takeoff_altitude(float relative_altitude_m);
    float get_takeoff_altitude_m() const;


    void set_max_speed(float speed_m_s);
    float get_max_speed_m_s() const;

    // Non-copyable
    Action(const Action &) = delete;
    const Action &operator=(const Action &) = delete;

private:
    // Underlying implementation, set at instantiation
    ActionImpl *_impl;
};

} // namespace dronelink
