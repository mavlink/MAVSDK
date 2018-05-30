#include <future>

#include "telemetry/telemetry.h"
#include "telemetry/telemetry.grpc.pb.h"

namespace dronecore {
namespace backend {

template<typename Telemetry = Telemetry>
class TelemetryServiceImpl final : public dronecore::rpc::telemetry::TelemetryService::Service {
public:
    TelemetryServiceImpl(Telemetry &telemetry) :
        _telemetry(telemetry),
        _stop_promise(std::promise<void>()),
        _stop_future(_stop_promise.get_future())
    {}

    grpc::Status
    SubscribePosition(grpc::ServerContext * /* context */,
                      const dronecore::rpc::telemetry::SubscribePositionRequest * /* request */,
                      grpc::ServerWriter<rpc::telemetry::PositionResponse> *writer) override
    {
        _telemetry.position_async([&writer](dronecore::Telemetry::Position position) {
            auto rpc_position = new dronecore::rpc::telemetry::Position();
            rpc_position->set_latitude_deg(position.latitude_deg);
            rpc_position->set_longitude_deg(position.longitude_deg);
            rpc_position->set_relative_altitude_m(position.relative_altitude_m);
            rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

            dronecore::rpc::telemetry::PositionResponse rpc_position_response;
            rpc_position_response.set_allocated_position(rpc_position);
            writer->Write(rpc_position_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeHealth(grpc::ServerContext * /* context */,
                    const dronecore::rpc::telemetry::SubscribeHealthRequest * /* request */,
                    grpc::ServerWriter<rpc::telemetry::HealthResponse> *writer) override
    {
        _telemetry.health_async([&writer](dronecore::Telemetry::Health health) {
            auto rpc_health = new dronecore::rpc::telemetry::Health();
            rpc_health->set_is_gyrometer_calibration_ok(health.gyrometer_calibration_ok);
            rpc_health->set_is_accelerometer_calibration_ok(health.accelerometer_calibration_ok);
            rpc_health->set_is_magnetometer_calibration_ok(health.magnetometer_calibration_ok);
            rpc_health->set_is_level_calibration_ok(health.level_calibration_ok);
            rpc_health->set_is_local_position_ok(health.local_position_ok);
            rpc_health->set_is_global_position_ok(health.global_position_ok);
            rpc_health->set_is_home_position_ok(health.home_position_ok);

            dronecore::rpc::telemetry::HealthResponse rpc_health_response;
            rpc_health_response.set_allocated_health(rpc_health);
            writer->Write(rpc_health_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeHome(grpc::ServerContext * /* context */,
                  const dronecore::rpc::telemetry::SubscribeHomeRequest * /* request */,
                  grpc::ServerWriter<rpc::telemetry::HomeResponse> *writer) override
    {
        _telemetry.home_position_async([&writer](dronecore::Telemetry::Position position) {
            auto rpc_position = new dronecore::rpc::telemetry::Position();
            rpc_position->set_latitude_deg(position.latitude_deg);
            rpc_position->set_longitude_deg(position.longitude_deg);
            rpc_position->set_relative_altitude_m(position.relative_altitude_m);
            rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

            dronecore::rpc::telemetry::HomeResponse rpc_home_response;
            rpc_home_response.set_allocated_home(rpc_position);
            writer->Write(rpc_home_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeInAir(grpc::ServerContext * /* context */,
                   const dronecore::rpc::telemetry::SubscribeInAirRequest * /* request */,
                   grpc::ServerWriter<rpc::telemetry::InAirResponse> *writer) override
    {
        _telemetry.in_air_async([&writer](bool is_in_air) {
            dronecore::rpc::telemetry::InAirResponse rpc_in_air_response;
            rpc_in_air_response.set_is_in_air(is_in_air);
            writer->Write(rpc_in_air_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeArmed(grpc::ServerContext * /* context */,
                   const dronecore::rpc::telemetry::SubscribeArmedRequest * /* request */,
                   grpc::ServerWriter<rpc::telemetry::ArmedResponse> *writer) override
    {
        _telemetry.armed_async([&writer](bool is_armed) {
            dronecore::rpc::telemetry::ArmedResponse rpc_armed_response;
            rpc_armed_response.set_is_armed(is_armed);
            writer->Write(rpc_armed_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeGPSInfo(grpc::ServerContext * /* context */,
                     const dronecore::rpc::telemetry::SubscribeGPSInfoRequest * /* request */,
                     grpc::ServerWriter<rpc::telemetry::GPSInfoResponse> *writer) override
    {
        _telemetry.gps_info_async([this, &writer](dronecore::Telemetry::GPSInfo gps_info) {
            auto rpc_gps_info = new dronecore::rpc::telemetry::GPSInfo();
            rpc_gps_info->set_num_satellites(gps_info.num_satellites);
            rpc_gps_info->set_fix_type(translateGPSFixType(gps_info.fix_type));

            dronecore::rpc::telemetry::GPSInfoResponse rpc_gps_info_response;
            rpc_gps_info_response.set_allocated_gps_info(rpc_gps_info);
            writer->Write(rpc_gps_info_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    dronecore::rpc::telemetry::FixType translateGPSFixType(const int fix_type) const
    {
        switch (fix_type) {
            default:
            case 0:
                return dronecore::rpc::telemetry::FixType::NO_GPS;
            case 1:
                return dronecore::rpc::telemetry::FixType::NO_FIX;
            case 2:
                return dronecore::rpc::telemetry::FixType::FIX_2D;
            case 3:
                return dronecore::rpc::telemetry::FixType::FIX_3D;
            case 4:
                return dronecore::rpc::telemetry::FixType::FIX_DGPS;
            case 5:
                return dronecore::rpc::telemetry::FixType::RTK_FLOAT;
            case 6:
                return dronecore::rpc::telemetry::FixType::RTK_FIXED;
        }
    }

    grpc::Status
    SubscribeBattery(grpc::ServerContext * /* context */,
                     const dronecore::rpc::telemetry::SubscribeBatteryRequest * /* request */,
                     grpc::ServerWriter<rpc::telemetry::BatteryResponse> *writer) override
    {
        _telemetry.battery_async([&writer](dronecore::Telemetry::Battery battery) {
            auto rpc_battery = new dronecore::rpc::telemetry::Battery();
            rpc_battery->set_voltage_v(battery.voltage_v);
            rpc_battery->set_remaining_percent(battery.remaining_percent);

            dronecore::rpc::telemetry::BatteryResponse rpc_battery_response;
            rpc_battery_response.set_allocated_battery(rpc_battery);
            writer->Write(rpc_battery_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeFlightMode(grpc::ServerContext * /* context */,
                        const dronecore::rpc::telemetry::SubscribeFlightModeRequest * /* request */,
                        grpc::ServerWriter<rpc::telemetry::FlightModeResponse> *writer) override
    {
        _telemetry.flight_mode_async([this, &writer](dronecore::Telemetry::FlightMode flight_mode) {
            auto rpc_flight_mode = translateFlightMode(flight_mode);

            dronecore::rpc::telemetry::FlightModeResponse rpc_flight_mode_response;
            rpc_flight_mode_response.set_flight_mode(rpc_flight_mode);
            writer->Write(rpc_flight_mode_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    rpc::telemetry::FlightMode
    translateFlightMode(const dronecore::Telemetry::FlightMode flight_mode) const
    {
        switch (flight_mode) {
            default:
            case dronecore::Telemetry::FlightMode::UNKNOWN:
                return rpc::telemetry::FlightMode::UNKNOWN;
            case dronecore::Telemetry::FlightMode::READY:
                return rpc::telemetry::FlightMode::READY;
            case dronecore::Telemetry::FlightMode::TAKEOFF:
                return rpc::telemetry::FlightMode::TAKEOFF;
            case dronecore::Telemetry::FlightMode::HOLD:
                return rpc::telemetry::FlightMode::HOLD;
            case dronecore::Telemetry::FlightMode::MISSION:
                return rpc::telemetry::FlightMode::MISSION;
            case dronecore::Telemetry::FlightMode::RETURN_TO_LAUNCH:
                return rpc::telemetry::FlightMode::RETURN_TO_LAUNCH;
            case dronecore::Telemetry::FlightMode::LAND:
                return rpc::telemetry::FlightMode::LAND;
            case dronecore::Telemetry::FlightMode::OFFBOARD:
                return rpc::telemetry::FlightMode::OFFBOARD;
            case dronecore::Telemetry::FlightMode::FOLLOW_ME:
                return rpc::telemetry::FlightMode::FOLLOW_ME;
        }
    }

    grpc::Status SubscribeAttitudeQuaternion(
        grpc::ServerContext * /* context */,
        const dronecore::rpc::telemetry::SubscribeAttitudeQuaternionRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::AttitudeQuaternionResponse> *writer) override
    {
        _telemetry.attitude_quaternion_async(
            [&writer](dronecore::Telemetry::Quaternion quaternion) {
                auto rpc_quaternion = new dronecore::rpc::telemetry::Quaternion();
                rpc_quaternion->set_w(quaternion.w);
                rpc_quaternion->set_x(quaternion.x);
                rpc_quaternion->set_y(quaternion.y);
                rpc_quaternion->set_z(quaternion.z);

                dronecore::rpc::telemetry::AttitudeQuaternionResponse rpc_quaternion_response;
                rpc_quaternion_response.set_allocated_attitude_quaternion(rpc_quaternion);
                writer->Write(rpc_quaternion_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeAttitudeEuler(
        grpc::ServerContext * /* context */,
        const dronecore::rpc::telemetry::SubscribeAttitudeEulerRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::AttitudeEulerResponse> *writer) override
    {
        _telemetry.attitude_euler_angle_async(
            [&writer](dronecore::Telemetry::EulerAngle euler_angle) {
                auto rpc_euler_angle = new dronecore::rpc::telemetry::EulerAngle();
                rpc_euler_angle->set_roll_deg(euler_angle.roll_deg);
                rpc_euler_angle->set_pitch_deg(euler_angle.pitch_deg);
                rpc_euler_angle->set_yaw_deg(euler_angle.yaw_deg);

                dronecore::rpc::telemetry::AttitudeEulerResponse rpc_euler_response;
                rpc_euler_response.set_allocated_attitude_euler(rpc_euler_angle);
                writer->Write(rpc_euler_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCameraAttitudeQuaternion(
        grpc::ServerContext * /* context */,
        const dronecore::rpc::telemetry::SubscribeCameraAttitudeQuaternionRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::CameraAttitudeQuaternionResponse> *writer) override
    {
        _telemetry.camera_attitude_quaternion_async(
            [&writer](dronecore::Telemetry::Quaternion quaternion) {
                auto rpc_quaternion = new dronecore::rpc::telemetry::Quaternion();
                rpc_quaternion->set_w(quaternion.w);
                rpc_quaternion->set_x(quaternion.x);
                rpc_quaternion->set_y(quaternion.y);
                rpc_quaternion->set_z(quaternion.z);

                dronecore::rpc::telemetry::CameraAttitudeQuaternionResponse rpc_quaternion_response;
                rpc_quaternion_response.set_allocated_attitude_quaternion(rpc_quaternion);
                writer->Write(rpc_quaternion_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCameraAttitudeEuler(
        grpc::ServerContext * /* context */,
        const dronecore::rpc::telemetry::SubscribeCameraAttitudeEulerRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::CameraAttitudeEulerResponse> *writer) override
    {
        _telemetry.camera_attitude_euler_angle_async(
            [&writer](dronecore::Telemetry::EulerAngle euler_angle) {
                auto rpc_euler_angle = new dronecore::rpc::telemetry::EulerAngle();
                rpc_euler_angle->set_roll_deg(euler_angle.roll_deg);
                rpc_euler_angle->set_pitch_deg(euler_angle.pitch_deg);
                rpc_euler_angle->set_yaw_deg(euler_angle.yaw_deg);

                dronecore::rpc::telemetry::CameraAttitudeEulerResponse rpc_euler_response;
                rpc_euler_response.set_allocated_attitude_euler(rpc_euler_angle);
                writer->Write(rpc_euler_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeGroundSpeedNED(
        grpc::ServerContext * /* context */,
        const dronecore::rpc::telemetry::SubscribeGroundSpeedNEDRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::GroundSpeedNEDResponse> *writer) override
    {
        _telemetry.ground_speed_ned_async(
            [&writer](dronecore::Telemetry::GroundSpeedNED ground_speed) {
                auto rpc_ground_speed = new dronecore::rpc::telemetry::SpeedNED();
                rpc_ground_speed->set_velocity_north_m_s(ground_speed.velocity_north_m_s);
                rpc_ground_speed->set_velocity_east_m_s(ground_speed.velocity_east_m_s);
                rpc_ground_speed->set_velocity_down_m_s(ground_speed.velocity_down_m_s);

                dronecore::rpc::telemetry::GroundSpeedNEDResponse rpc_ground_speed_response;
                rpc_ground_speed_response.set_allocated_ground_speed_ned(rpc_ground_speed);
                writer->Write(rpc_ground_speed_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeRCStatus(grpc::ServerContext * /* context */,
                      const dronecore::rpc::telemetry::SubscribeRCStatusRequest * /* request */,
                      grpc::ServerWriter<rpc::telemetry::RCStatusResponse> *writer) override
    {
        _telemetry.rc_status_async([&writer](dronecore::Telemetry::RCStatus rc_status) {
            auto rpc_rc_status = new dronecore::rpc::telemetry::RCStatus();
            rpc_rc_status->set_was_available_once(rc_status.available_once);
            rpc_rc_status->set_is_available(rc_status.available);
            rpc_rc_status->set_signal_strength_percent(rc_status.signal_strength_percent);

            dronecore::rpc::telemetry::RCStatusResponse rpc_rc_status_response;
            rpc_rc_status_response.set_allocated_rc_status(rpc_rc_status);
            writer->Write(rpc_rc_status_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    void stop() { _stop_promise.set_value(); }

private:
    Telemetry &_telemetry;
    std::promise<void> _stop_promise;
    std::future<void> _stop_future;
};

} // namespace backend
} // namespace dronecore
