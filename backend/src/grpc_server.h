#pragma once

#include <grpc++/server.h>
#include <memory>

#include "plugins/action/action.h"
#include "action/action_service_impl.h"
#include "plugins/calibration/calibration.h"
#include "calibration/calibration_service_impl.h"
#include "plugins/camera/camera.h"
#include "camera/camera_service_impl.h"
#include "core/core_service_impl.h"
#include "dronecode_sdk.h"
#include "plugins/mission/mission.h"
#include "mission/mission_service_impl.h"
#include "telemetry/telemetry_service_impl.h"
#include "info/info_service_impl.h"

namespace dronecode_sdk {
namespace backend {

class GRPCServer {
public:
    GRPCServer(DronecodeSDK &dc) :
        _dc(dc),
        _core(_dc),
        _action(_dc.system()),
        _action_service(_action),
        _calibration(_dc.system()),
        _calibration_service(_calibration),
        _camera(_dc.system()),
        _camera_service(_camera),
        _mission(_dc.system()),
        _mission_service(_mission),
        _telemetry(_dc.system()),
        _telemetry_service(_telemetry),
        _info(_dc.system()),
        _info_service(_info)
    {}

    void run();
    void wait();

private:
    void setup_port(grpc::ServerBuilder &builder);

    DronecodeSDK &_dc;

    CoreServiceImpl<> _core;
    Action _action;
    ActionServiceImpl<> _action_service;
    Calibration _calibration;
    CalibrationServiceImpl<> _calibration_service;
    Camera _camera;
    CameraServiceImpl<> _camera_service;
    Mission _mission;
    MissionServiceImpl<> _mission_service;
    Telemetry _telemetry;
    TelemetryServiceImpl<> _telemetry_service;
    Info _info;
    InfoServiceImpl<> _info_service;

    std::unique_ptr<grpc::Server> _server;
};

} // namespace backend
} // namespace dronecode_sdk
