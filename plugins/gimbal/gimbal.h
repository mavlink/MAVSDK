#pragma once

#include <functional>

namespace dronecore {

class GimbalImpl;

class Gimbal
{
public:
    explicit Gimbal(GimbalImpl *impl);
    ~Gimbal();

    enum class Result {
        SUCCESS = 0,
        ERROR,
        TIMEOUT,
        UNKNOWN
    };

    static const char *result_str(Result result);

    typedef std::function<void(Result)> result_callback_t;

    Result set_pitch_and_yaw(float pitch_deg, float yaw_deg);

    void set_pitch_and_yaw_async(float pitch_deg, float yaw_deg, result_callback_t callback);

    // Non-copyable
    Gimbal(const Gimbal &) = delete;
    const Gimbal &operator=(const Gimbal &) = delete;

private:
    // Underlying implementation, set at instantiation
    GimbalImpl *_impl;
};

} // namespace dronecore
