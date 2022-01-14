#include "grpc_server.h"

#include <grpc++/server_builder.h>
#include <grpc++/security/server_credentials.h>

#include "log.h"

namespace mavsdk {
namespace mavsdk_server {

void GrpcServer::set_port(const int port)
{
    _port = port;
}

int GrpcServer::run()
{
    grpc::ServerBuilder builder;
    setup_port(builder);

    builder.RegisterService(&_core);
    builder.RegisterService(&_action_service);
    builder.RegisterService(&_action_server_service);
    builder.RegisterService(&_calibration_service);
    builder.RegisterService(&_camera_service);
    builder.RegisterService(&_camera_server_service);
    builder.RegisterService(&_failure_service);
    builder.RegisterService(&_follow_me_service);
    builder.RegisterService(&_ftp_service);
    builder.RegisterService(&_geofence_service);
    builder.RegisterService(&_gimbal_service);
    builder.RegisterService(&_info_service);
    builder.RegisterService(&_log_files_service);
    builder.RegisterService(&_manual_control_service);
    builder.RegisterService(&_mission_service);
    builder.RegisterService(&_mission_raw_service);
    builder.RegisterService(&_mission_raw_server_service);
    builder.RegisterService(&_mocap_service);
    builder.RegisterService(&_offboard_service);
    builder.RegisterService(&_param_service);
    builder.RegisterService(&_param_server_service);
    builder.RegisterService(&_server_utility_service);
    builder.RegisterService(&_shell_service);
    builder.RegisterService(&_telemetry_service);
    builder.RegisterService(&_telemetry_server_service);
    builder.RegisterService(&_tracking_server_service);
    builder.RegisterService(&_transponder_service);
    builder.RegisterService(&_tune_service);

    _server = builder.BuildAndStart();

    if (_bound_port != 0) {
        LogInfo() << "Server started";
        LogInfo() << "Server set to listen on 0.0.0.0:" << _bound_port;
    } else {
        LogErr() << "Failed to bind server to port " << _port;
    }

    return _bound_port;
}

void GrpcServer::wait()
{
    if (_server != nullptr) {
        _server->Wait();
    } else {
        LogWarn() << "Calling 'wait()' on a non-existing server. Did you call 'run()' before?";
    }
}

void GrpcServer::stop()
{
    if (_server != nullptr) {
        _core.stop();
        _action_service.stop();
        _action_server_service.stop();
        _calibration_service.stop();
        _camera_service.stop();
        _camera_server_service.stop();
        _failure_service.stop();
        _follow_me_service.stop();
        _ftp_service.stop();
        _geofence_service.stop();
        _gimbal_service.stop();
        _info_service.stop();
        _log_files_service.stop();
        _manual_control_service.stop();
        _mission_service.stop();
        _mission_raw_service.stop();
        _mission_raw_server_service.stop();
        _mocap_service.stop();
        _offboard_service.stop();
        _param_service.stop();
        _param_server_service.stop();
        _server_utility_service.stop();
        _shell_service.stop();
        _telemetry_service.stop();
        _telemetry_server_service.stop();
        _tracking_server_service.stop();
        _tune_service.stop();
        _transponder_service.stop();
        _server->Shutdown();
    } else {
        LogWarn() << "Calling 'stop()' on a non-existing server. Did you call 'run()' before?";
    }
}

void GrpcServer::setup_port(grpc::ServerBuilder& builder)
{
    const std::string server_address("0.0.0.0:" + std::to_string(_port));
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(), &_bound_port);
}

} // namespace mavsdk_server
} // namespace mavsdk
