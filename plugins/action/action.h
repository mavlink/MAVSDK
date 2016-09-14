#pragma once

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
        TIMEOUT,
        UNKNOWN
    };

    typedef void (*result_callback_t)(Result result, void *user);

    struct CallbackData {
        result_callback_t callback;
        void *user;
    };

    Result arm() const;
    Result disarm() const;
    Result kill() const;
    Result takeoff() const;
    Result land() const;
    Result return_to_land() const;

    void arm_async(CallbackData callback_data);
    void disarm_async(CallbackData callback_data);
    void kill_async(CallbackData callback_data);
    void takeoff_async(CallbackData callback_data);
    void land_async(CallbackData callback_data);
    void return_to_land_async(CallbackData callback_data);

    // Non-copyable
    Action(const Action &) = delete;
    const Action &operator=(const Action &) = delete;

private:
    // Underlying implementation, set at instantiation
    ActionImpl *_impl;
};

} // namespace dronelink
