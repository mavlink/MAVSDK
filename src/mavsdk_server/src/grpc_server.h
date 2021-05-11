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
        _core(mavsdk),
        _action_service(mavsdk),
        _calibration_service(mavsdk),
        _camera_service(mavsdk),
        _failure_service(mavsdk),
        _follow_me_service(mavsdk),
        _ftp_service(mavsdk),
        _geofence_service(mavsdk),
        _gimbal_service(mavsdk),
        _info_service(mavsdk),
        _log_files_service(mavsdk),
        _manual_control_service(mavsdk),
        _mission_service(mavsdk),
        _mission_raw_service(mavsdk),
        _mocap_service(mavsdk),
        _offboard_service(mavsdk),
        _param_service(mavsdk),
        _server_utility_service(mavsdk),
        _shell_service(mavsdk),
        _telemetry_service(mavsdk),
        _tracking_server_service(mavsdk),
        _tune_service(mavsdk),
        _transponder_service(mavsdk)
    {}

    int run();
    void wait();
    void stop();
    void set_port(int port);

private:
    void setup_port(grpc::ServerBuilder& builder);

    Mavsdk& _mavsdk;
    CoreServiceImpl<> _core;
    ActionServiceImpl<> _action_service;
    CalibrationServiceImpl<> _calibration_service;
    CameraServiceImpl<> _camera_service;
    FailureServiceImpl<> _failure_service;
    FollowMeServiceImpl<> _follow_me_service;
    FtpServiceImpl<> _ftp_service;
    GeofenceServiceImpl<> _geofence_service;
    GimbalServiceImpl<> _gimbal_service;
    InfoServiceImpl<> _info_service;
    LogFilesServiceImpl<> _log_files_service;
    ManualControlServiceImpl<> _manual_control_service;
    MissionServiceImpl<> _mission_service;
    MissionRawServiceImpl<> _mission_raw_service;
    MocapServiceImpl<> _mocap_service;
    OffboardServiceImpl<> _offboard_service;
    ParamServiceImpl<> _param_service;
    ServerUtilityServiceImpl<> _server_utility_service;
    ShellServiceImpl<> _shell_service;
    TelemetryServiceImpl<> _telemetry_service;
    TrackingServerServiceImpl<> _tracking_server_service;
    TuneServiceImpl<> _tune_service;
    TransponderServiceImpl<> _transponder_service;

    std::unique_ptr<grpc::Server> _server;

    int _port{0};
    int _bound_port{0};
};

} // namespace mavsdk_server
} // namespace mavsdk
