#pragma once

#include <grpcpp/server.h>
#include <grpcpp/server_builder.h>
#include <memory>

#include "plugins/action/action.h"
#include "action/action_service_impl.h"
#include "plugins/calibration/calibration.h"
#include "calibration/calibration_service_impl.h"
#include "plugins/camera/camera.h"
#include "camera/camera_service_impl.h"
#include "core/core_service_impl.h"
#include "mavsdk.h"
#include "plugins/mission/mission.h"
#include "mission/mission_service_impl.h"
#include "telemetry/telemetry_service_impl.h"
#include "info/info_service_impl.h"
#include "plugins/geofence/geofence.h"
#include "geofence/geofence_service_impl.h"
#include "plugins/gimbal/gimbal.h"
#include "gimbal/gimbal_service_impl.h"
#include "plugins/param/param.h"
#include "param/param_service_impl.h"
#include "plugins/offboard/offboard.h"
#include "offboard/offboard_service_impl.h"
#include "plugins/shell/shell.h"
#include "shell/shell_service_impl.h"
#include "plugins/mocap/mocap.h"
#include "mocap/mocap_service_impl.h"

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
        _geofence(_dc.system()),
        _geofence_service(_geofence),
        _gimbal(_dc.system()),
        _gimbal_service(_gimbal),
        _camera(_dc.system()),
        _camera_service(_camera),
        _mission(_dc.system()),
        _mission_service(_mission),
        _offboard(_dc.system()),
        _offboard_service(_offboard),
        _telemetry(_dc.system()),
        _telemetry_service(_telemetry),
        _info(_dc.system()),
        _info_service(_info),
        _param(_dc.system()),
        _param_service(_param),
        _shell(_dc.system()),
        _shell_service(_shell),
        _mocap(_dc.system()),
        _mocap_service(_mocap)
    {}

    void set_port(int port);
    int run();
    void wait();

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
    Geofence _geofence;
    GeofenceServiceImpl<> _geofence_service;
    Gimbal _gimbal;
    GimbalServiceImpl<> _gimbal_service;
    Mission _mission;
    MissionServiceImpl<> _mission_service;
    Offboard _offboard;
    OffboardServiceImpl<> _offboard_service;
    Telemetry _telemetry;
    TelemetryServiceImpl<> _telemetry_service;
    Info _info;
    InfoServiceImpl<> _info_service;
    Param _param;
    ParamServiceImpl<> _param_service;
    Shell _shell;
    ShellServiceImpl<> _shell_service;
    Mocap _mocap;
    MocapServiceImpl<> _mocap_service;

    std::unique_ptr<grpc::Server> _server;

    int _port;
    int _bound_port = 0;
};

} // namespace backend
} // namespace mavsdk
