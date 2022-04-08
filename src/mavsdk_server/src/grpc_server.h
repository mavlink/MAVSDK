#pragma once

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <memory>

#include "mavsdk.h"
#include "core/core_service_impl.h"
#include "plugins/action/action.h"
#include "action/action_service_impl.h"
#include "plugins/action_server/action_server.h"
#include "action_server/action_server_service_impl.h"
#include "plugins/calibration/calibration.h"
#include "calibration/calibration_service_impl.h"
#include "plugins/camera/camera.h"
#include "camera/camera_service_impl.h"
#include "plugins/camera_server/camera_server.h"
#include "camera_server/camera_server_service_impl.h"
#include "plugins/failure/failure.h"
#include "failure/failure_service_impl.h"
#include "plugins/follow_me/follow_me.h"
#include "follow_me/follow_me_service_impl.h"
#include "plugins/ftp/ftp.h"
#include "ftp/ftp_service_impl.h"
#include "plugins/geofence/geofence.h"
#include "geofence/geofence_service_impl.h"
#include "plugins/gimbal/gimbal.h"
#include "gimbal/gimbal_service_impl.h"
#include "plugins/info/info.h"
#include "info/info_service_impl.h"
#include "plugins/log_files/log_files.h"
#include "log_files/log_files_service_impl.h"
#include "plugins/manual_control/manual_control.h"
#include "manual_control/manual_control_service_impl.h"
#include "plugins/mission/mission.h"
#include "mission/mission_service_impl.h"
#include "plugins/mission_raw/mission_raw.h"
#include "mission_raw/mission_raw_service_impl.h"
#include "plugins/mission_raw_server/mission_raw_server.h"
#include "mission_raw_server/mission_raw_server_service_impl.h"
#include "plugins/mocap/mocap.h"
#include "mocap/mocap_service_impl.h"
#include "plugins/offboard/offboard.h"
#include "offboard/offboard_service_impl.h"
#include "plugins/param/param.h"
#include "param/param_service_impl.h"
#include "plugins/param_server/param_server.h"
#include "param_server/param_server_service_impl.h"
#include "plugins/server_utility/server_utility.h"
#include "server_utility/server_utility_service_impl.h"
#include "plugins/shell/shell.h"
#include "shell/shell_service_impl.h"
#include "plugins/telemetry/telemetry.h"
#include "telemetry/telemetry_service_impl.h"
#include "plugins/telemetry_server/telemetry_server.h"
#include "telemetry_server/telemetry_server_service_impl.h"
#include "plugins/tracking_server/tracking_server.h"
#include "tracking_server/tracking_server_service_impl.h"
#include "plugins/tune/tune.h"
#include "tune/tune_service_impl.h"
#include "plugins/transponder/transponder.h"
#include "transponder/transponder_service_impl.h"

namespace mavsdk {
namespace mavsdk_server {

class GrpcServer {
public:
    GrpcServer(Mavsdk& mavsdk) :
        _core(mavsdk),
        _action_lazy_plugin(mavsdk),
        _action_service(_action_lazy_plugin),
        _action_server_lazy_plugin(mavsdk),
        _action_server_service(_action_server_lazy_plugin),
        _calibration_lazy_plugin(mavsdk),
        _calibration_service(_calibration_lazy_plugin),
        _camera_lazy_plugin(mavsdk),
        _camera_service(_camera_lazy_plugin),
        _camera_server_lazy_plugin(mavsdk),
        _camera_server_service(_camera_server_lazy_plugin),
        _failure_lazy_plugin(mavsdk),
        _failure_service(_failure_lazy_plugin),
        _follow_me_lazy_plugin(mavsdk),
        _follow_me_service(_follow_me_lazy_plugin),
        _ftp_lazy_plugin(mavsdk),
        _ftp_service(_ftp_lazy_plugin),
        _geofence_lazy_plugin(mavsdk),
        _geofence_service(_geofence_lazy_plugin),
        _gimbal_lazy_plugin(mavsdk),
        _gimbal_service(_gimbal_lazy_plugin),
        _info_lazy_plugin(mavsdk),
        _info_service(_info_lazy_plugin),
        _log_files_lazy_plugin(mavsdk),
        _log_files_service(_log_files_lazy_plugin),
        _manual_control_lazy_plugin(mavsdk),
        _manual_control_service(_manual_control_lazy_plugin),
        _mission_lazy_plugin(mavsdk),
        _mission_service(_mission_lazy_plugin),
        _mission_raw_lazy_plugin(mavsdk),
        _mission_raw_service(_mission_raw_lazy_plugin),
        _mission_raw_server_lazy_plugin(mavsdk),
        _mission_raw_server_service(_mission_raw_server_lazy_plugin),
        _mocap_lazy_plugin(mavsdk),
        _mocap_service(_mocap_lazy_plugin),
        _offboard_lazy_plugin(mavsdk),
        _offboard_service(_offboard_lazy_plugin),
        _param_lazy_plugin(mavsdk),
        _param_service(_param_lazy_plugin),
        _param_server_lazy_plugin(mavsdk),
        _param_server_service(_param_server_lazy_plugin),
        _server_utility_lazy_plugin(mavsdk),
        _server_utility_service(_server_utility_lazy_plugin),
        _shell_lazy_plugin(mavsdk),
        _shell_service(_shell_lazy_plugin),
        _telemetry_lazy_plugin(mavsdk),
        _telemetry_service(_telemetry_lazy_plugin),
        _telemetry_server_lazy_plugin(mavsdk),
        _telemetry_server_service(_telemetry_server_lazy_plugin),
        _tracking_server_lazy_plugin(mavsdk),
        _tracking_server_service(_tracking_server_lazy_plugin),
        _tune_lazy_plugin(mavsdk),
        _tune_service(_tune_lazy_plugin),
        _transponder_lazy_plugin(mavsdk),
        _transponder_service(_transponder_lazy_plugin)
    {}

    int run();
    void wait();
    void stop();
    void set_port(int port);

private:
    void setup_port(grpc::ServerBuilder& builder);

    CoreServiceImpl<> _core;
    LazyPlugin<Action> _action_lazy_plugin;
    ActionServiceImpl<> _action_service;
    LazyServerPlugin<ActionServer> _action_server_lazy_plugin;
    ActionServerServiceImpl<> _action_server_service;
    LazyPlugin<Calibration> _calibration_lazy_plugin;
    CalibrationServiceImpl<> _calibration_service;
    LazyPlugin<Camera> _camera_lazy_plugin;
    CameraServiceImpl<> _camera_service;
    LazyServerPlugin<CameraServer> _camera_server_lazy_plugin;
    CameraServerServiceImpl<> _camera_server_service;
    LazyPlugin<Failure> _failure_lazy_plugin;
    FailureServiceImpl<> _failure_service;
    LazyPlugin<FollowMe> _follow_me_lazy_plugin;
    FollowMeServiceImpl<> _follow_me_service;
    LazyPlugin<Ftp> _ftp_lazy_plugin;
    FtpServiceImpl<> _ftp_service;
    LazyPlugin<Geofence> _geofence_lazy_plugin;
    GeofenceServiceImpl<> _geofence_service;
    LazyPlugin<Gimbal> _gimbal_lazy_plugin;
    GimbalServiceImpl<> _gimbal_service;
    LazyPlugin<Info> _info_lazy_plugin;
    InfoServiceImpl<> _info_service;
    LazyPlugin<LogFiles> _log_files_lazy_plugin;
    LogFilesServiceImpl<> _log_files_service;
    LazyPlugin<ManualControl> _manual_control_lazy_plugin;
    ManualControlServiceImpl<> _manual_control_service;
    LazyPlugin<Mission> _mission_lazy_plugin;
    MissionServiceImpl<> _mission_service;
    LazyPlugin<MissionRaw> _mission_raw_lazy_plugin;
    MissionRawServiceImpl<> _mission_raw_service;
    LazyServerPlugin<MissionRawServer> _mission_raw_server_lazy_plugin;
    MissionRawServerServiceImpl<> _mission_raw_server_service;
    LazyPlugin<Mocap> _mocap_lazy_plugin;
    MocapServiceImpl<> _mocap_service;
    LazyPlugin<Offboard> _offboard_lazy_plugin;
    OffboardServiceImpl<> _offboard_service;
    LazyPlugin<Param> _param_lazy_plugin;
    ParamServiceImpl<> _param_service;
    LazyServerPlugin<ParamServer> _param_server_lazy_plugin;
    ParamServerServiceImpl<> _param_server_service;
    LazyPlugin<ServerUtility> _server_utility_lazy_plugin;
    ServerUtilityServiceImpl<> _server_utility_service;
    LazyPlugin<Shell> _shell_lazy_plugin;
    ShellServiceImpl<> _shell_service;
    LazyPlugin<Telemetry> _telemetry_lazy_plugin;
    TelemetryServiceImpl<> _telemetry_service;
    LazyServerPlugin<TelemetryServer> _telemetry_server_lazy_plugin;
    TelemetryServerServiceImpl<> _telemetry_server_service;
    LazyServerPlugin<TrackingServer> _tracking_server_lazy_plugin;
    TrackingServerServiceImpl<> _tracking_server_service;
    LazyPlugin<Tune> _tune_lazy_plugin;
    TuneServiceImpl<> _tune_service;
    LazyPlugin<Transponder> _transponder_lazy_plugin;
    TransponderServiceImpl<> _transponder_service;

    std::unique_ptr<grpc::Server> _server;

    int _port{0};
    int _bound_port{0};
};

} // namespace mavsdk_server
} // namespace mavsdk
