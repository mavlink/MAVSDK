#include <future>

#include "plugins/telemetry/telemetry.h"
#include "telemetry/telemetry.grpc.pb.h"

namespace mavsdk {
namespace backend {

template<typename Telemetry = Telemetry>
class TelemetryServiceImpl final : public mavsdk::rpc::telemetry::TelemetryService::Service {
public:
    TelemetryServiceImpl(Telemetry &telemetry) :
        _telemetry(telemetry),
        _stop_promise(std::promise<void>()),
        _stop_future(_stop_promise.get_future())
    {}

    grpc::Status
    SubscribePosition(grpc::ServerContext * /* context */,
                      const mavsdk::rpc::telemetry::SubscribePositionRequest * /* request */,
                      grpc::ServerWriter<rpc::telemetry::PositionResponse> *writer) override
    {
        _telemetry.position_async([&writer](mavsdk::Telemetry::Position position) {
            auto rpc_position = new mavsdk::rpc::telemetry::Position();
            rpc_position->set_latitude_deg(position.latitude_deg);
            rpc_position->set_longitude_deg(position.longitude_deg);
            rpc_position->set_relative_altitude_m(position.relative_altitude_m);
            rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

            mavsdk::rpc::telemetry::PositionResponse rpc_position_response;
            rpc_position_response.set_allocated_position(rpc_position);
            writer->Write(rpc_position_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeHealth(grpc::ServerContext * /* context */,
                    const mavsdk::rpc::telemetry::SubscribeHealthRequest * /* request */,
                    grpc::ServerWriter<rpc::telemetry::HealthResponse> *writer) override
    {
        _telemetry.health_async([&writer](mavsdk::Telemetry::Health health) {
            auto rpc_health = new mavsdk::rpc::telemetry::Health();
            rpc_health->set_is_gyrometer_calibration_ok(health.gyrometer_calibration_ok);
            rpc_health->set_is_accelerometer_calibration_ok(health.accelerometer_calibration_ok);
            rpc_health->set_is_magnetometer_calibration_ok(health.magnetometer_calibration_ok);
            rpc_health->set_is_level_calibration_ok(health.level_calibration_ok);
            rpc_health->set_is_local_position_ok(health.local_position_ok);
            rpc_health->set_is_global_position_ok(health.global_position_ok);
            rpc_health->set_is_home_position_ok(health.home_position_ok);

            mavsdk::rpc::telemetry::HealthResponse rpc_health_response;
            rpc_health_response.set_allocated_health(rpc_health);
            writer->Write(rpc_health_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeHome(grpc::ServerContext * /* context */,
                               const mavsdk::rpc::telemetry::SubscribeHomeRequest * /* request */,
                               grpc::ServerWriter<rpc::telemetry::HomeResponse> *writer) override
    {
        _telemetry.home_position_async([&writer](mavsdk::Telemetry::Position position) {
            auto rpc_position = new mavsdk::rpc::telemetry::Position();
            rpc_position->set_latitude_deg(position.latitude_deg);
            rpc_position->set_longitude_deg(position.longitude_deg);
            rpc_position->set_relative_altitude_m(position.relative_altitude_m);
            rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

            mavsdk::rpc::telemetry::HomeResponse rpc_home_response;
            rpc_home_response.set_allocated_home(rpc_position);
            writer->Write(rpc_home_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeInAir(grpc::ServerContext * /* context */,
                                const mavsdk::rpc::telemetry::SubscribeInAirRequest * /* request */,
                                grpc::ServerWriter<rpc::telemetry::InAirResponse> *writer) override
    {
        _telemetry.in_air_async([&writer](bool is_in_air) {
            mavsdk::rpc::telemetry::InAirResponse rpc_in_air_response;
            rpc_in_air_response.set_is_in_air(is_in_air);
            writer->Write(rpc_in_air_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeStatusText(grpc::ServerContext * /* context */,
                        const mavsdk::rpc::telemetry::SubscribeStatusTextRequest * /* request */,
                        grpc::ServerWriter<rpc::telemetry::StatusTextResponse> *writer) override
    {
        _telemetry.status_text_async([this, &writer](mavsdk::Telemetry::StatusText status_text) {
            auto rpc_status_text = new mavsdk::rpc::telemetry::StatusText();
            rpc_status_text->set_text(status_text.text);
            rpc_status_text->set_type(translateStatusTextType(status_text.type));

            mavsdk::rpc::telemetry::StatusTextResponse rpc_status_text_response;
            rpc_status_text_response.set_allocated_status_text(rpc_status_text);
            writer->Write(rpc_status_text_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    mavsdk::rpc::telemetry::StatusText::StatusType
    translateStatusTextType(const mavsdk::Telemetry::StatusText::StatusType type) const
    {
        switch (type) {
            default:
            case mavsdk::Telemetry::StatusText::StatusType::INFO:
                return mavsdk::rpc::telemetry::StatusText::StatusType::StatusText_StatusType_INFO;
            case mavsdk::Telemetry::StatusText::StatusType::WARNING:
                return mavsdk::rpc::telemetry::StatusText::StatusType::
                    StatusText_StatusType_WARNING;
            case mavsdk::Telemetry::StatusText::StatusType::CRITICAL:
                return mavsdk::rpc::telemetry::StatusText::StatusType::
                    StatusText_StatusType_CRITICAL;
        }
    }

    grpc::Status SubscribeArmed(grpc::ServerContext * /* context */,
                                const mavsdk::rpc::telemetry::SubscribeArmedRequest * /* request */,
                                grpc::ServerWriter<rpc::telemetry::ArmedResponse> *writer) override
    {
        _telemetry.armed_async([&writer](bool is_armed) {
            mavsdk::rpc::telemetry::ArmedResponse rpc_armed_response;
            rpc_armed_response.set_is_armed(is_armed);
            writer->Write(rpc_armed_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeGpsInfo(grpc::ServerContext * /* context */,
                     const mavsdk::rpc::telemetry::SubscribeGpsInfoRequest * /* request */,
                     grpc::ServerWriter<rpc::telemetry::GpsInfoResponse> *writer) override
    {
        _telemetry.gps_info_async([this, &writer](mavsdk::Telemetry::GPSInfo gps_info) {
            auto rpc_gps_info = new mavsdk::rpc::telemetry::GpsInfo();
            rpc_gps_info->set_num_satellites(gps_info.num_satellites);
            rpc_gps_info->set_fix_type(translateGpsFixType(gps_info.fix_type));

            mavsdk::rpc::telemetry::GpsInfoResponse rpc_gps_info_response;
            rpc_gps_info_response.set_allocated_gps_info(rpc_gps_info);
            writer->Write(rpc_gps_info_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    mavsdk::rpc::telemetry::FixType translateGpsFixType(const int fix_type) const
    {
        switch (fix_type) {
            default:
            case 0:
                return mavsdk::rpc::telemetry::FixType::NO_GPS;
            case 1:
                return mavsdk::rpc::telemetry::FixType::NO_FIX;
            case 2:
                return mavsdk::rpc::telemetry::FixType::FIX_2D;
            case 3:
                return mavsdk::rpc::telemetry::FixType::FIX_3D;
            case 4:
                return mavsdk::rpc::telemetry::FixType::FIX_DGPS;
            case 5:
                return mavsdk::rpc::telemetry::FixType::RTK_FLOAT;
            case 6:
                return mavsdk::rpc::telemetry::FixType::RTK_FIXED;
        }
    }

    grpc::Status
    SubscribeBattery(grpc::ServerContext * /* context */,
                     const mavsdk::rpc::telemetry::SubscribeBatteryRequest * /* request */,
                     grpc::ServerWriter<rpc::telemetry::BatteryResponse> *writer) override
    {
        _telemetry.battery_async([&writer](mavsdk::Telemetry::Battery battery) {
            auto rpc_battery = new mavsdk::rpc::telemetry::Battery();
            rpc_battery->set_voltage_v(battery.voltage_v);
            rpc_battery->set_remaining_percent(battery.remaining_percent);

            mavsdk::rpc::telemetry::BatteryResponse rpc_battery_response;
            rpc_battery_response.set_allocated_battery(rpc_battery);
            writer->Write(rpc_battery_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeFlightMode(grpc::ServerContext * /* context */,
                        const mavsdk::rpc::telemetry::SubscribeFlightModeRequest * /* request */,
                        grpc::ServerWriter<rpc::telemetry::FlightModeResponse> *writer) override
    {
        _telemetry.flight_mode_async([this, &writer](mavsdk::Telemetry::FlightMode flight_mode) {
            auto rpc_flight_mode = translateFlightMode(flight_mode);

            mavsdk::rpc::telemetry::FlightModeResponse rpc_flight_mode_response;
            rpc_flight_mode_response.set_flight_mode(rpc_flight_mode);
            writer->Write(rpc_flight_mode_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    rpc::telemetry::FlightMode
    translateFlightMode(const mavsdk::Telemetry::FlightMode flight_mode) const
    {
        switch (flight_mode) {
            default:
            case mavsdk::Telemetry::FlightMode::UNKNOWN:
                return rpc::telemetry::FlightMode::UNKNOWN;
            case mavsdk::Telemetry::FlightMode::READY:
                return rpc::telemetry::FlightMode::READY;
            case mavsdk::Telemetry::FlightMode::TAKEOFF:
                return rpc::telemetry::FlightMode::TAKEOFF;
            case mavsdk::Telemetry::FlightMode::HOLD:
                return rpc::telemetry::FlightMode::HOLD;
            case mavsdk::Telemetry::FlightMode::MISSION:
                return rpc::telemetry::FlightMode::MISSION;
            case mavsdk::Telemetry::FlightMode::RETURN_TO_LAUNCH:
                return rpc::telemetry::FlightMode::RETURN_TO_LAUNCH;
            case mavsdk::Telemetry::FlightMode::LAND:
                return rpc::telemetry::FlightMode::LAND;
            case mavsdk::Telemetry::FlightMode::OFFBOARD:
                return rpc::telemetry::FlightMode::OFFBOARD;
            case mavsdk::Telemetry::FlightMode::FOLLOW_ME:
                return rpc::telemetry::FlightMode::FOLLOW_ME;
        }
    }

    grpc::Status SubscribeAttitudeQuaternion(
        grpc::ServerContext * /* context */,
        const mavsdk::rpc::telemetry::SubscribeAttitudeQuaternionRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::AttitudeQuaternionResponse> *writer) override
    {
        _telemetry.attitude_quaternion_async([&writer](mavsdk::Telemetry::Quaternion quaternion) {
            auto rpc_quaternion = new mavsdk::rpc::telemetry::Quaternion();
            rpc_quaternion->set_w(quaternion.w);
            rpc_quaternion->set_x(quaternion.x);
            rpc_quaternion->set_y(quaternion.y);
            rpc_quaternion->set_z(quaternion.z);

            mavsdk::rpc::telemetry::AttitudeQuaternionResponse rpc_quaternion_response;
            rpc_quaternion_response.set_allocated_attitude_quaternion(rpc_quaternion);
            writer->Write(rpc_quaternion_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeAttitudeEuler(
        grpc::ServerContext * /* context */,
        const mavsdk::rpc::telemetry::SubscribeAttitudeEulerRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::AttitudeEulerResponse> *writer) override
    {
        _telemetry.attitude_euler_angle_async([&writer](mavsdk::Telemetry::EulerAngle euler_angle) {
            auto rpc_euler_angle = new mavsdk::rpc::telemetry::EulerAngle();
            rpc_euler_angle->set_roll_deg(euler_angle.roll_deg);
            rpc_euler_angle->set_pitch_deg(euler_angle.pitch_deg);
            rpc_euler_angle->set_yaw_deg(euler_angle.yaw_deg);

            mavsdk::rpc::telemetry::AttitudeEulerResponse rpc_euler_response;
            rpc_euler_response.set_allocated_attitude_euler(rpc_euler_angle);
            writer->Write(rpc_euler_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCameraAttitudeQuaternion(
        grpc::ServerContext * /* context */,
        const mavsdk::rpc::telemetry::SubscribeCameraAttitudeQuaternionRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::CameraAttitudeQuaternionResponse> *writer) override
    {
        _telemetry.camera_attitude_quaternion_async(
            [&writer](mavsdk::Telemetry::Quaternion quaternion) {
                auto rpc_quaternion = new mavsdk::rpc::telemetry::Quaternion();
                rpc_quaternion->set_w(quaternion.w);
                rpc_quaternion->set_x(quaternion.x);
                rpc_quaternion->set_y(quaternion.y);
                rpc_quaternion->set_z(quaternion.z);

                mavsdk::rpc::telemetry::CameraAttitudeQuaternionResponse rpc_quaternion_response;
                rpc_quaternion_response.set_allocated_attitude_quaternion(rpc_quaternion);
                writer->Write(rpc_quaternion_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCameraAttitudeEuler(
        grpc::ServerContext * /* context */,
        const mavsdk::rpc::telemetry::SubscribeCameraAttitudeEulerRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::CameraAttitudeEulerResponse> *writer) override
    {
        _telemetry.camera_attitude_euler_angle_async(
            [&writer](mavsdk::Telemetry::EulerAngle euler_angle) {
                auto rpc_euler_angle = new mavsdk::rpc::telemetry::EulerAngle();
                rpc_euler_angle->set_roll_deg(euler_angle.roll_deg);
                rpc_euler_angle->set_pitch_deg(euler_angle.pitch_deg);
                rpc_euler_angle->set_yaw_deg(euler_angle.yaw_deg);

                mavsdk::rpc::telemetry::CameraAttitudeEulerResponse rpc_euler_response;
                rpc_euler_response.set_allocated_attitude_euler(rpc_euler_angle);
                writer->Write(rpc_euler_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeGroundSpeedNed(
        grpc::ServerContext * /* context */,
        const mavsdk::rpc::telemetry::SubscribeGroundSpeedNedRequest * /* request */,
        grpc::ServerWriter<rpc::telemetry::GroundSpeedNedResponse> *writer) override
    {
        _telemetry.ground_speed_ned_async(
            [&writer](mavsdk::Telemetry::GroundSpeedNED ground_speed) {
                auto rpc_ground_speed = new mavsdk::rpc::telemetry::SpeedNed();
                rpc_ground_speed->set_velocity_north_m_s(ground_speed.velocity_north_m_s);
                rpc_ground_speed->set_velocity_east_m_s(ground_speed.velocity_east_m_s);
                rpc_ground_speed->set_velocity_down_m_s(ground_speed.velocity_down_m_s);

                mavsdk::rpc::telemetry::GroundSpeedNedResponse rpc_ground_speed_response;
                rpc_ground_speed_response.set_allocated_ground_speed_ned(rpc_ground_speed);
                writer->Write(rpc_ground_speed_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status
    SubscribeRcStatus(grpc::ServerContext * /* context */,
                      const mavsdk::rpc::telemetry::SubscribeRcStatusRequest * /* request */,
                      grpc::ServerWriter<rpc::telemetry::RcStatusResponse> *writer) override
    {
        _telemetry.rc_status_async([&writer](mavsdk::Telemetry::RCStatus rc_status) {
            auto rpc_rc_status = new mavsdk::rpc::telemetry::RcStatus();
            rpc_rc_status->set_was_available_once(rc_status.available_once);
            rpc_rc_status->set_is_available(rc_status.available);
            rpc_rc_status->set_signal_strength_percent(rc_status.signal_strength_percent);

            mavsdk::rpc::telemetry::RcStatusResponse rpc_rc_status_response;
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
} // namespace mavsdk
