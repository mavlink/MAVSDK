#include "telemetry.h"
#include <atomic>
#include <mutex>

namespace dronelink {

class TelemetryImpl {
public:
    TelemetryImpl();
    ~TelemetryImpl();

    Telemetry::Position get_position() const;
    void set_position(Telemetry::Position position);

    Telemetry::Position get_home_position() const;
    void set_home_position(Telemetry::Position home_position);

    bool in_air() const;
    void set_in_air(bool in_air);

    Telemetry::EulerAngle get_attitude_euler_angle() const;
    Telemetry::Quaternion get_attitude_quaternion() const;
    void set_attitude_quaternion(Telemetry::Quaternion quaternion);

    Telemetry::GroundSpeedNED get_ground_speed_ned() const;
    void set_ground_speed_ned(Telemetry::GroundSpeedNED ground_speed_ned);

    Telemetry::GPSInfo get_gps_info() const;
    void set_gps_info(Telemetry::GPSInfo gps_info);

    Telemetry::Battery get_battery() const;
    void set_battery(Telemetry::Battery battery);

private:
    // Make all fields thread-safe using mutexs
    // The mutexs are mutable so that the lock can get aqcuired in
    // methods marked const.
    mutable std::mutex _position_mutex;
    Telemetry::Position _position;

    mutable std::mutex _home_position_mutex;
    Telemetry::Position _home_position;

    // If possible, just use atomic instead of a mutex.
    std::atomic_bool _in_air;

    mutable std::mutex _attitude_quaternion_mutex;
    Telemetry::Quaternion _attitude_quaternion;

    mutable std::mutex _ground_speed_ned_mutex;
    Telemetry::GroundSpeedNED _ground_speed_ned;

    mutable std::mutex _gps_info_mutex;
    Telemetry::GPSInfo _gps_info;

    mutable std::mutex _battery_mutex;
    Telemetry::Battery _battery;
};

} // namespace dronelink
