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

    void arm_async(CallbackData);
    void disarm_async(CallbackData);
    void kill_async(CallbackData);
    void takeoff_async(CallbackData);
    void land_async(CallbackData);
    void return_to_land_async(CallbackData);

    // Non-copyable
    Action(const Action &) = delete;
    const Action &operator=(const Action &) = delete;

private:
    // Underlying implementation, set at instantiation
    ActionImpl *_impl;
};

} // namespace dronelink
