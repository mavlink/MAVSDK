#pragma once

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <memory>
#include "mavsdk.h"

#include "plugins/action/action.h"
#include "action/action_service_impl.h"
#include "plugins/calibration/calibration.h"
#include "calibration/calibration_service_impl.h"
#include "plugins/camera/camera.h"
#include "camera/camera_service_impl.h"
#include "core/core_service_impl.h"
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
#include "plugins/shell/shell.h"
#include "shell/shell_service_impl.h"
#include "plugins/telemetry/telemetry.h"
#include "telemetry/telemetry_service_impl.h"
#include "plugins/tune/tune.h"
#include "tune/tune_service_impl.h"

namespace mavsdk {
namespace backend {

class GRPCServer {
public:
    GRPCServer(Mavsdk& dc) :
        _port(0),
        _dc(dc),
        _core(_dc),
        _action(_dc.system()),
        _action_service(_action),
        _calibration(_dc.system()),
        _calibration_service(_calibration),
        _camera(_dc.system()),
        _camera_service(_camera),
        _follow_me(_dc.system()),
        _follow_me_service(_follow_me),
        _ftp(_dc.system()),
        _ftp_service(_ftp),
        _geofence(_dc.system()),
        _geofence_service(_geofence),
        _gimbal(_dc.system()),
        _gimbal_service(_gimbal),
        _info(_dc.system()),
        _info_service(_info),
        _log_files(_dc.system()),
        _log_files_service(_log_files),
        _mission(_dc.system()),
        _mission_service(_mission),
        _mission_raw(_dc.system()),
        _mission_raw_service(_mission_raw),
        _mocap(_dc.system()),
        _mocap_service(_mocap),
        _offboard(_dc.system()),
        _offboard_service(_offboard),
        _param(_dc.system()),
        _param_service(_param),
        _shell(_dc.system()),
        _shell_service(_shell),
        _telemetry(_dc.system()),
        _telemetry_service(_telemetry),
        _tune(_dc.system()),
        _tune_service(_tune),
    {}

    int run();
    void wait();
    void stop();
    void set_port(int port);

private:
    void setup_port(grpc::ServerBuilder& builder);

    Mavsdk& _dc;
    CoreServiceImpl<> _core;
    Action _action;
    ActionServiceImpl<> _action_service;
    Calibration _calibration;
    CalibrationServiceImpl<> _calibration_service;
    Camera _camera;
    CameraServiceImpl<> _camera_service;
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
    Shell _shell;
    ShellServiceImpl<> _shell_service;
    Telemetry _telemetry;
    TelemetryServiceImpl<> _telemetry_service;
    Tune _tune;
    TuneServiceImpl<> _tune_service;

    std::unique_ptr<grpc::Server> _server;

    int _port;
    int _bound_port = 0;
};

} // namespace backend
} // namespace mavsdk
