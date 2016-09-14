#pragma once

#include "error_handling.h"

namespace dronelink {

class ActionImpl;

class Action
{
public:
    explicit Action(ActionImpl *impl);
    ~Action();

    Result arm() const;
    Result disarm() const;
    Result kill() const;
    Result takeoff() const;
    Result land() const;
    Result return_to_land() const;

    void arm_async(result_callback_t callback);
    void disarm_async(result_callback_t callback);
    void kill_async(result_callback_t callback);
    void takeoff_async(result_callback_t callback);
    void land_async(result_callback_t callback);
    void return_to_land_async(result_callback_t callback);

    // Non-copyable
    Action(const Action &) = delete;
    const Action &operator=(const Action &) = delete;

private:
    // Underlying implementation, set at instantiation
    ActionImpl *_impl;
};

} // namespace dronelink
