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
        float north_m_s;
        float east_m_s;
        float down_m_s;
        float yaw_deg;
    };

    struct VelocityBodyYawspeed {
        float forward_m_s;
        float right_m_s;
        float down_m_s;
        float yawspeed_deg_s;
    };

    Offboard::Result start() const;
    Offboard::Result stop() const;

    void start_async(result_callback_t callback);
    void stop_async(result_callback_t callback);

    void set_velocity_ned(VelocityNEDYaw velocity_ned_yaw);
    void set_velocity_body(VelocityBodyYawspeed velocity_body_yawspeed);

    // Non-copyable
    Offboard(const Offboard &) = delete;
    const Offboard &operator=(const Offboard &) = delete;

private:
    // Underlying implementation, set at instantiation
    OffboardImpl *_impl;
};

} // namespace dronelink
