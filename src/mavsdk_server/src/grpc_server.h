#pragma once

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <memory>

#include "mavsdk.h"
#include "core/core_service_impl.h"
#include "plugins/action/action.h"
#include "action/action_service_impl.h"
#include "plugins/calibration/calibration.h"
#include "calibration/calibration_service_impl.h"
#include "plugins/camera/camera.h"
#include "camera/camera_service_impl.h"
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
#include "plugins/mocap/mocap.h"
#include "mocap/mocap_service_impl.h"
#include "plugins/offboard/offboard.h"
#include "offboard/offboard_service_impl.h"
#include "plugins/param/param.h"
#include "param/param_service_impl.h"
#include "plugins/server_utility/server_utility.h"
#include "server_utility/server_utility_service_impl.h"
#include "plugins/shell/shell.h"
#include "shell/shell_service_impl.h"
#include "plugins/telemetry/telemetry.h"
#include "telemetry/telemetry_service_impl.h"
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
        _mavsdk(mavsdk),
        _core(_mavsdk),
        _action(_mavsdk.system()),
        _action_service(_action),
        _calibration(_mavsdk.system()),
        _calibration_service(_calibration),
        _camera(_mavsdk.system()),
        _camera_service(_camera),
        _failure(_mavsdk.system()),
        _failure_service(_failure),
        _follow_me(_mavsdk.system()),
        _follow_me_service(_follow_me),
        _ftp(_mavsdk.system()),
        _ftp_service(_ftp),
        _geofence(_mavsdk.system()),
        _geofence_service(_geofence),
        _gimbal(_mavsdk.system()),
        _gimbal_service(_gimbal),
        _info(_mavsdk.system()),
        _info_service(_info),
        _log_files(_mavsdk.system()),
        _log_files_service(_log_files),
        _manual_control(_mavsdk.system()),
        _manual_control_service(_manual_control),
        _mission(_mavsdk.system()),
        _mission_service(_mission),
        _mission_raw(_mavsdk.system()),
        _mission_raw_service(_mission_raw),
        _mocap(_mavsdk.system()),
        _mocap_service(_mocap),
        _offboard(_mavsdk.system()),
        _offboard_service(_offboard),
        _param(_mavsdk.system()),
        _param_service(_param),
        _server_utility(_mavsdk.system()),
        _server_utility_service(_server_utility),
        _shell(_mavsdk.system()),
        _shell_service(_shell),
        _telemetry(_mavsdk.system()),
        _telemetry_service(_telemetry),
        _tracking_server(_mavsdk.system()),
        _tracking_server_service(_tracking_server),
        _tune(_mavsdk.system()),
        _tune_service(_tune),
        _transponder(_mavsdk.system()),
        _transponder_service(_transponder)
    {}

    int run();
    void wait();
    void stop();
    void set_port(int port);

private:
    void setup_port(grpc::ServerBuilder& builder);

    Mavsdk& _mavsdk;
    CoreServiceImpl<> _core;
    Action _action;
    ActionServiceImpl<> _action_service;
    Calibration _calibration;
    CalibrationServiceImpl<> _calibration_service;
    Camera _camera;
    CameraServiceImpl<> _camera_service;
    Failure _failure;
    FailureServiceImpl<> _failure_service;
    FollowMe _follow_me;
    FollowMeServiceImpl<> _follow_me_service;
    Ftp _ftp;
    FtpServiceImpl<> _ftp_service;
    Geofence _geofence;
    GeofenceServiceImpl<> _geofence_service;
    Gimbal _gimbal;
    GimbalServiceImpl<> _gimbal_service;
    Info _info;
    InfoServiceImpl<> _info_service;
    LogFiles _log_files;
    LogFilesServiceImpl<> _log_files_service;
    ManualControl _manual_control;
    ManualControlServiceImpl<> _manual_control_service;
    Mission _mission;
    MissionServiceImpl<> _mission_service;
    MissionRaw _mission_raw;
    MissionRawServiceImpl<> _mission_raw_service;
    Mocap _mocap;
    MocapServiceImpl<> _mocap_service;
    Offboard _offboard;
    OffboardServiceImpl<> _offboard_service;
    Param _param;
    ParamServiceImpl<> _param_service;
    ServerUtility _server_utility;
    ServerUtilityServiceImpl<> _server_utility_service;
    Shell _shell;
    ShellServiceImpl<> _shell_service;
    Telemetry _telemetry;
    TelemetryServiceImpl<> _telemetry_service;
    TrackingServer _tracking_server;
    TrackingServerServiceImpl<> _tracking_server_service;
    Tune _tune;
    TuneServiceImpl<> _tune_service;
    Transponder _transponder;
    TransponderServiceImpl<> _transponder_service;

    std::unique_ptr<grpc::Server> _server;

    int _port{0};
    int _bound_port{0};
};

} // namespace mavsdk_server
} // namespace mavsdk
