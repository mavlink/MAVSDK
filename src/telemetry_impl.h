#include "telemetry.h"
#include <atomic>

namespace dronelink {

class TelemetryImpl {
public:
    TelemetryImpl();
    ~TelemetryImpl();

    float get_absolute_altitude_m() const;
    void set_absolute_altitude_m(float absolute_altitude);

    float get_relative_altitude_m() const;
    void set_relative_altitude_m(float relative_altitude);

    Telemetry::Coordinates get_coordinates() const;
    void set_coordinates(Telemetry::Coordinates coordinates);

    bool in_air() const;
    void set_in_air(bool in_air);

    Telemetry::EulerAngle get_attitude_euler_angle() const;
    Telemetry::Quaternion get_attitude_quaternion() const;
    void set_attitude_quaternion(Telemetry::Quaternion quaternion);

private:
    // Use atomic to make this threadsafe
    std::atomic<float> _absolute_altitude;
    std::atomic<float> _relative_altitude;
    std::atomic<Telemetry::Coordinates> _coordinates;
    std::atomic_bool _in_air;
    std::atomic<Telemetry::Quaternion> _attitude_quaternion;
};

} // namespace dronelink
