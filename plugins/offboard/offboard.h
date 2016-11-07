#pragma once

#include <functional>

namespace dronelink {

class OffboardImpl;

class Offboard
{
public:
    explicit Offboard(OffboardImpl *impl);
    ~Offboard();

    enum class Result {
        SUCCESS = 0,
        NO_DEVICE,
        CONNECTION_ERROR,
        BUSY,
        COMMAND_DENIED,
        TIMEOUT,
        UNKNOWN
    };

    static const char *result_str(Result);

    typedef std::function<void(Result)> result_callback_t;

    struct VelocityNEDYaw {
        float velocity_north_m_s;
        float velocity_east_m_s;
        float velocity_down_m_s;
        float yaw_deg;
    };

    Offboard::Result start() const;
    Offboard::Result stop() const;

    void start_async(result_callback_t callback);
    void stop_async(result_callback_t callback);

    void set_velocity(Offboard::VelocityNEDYaw velocity_ned_yaw);

    // Non-copyable
    Offboard(const Offboard &) = delete;
    const Offboard &operator=(const Offboard &) = delete;

private:
    // Underlying implementation, set at instantiation
    OffboardImpl *_impl;
};

} // namespace dronelink
