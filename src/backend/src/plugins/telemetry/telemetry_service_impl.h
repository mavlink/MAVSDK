#include <future>

#include "plugins/telemetry/telemetry.h"
#include "telemetry/telemetry.grpc.pb.h"

namespace mavsdk {
namespace backend {

template<typename Telemetry = Telemetry>
class TelemetryServiceImpl final : public mavsdk::rpc::telemetry::TelemetryService::Service {
public:
    TelemetryServiceImpl(Telemetry& telemetry) :
        _telemetry(telemetry),
        _stop_promise(std::promise<void>()),
        _stop_future(_stop_promise.get_future())
    {}

    grpc::Status SubscribePosition(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribePositionRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::PositionResponse>* writer) override
    {
        std::mutex position_mutex{};

        _telemetry.position_async([&writer, &position_mutex](mavsdk::Telemetry::Position position) {
            auto rpc_position = new mavsdk::rpc::telemetry::Position();
            rpc_position->set_latitude_deg(position.latitude_deg);
            rpc_position->set_longitude_deg(position.longitude_deg);
            rpc_position->set_relative_altitude_m(position.relative_altitude_m);
            rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

            mavsdk::rpc::telemetry::PositionResponse rpc_position_response;
            rpc_position_response.set_allocated_position(rpc_position);

            std::lock_guard<std::mutex> lock(position_mutex);
            writer->Write(rpc_position_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeHealth(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeHealthRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::HealthResponse>* writer) override
    {
        std::mutex health_mutex{};

        _telemetry.health_async([&writer, &health_mutex](mavsdk::Telemetry::Health health) {
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

            std::lock_guard<std::mutex> lock(health_mutex);
            writer->Write(rpc_health_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeHome(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeHomeRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::HomeResponse>* writer) override
    {
        std::mutex home_mutex{};

        _telemetry.home_position_async(
            [&writer, &home_mutex](mavsdk::Telemetry::Position position) {
                auto rpc_position = new mavsdk::rpc::telemetry::Position();
                rpc_position->set_latitude_deg(position.latitude_deg);
                rpc_position->set_longitude_deg(position.longitude_deg);
                rpc_position->set_relative_altitude_m(position.relative_altitude_m);
                rpc_position->set_absolute_altitude_m(position.absolute_altitude_m);

                mavsdk::rpc::telemetry::HomeResponse rpc_home_response;
                rpc_home_response.set_allocated_home(rpc_position);

                std::lock_guard<std::mutex> lock(home_mutex);
                writer->Write(rpc_home_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeInAir(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeInAirRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::InAirResponse>* writer) override
    {
        std::mutex in_air_mutex{};

        _telemetry.in_air_async([&writer, &in_air_mutex](bool is_in_air) {
            mavsdk::rpc::telemetry::InAirResponse rpc_in_air_response;
            rpc_in_air_response.set_is_in_air(is_in_air);

            std::lock_guard<std::mutex> lock(in_air_mutex);
            writer->Write(rpc_in_air_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeLandedState(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeLandedStateRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::LandedStateResponse>* writer) override
    {
        std::mutex landed_state_mutex{};

        _telemetry.landed_state_async(
            [this, &writer, &landed_state_mutex](mavsdk::Telemetry::LandedState landed_state) {
                mavsdk::rpc::telemetry::LandedStateResponse rpc_landed_state_response;
                rpc_landed_state_response.set_landed_state(translateLandedState(landed_state));

                std::lock_guard<std::mutex> lock(landed_state_mutex);
                writer->Write(rpc_landed_state_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    rpc::telemetry::LandedState
    translateLandedState(const mavsdk::Telemetry::LandedState landed_state) const
    {
        switch (landed_state) {
            default:
            case mavsdk::Telemetry::LandedState::UNKNOWN:
                return rpc::telemetry::LandedState::LANDED_STATE_UNKNOWN;
            case mavsdk::Telemetry::LandedState::ON_GROUND:
                return rpc::telemetry::LandedState::LANDED_STATE_ON_GROUND;
            case mavsdk::Telemetry::LandedState::IN_AIR:
                return rpc::telemetry::LandedState::LANDED_STATE_IN_AIR;
            case mavsdk::Telemetry::LandedState::TAKING_OFF:
                return rpc::telemetry::LandedState::LANDED_STATE_TAKING_OFF;
            case mavsdk::Telemetry::LandedState::LANDING:
                return rpc::telemetry::LandedState::LANDED_STATE_LANDING;
        }
    }

    grpc::Status SubscribeStatusText(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeStatusTextRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::StatusTextResponse>* writer) override
    {
        std::mutex status_text_mutex{};

        _telemetry.status_text_async(
            [this, &writer, &status_text_mutex](mavsdk::Telemetry::StatusText status_text) {
                auto rpc_status_text = new mavsdk::rpc::telemetry::StatusText();
                rpc_status_text->set_text(status_text.text);
                rpc_status_text->set_type(translateStatusTextType(status_text.type));

                mavsdk::rpc::telemetry::StatusTextResponse rpc_status_text_response;
                rpc_status_text_response.set_allocated_status_text(rpc_status_text);

                std::lock_guard<std::mutex> lock(status_text_mutex);
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

    grpc::Status SubscribeArmed(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeArmedRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::ArmedResponse>* writer) override
    {
        std::mutex armed_mutex{};

        _telemetry.armed_async([&writer, &armed_mutex](bool is_armed) {
            mavsdk::rpc::telemetry::ArmedResponse rpc_armed_response;
            rpc_armed_response.set_is_armed(is_armed);

            std::lock_guard<std::mutex> lock(armed_mutex);
            writer->Write(rpc_armed_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeGpsInfo(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeGpsInfoRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::GpsInfoResponse>* writer) override
    {
        std::mutex gps_info_mutex{};

        _telemetry.gps_info_async(
            [this, &writer, &gps_info_mutex](mavsdk::Telemetry::GPSInfo gps_info) {
                auto rpc_gps_info = new mavsdk::rpc::telemetry::GpsInfo();
                rpc_gps_info->set_num_satellites(gps_info.num_satellites);
                rpc_gps_info->set_fix_type(translateGpsFixType(gps_info.fix_type));

                mavsdk::rpc::telemetry::GpsInfoResponse rpc_gps_info_response;
                rpc_gps_info_response.set_allocated_gps_info(rpc_gps_info);

                std::lock_guard<std::mutex> lock(gps_info_mutex);
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

    grpc::Status SubscribeBattery(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeBatteryRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::BatteryResponse>* writer) override
    {
        std::mutex battery_mutex{};

        _telemetry.battery_async([&writer, &battery_mutex](mavsdk::Telemetry::Battery battery) {
            auto rpc_battery = new mavsdk::rpc::telemetry::Battery();
            rpc_battery->set_voltage_v(battery.voltage_v);
            rpc_battery->set_remaining_percent(battery.remaining_percent);

            mavsdk::rpc::telemetry::BatteryResponse rpc_battery_response;
            rpc_battery_response.set_allocated_battery(rpc_battery);

            std::lock_guard<std::mutex> lock(battery_mutex);
            writer->Write(rpc_battery_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeFlightMode(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeFlightModeRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::FlightModeResponse>* writer) override
    {
        std::mutex flight_mode_mutex{};

        _telemetry.flight_mode_async(
            [this, &writer, &flight_mode_mutex](mavsdk::Telemetry::FlightMode flight_mode) {
                auto rpc_flight_mode = translateFlightMode(flight_mode);

                mavsdk::rpc::telemetry::FlightModeResponse rpc_flight_mode_response;
                rpc_flight_mode_response.set_flight_mode(rpc_flight_mode);

                std::lock_guard<std::mutex> lock(flight_mode_mutex);
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
            case mavsdk::Telemetry::FlightMode::MANUAL:
                return rpc::telemetry::FlightMode::MANUAL;
            case mavsdk::Telemetry::FlightMode::ALTCTL:
                return rpc::telemetry::FlightMode::ALTCTL;
            case mavsdk::Telemetry::FlightMode::POSCTL:
                return rpc::telemetry::FlightMode::POSCTL;
            case mavsdk::Telemetry::FlightMode::ACRO:
                return rpc::telemetry::FlightMode::ACRO;
            case mavsdk::Telemetry::FlightMode::STABILIZED:
                return rpc::telemetry::FlightMode::STABILIZED;
            case mavsdk::Telemetry::FlightMode::RATTITUDE:
                return rpc::telemetry::FlightMode::RATTITUDE;
        }
    }

    grpc::Status SubscribeAttitudeQuaternion(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeAttitudeQuaternionRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::AttitudeQuaternionResponse>* writer) override
    {
        std::mutex attitude_quaternion_mutex{};

        _telemetry.attitude_quaternion_async(
            [&writer, &attitude_quaternion_mutex](mavsdk::Telemetry::Quaternion quaternion) {
                auto rpc_quaternion = new mavsdk::rpc::telemetry::Quaternion();
                rpc_quaternion->set_w(quaternion.w);
                rpc_quaternion->set_x(quaternion.x);
                rpc_quaternion->set_y(quaternion.y);
                rpc_quaternion->set_z(quaternion.z);

                mavsdk::rpc::telemetry::AttitudeQuaternionResponse rpc_quaternion_response;
                rpc_quaternion_response.set_allocated_attitude_quaternion(rpc_quaternion);

                std::lock_guard<std::mutex> lock(attitude_quaternion_mutex);
                writer->Write(rpc_quaternion_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeAttitudeAngularVelocityBody(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeAttitudeAngularVelocityBodyRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::AttitudeAngularVelocityBodyResponse>* writer) override
    {
        std::mutex attitude_angular_velocity_body_mutex{};

        _telemetry.attitude_angular_velocity_body_async(
            [&writer, &attitude_angular_velocity_body_mutex](
                mavsdk::Telemetry::AngularVelocityBody angular_velocity_body) {
                auto rpc_angular_velocity_body = new mavsdk::rpc::telemetry::AngularVelocityBody();
                rpc_angular_velocity_body->set_roll_rad_s(angular_velocity_body.roll_rad_s);
                rpc_angular_velocity_body->set_pitch_rad_s(angular_velocity_body.pitch_rad_s);
                rpc_angular_velocity_body->set_yaw_rad_s(angular_velocity_body.yaw_rad_s);

                mavsdk::rpc::telemetry::AttitudeAngularVelocityBodyResponse
                    rpc_angular_velocity_body_response;
                rpc_angular_velocity_body_response.set_allocated_attitude_angular_velocity_body(
                    rpc_angular_velocity_body);

                std::lock_guard<std::mutex> lock(attitude_angular_velocity_body_mutex);
                writer->Write(rpc_angular_velocity_body_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeAttitudeEuler(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeAttitudeEulerRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::AttitudeEulerResponse>* writer) override
    {
        std::mutex attitude_euler_mutex{};

        _telemetry.attitude_euler_angle_async(
            [&writer, &attitude_euler_mutex](mavsdk::Telemetry::EulerAngle euler_angle) {
                auto rpc_euler_angle = new mavsdk::rpc::telemetry::EulerAngle();
                rpc_euler_angle->set_roll_deg(euler_angle.roll_deg);
                rpc_euler_angle->set_pitch_deg(euler_angle.pitch_deg);
                rpc_euler_angle->set_yaw_deg(euler_angle.yaw_deg);

                mavsdk::rpc::telemetry::AttitudeEulerResponse rpc_euler_response;
                rpc_euler_response.set_allocated_attitude_euler(rpc_euler_angle);

                std::lock_guard<std::mutex> lock(attitude_euler_mutex);
                writer->Write(rpc_euler_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCameraAttitudeQuaternion(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeCameraAttitudeQuaternionRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::CameraAttitudeQuaternionResponse>* writer) override
    {
        std::mutex camera_attitude_quaternion_mutex{};

        _telemetry.camera_attitude_quaternion_async(
            [&writer, &camera_attitude_quaternion_mutex](mavsdk::Telemetry::Quaternion quaternion) {
                auto rpc_quaternion = new mavsdk::rpc::telemetry::Quaternion();
                rpc_quaternion->set_w(quaternion.w);
                rpc_quaternion->set_x(quaternion.x);
                rpc_quaternion->set_y(quaternion.y);
                rpc_quaternion->set_z(quaternion.z);

                mavsdk::rpc::telemetry::CameraAttitudeQuaternionResponse rpc_quaternion_response;
                rpc_quaternion_response.set_allocated_attitude_quaternion(rpc_quaternion);

                std::lock_guard<std::mutex> lock(camera_attitude_quaternion_mutex);
                writer->Write(rpc_quaternion_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeCameraAttitudeEuler(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeCameraAttitudeEulerRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::CameraAttitudeEulerResponse>* writer) override
    {
        std::mutex camera_attitude_euler_mutex{};

        _telemetry.camera_attitude_euler_angle_async(
            [&writer, &camera_attitude_euler_mutex](mavsdk::Telemetry::EulerAngle euler_angle) {
                auto rpc_euler_angle = new mavsdk::rpc::telemetry::EulerAngle();
                rpc_euler_angle->set_roll_deg(euler_angle.roll_deg);
                rpc_euler_angle->set_pitch_deg(euler_angle.pitch_deg);
                rpc_euler_angle->set_yaw_deg(euler_angle.yaw_deg);

                mavsdk::rpc::telemetry::CameraAttitudeEulerResponse rpc_euler_response;
                rpc_euler_response.set_allocated_attitude_euler(rpc_euler_angle);

                std::lock_guard<std::mutex> lock(camera_attitude_euler_mutex);
                writer->Write(rpc_euler_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeGroundSpeedNed(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeGroundSpeedNedRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::GroundSpeedNedResponse>* writer) override
    {
        std::mutex ground_speed_mutex{};

        _telemetry.ground_speed_ned_async(
            [&writer, &ground_speed_mutex](mavsdk::Telemetry::GroundSpeedNED ground_speed) {
                auto rpc_ground_speed = new mavsdk::rpc::telemetry::SpeedNed();
                rpc_ground_speed->set_velocity_north_m_s(ground_speed.velocity_north_m_s);
                rpc_ground_speed->set_velocity_east_m_s(ground_speed.velocity_east_m_s);
                rpc_ground_speed->set_velocity_down_m_s(ground_speed.velocity_down_m_s);

                mavsdk::rpc::telemetry::GroundSpeedNedResponse rpc_ground_speed_response;
                rpc_ground_speed_response.set_allocated_ground_speed_ned(rpc_ground_speed);

                std::lock_guard<std::mutex> lock(ground_speed_mutex);
                writer->Write(rpc_ground_speed_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeRcStatus(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeRcStatusRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::RcStatusResponse>* writer) override
    {
        std::mutex rc_status_mutex{};

        _telemetry.rc_status_async(
            [&writer, &rc_status_mutex](mavsdk::Telemetry::RCStatus rc_status) {
                auto rpc_rc_status = new mavsdk::rpc::telemetry::RcStatus();
                rpc_rc_status->set_was_available_once(rc_status.available_once);
                rpc_rc_status->set_is_available(rc_status.available);
                rpc_rc_status->set_signal_strength_percent(rc_status.signal_strength_percent);

                mavsdk::rpc::telemetry::RcStatusResponse rpc_rc_status_response;
                rpc_rc_status_response.set_allocated_rc_status(rpc_rc_status);

                std::lock_guard<std::mutex> lock(rc_status_mutex);
                writer->Write(rpc_rc_status_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeActuatorControlTarget(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeActuatorControlTargetRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::ActuatorControlTargetResponse>* writer) override
    {
        std::mutex actuator_control_target_mutex{};

        _telemetry.actuator_control_target_async(
            [&writer, &actuator_control_target_mutex](
                mavsdk::Telemetry::ActuatorControlTarget actuator_control_target) {
                auto rpc_actuator_control_target =
                    new mavsdk::rpc::telemetry::ActuatorControlTarget();
                rpc_actuator_control_target->set_group(actuator_control_target.group);
                for (int i = 0; i < 8; i++) {
                    rpc_actuator_control_target->add_controls(actuator_control_target.controls[i]);
                }

                mavsdk::rpc::telemetry::ActuatorControlTargetResponse
                    rpc_actuator_control_target_response;
                rpc_actuator_control_target_response.set_allocated_actuator_control_target(
                    rpc_actuator_control_target);

                std::lock_guard<std::mutex> lock(actuator_control_target_mutex);
                writer->Write(rpc_actuator_control_target_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    grpc::Status SubscribeActuatorOutputStatus(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeActuatorOutputStatusRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::ActuatorOutputStatusResponse>* writer) override
    {
        std::mutex actuator_output_status_mutex{};

        _telemetry.actuator_output_status_async(
            [&writer, &actuator_output_status_mutex](
                mavsdk::Telemetry::ActuatorOutputStatus actuator_output_status) {
                auto rpc_actuator_output_status =
                    new mavsdk::rpc::telemetry::ActuatorOutputStatus();
                rpc_actuator_output_status->set_active(actuator_output_status.active);
                for (unsigned i = 0; i < actuator_output_status.active; i++) {
                    rpc_actuator_output_status->add_actuator(actuator_output_status.actuator[i]);
                }

                mavsdk::rpc::telemetry::ActuatorOutputStatusResponse
                    rpc_actuator_output_status_response;
                rpc_actuator_output_status_response.set_allocated_actuator_output_status(
                    rpc_actuator_output_status);

                std::lock_guard<std::mutex> lock(actuator_output_status_mutex);
                writer->Write(rpc_actuator_output_status_response);
            });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    mavsdk::rpc::telemetry::Odometry::MavFrame
    translateFrameId(const mavsdk::Telemetry::Odometry::MavFrame id) const
    {
        switch (id) {
            default:
            case mavsdk::Telemetry::Odometry::MavFrame::UNDEF:
                return mavsdk::rpc::telemetry::Odometry::MavFrame::Odometry_MavFrame_UNDEF;
            case mavsdk::Telemetry::Odometry::MavFrame::BODY_NED:
                return mavsdk::rpc::telemetry::Odometry::MavFrame::Odometry_MavFrame_BODY_NED;
            case mavsdk::Telemetry::Odometry::MavFrame::VISION_NED:
                return mavsdk::rpc::telemetry::Odometry::MavFrame::Odometry_MavFrame_VISION_NED;
            case mavsdk::Telemetry::Odometry::MavFrame::ESTIM_NED:
                return mavsdk::rpc::telemetry::Odometry::MavFrame::Odometry_MavFrame_ESTIM_NED;
        }
    }

    grpc::Status SubscribeOdometry(
        grpc::ServerContext* /* context */,
        const mavsdk::rpc::telemetry::SubscribeOdometryRequest* /* request */,
        grpc::ServerWriter<rpc::telemetry::OdometryResponse>* writer) override
    {
        std::mutex odometry_mutex{};

        _telemetry.odometry_async([this, &writer, &odometry_mutex](
                                      mavsdk::Telemetry::Odometry odometry) {
            auto rpc_odometry = new mavsdk::rpc::telemetry::Odometry();
            rpc_odometry->set_time_usec(odometry.time_usec);

            rpc_odometry->set_frame_id(translateFrameId(odometry.frame_id));
            rpc_odometry->set_child_frame_id(translateFrameId(odometry.child_frame_id));

            auto rpc_position_body = new mavsdk::rpc::telemetry::PositionBody();
            rpc_position_body->set_x_m(odometry.position_body.x_m);
            rpc_position_body->set_y_m(odometry.position_body.y_m);
            rpc_position_body->set_z_m(odometry.position_body.z_m);
            rpc_odometry->set_allocated_position_body(rpc_position_body);

            auto rpc_q = new mavsdk::rpc::telemetry::Quaternion();
            rpc_q->set_w(odometry.q.w);
            rpc_q->set_x(odometry.q.x);
            rpc_q->set_y(odometry.q.y);
            rpc_q->set_z(odometry.q.z);
            rpc_odometry->set_allocated_q(rpc_q);

            auto rpc_speed_body = new mavsdk::rpc::telemetry::SpeedBody();
            rpc_speed_body->set_velocity_x_m_s(odometry.velocity_body.x_m_s);
            rpc_speed_body->set_velocity_y_m_s(odometry.velocity_body.y_m_s);
            rpc_speed_body->set_velocity_z_m_s(odometry.velocity_body.z_m_s);
            rpc_odometry->set_allocated_speed_body(rpc_speed_body);

            auto rpc_angular_velocity_body = new mavsdk::rpc::telemetry::AngularVelocityBody();
            rpc_angular_velocity_body->set_roll_rad_s(odometry.angular_velocity_body.roll_rad_s);
            rpc_angular_velocity_body->set_pitch_rad_s(odometry.angular_velocity_body.pitch_rad_s);
            rpc_angular_velocity_body->set_yaw_rad_s(odometry.angular_velocity_body.yaw_rad_s);
            rpc_odometry->set_allocated_angular_velocity_body(rpc_angular_velocity_body);

            auto pose_covariance = new mavsdk::rpc::telemetry::Covariance();
            for (int i = 0; i < 21; i++) {
                pose_covariance->add_covariance_matrix(odometry.pose_covariance[i]);
            }
            rpc_odometry->set_allocated_pose_covariance(pose_covariance);

            auto velocity_covariance = new mavsdk::rpc::telemetry::Covariance();
            for (int i = 0; i < 21; i++) {
                velocity_covariance->add_covariance_matrix(odometry.velocity_covariance[i]);
            }
            rpc_odometry->set_allocated_velocity_covariance(velocity_covariance);

            mavsdk::rpc::telemetry::OdometryResponse rpc_odometry_response;
            rpc_odometry_response.set_allocated_odometry(rpc_odometry);

            std::lock_guard<std::mutex> lock(odometry_mutex);
            writer->Write(rpc_odometry_response);
        });

        _stop_future.wait();
        return grpc::Status::OK;
    }

    void stop() { _stop_promise.set_value(); }

private:
    Telemetry& _telemetry;
    std::promise<void> _stop_promise;
    std::future<void> _stop_future;
};

} // namespace backend
} // namespace mavsdk
