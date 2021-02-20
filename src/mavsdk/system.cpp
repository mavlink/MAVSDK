#include "system.h"
#include "global_include.h"
#include "mavsdk_impl.h"
#include "mavlink_include.h"
#include "system_impl.h"
#include "plugin_impl_base.h"
#include <functional>
#include <algorithm>
#include "px4_custom_mode.h"

// Set to 1 to log incoming/outgoing mavlink messages.
#define MESSAGE_DEBUGGING 0

namespace mavsdk {

using namespace std::placeholders; // for `_1`

System::System(MavsdkImpl& parent, uint8_t system_id, uint8_t component_id, bool connected) :
    _system_impl(std::make_unique<SystemImpl>(parent, system_id, component_id, connected))
{}

System::~System() {}

bool System::is_standalone() const
{
    return _system_impl->is_standalone();
}

bool System::has_autopilot() const
{
    return _system_impl->has_autopilot();
}

bool System::has_camera(int camera_id) const
{
    return _system_impl->has_camera(camera_id);
}

bool System::has_gimbal() const
{
    return _system_impl->has_gimbal();
}

bool System::is_connected() const
{
    return _system_impl->is_connected();
}

uint64_t System::get_uuid() const
{
    // We want to support UUIDs if the autopilot tells us.
    return _system_impl->get_uuid();
}

uint8_t System::get_system_id() const
{
    return _system_impl->get_system_id();
}

void System::subscribe_is_connected(IsConnectedCallback callback)
{
    return _system_impl->subscribe_is_connected(callback);
}

void System::register_component_discovered_callback(discover_callback_t callback) const
{
    return _system_impl->register_component_discovered_callback(callback);
}

void System::enable_timesync()
{
    _system_impl->enable_timesync();
}

Action* System::action()
{
    return _system_impl->action();
}

Calibration* System::calibration()
{
    return _system_impl->calibration();
}

Camera* System::camera()
{
    return _system_impl->camera();
}

Failure* System::failure()
{
    return _system_impl->failure();
}

FollowMe* System::follow_me()
{
    return _system_impl->follow_me();
}

Ftp* System::ftp()
{
    return _system_impl->ftp();
}

Geofence* System::geofence()
{
    return _system_impl->geofence();
}

Gimbal* System::gimbal()
{
    return _system_impl->gimbal();
}

Info* System::info()
{
    return _system_impl->info();
}

LogFiles* System::log_files()
{
    return _system_impl->log_files();
}

ManualControl* System::manual_control()
{
    return _system_impl->manual_control();
}

MavlinkPassthrough* System::mavlink_passthrough()
{
    return _system_impl->mavlink_passthrough();
}

Mission* System::mission()
{
    return _system_impl->mission();
}

MissionRaw* System::mission_raw()
{
    return _system_impl->mission_raw();
}

Mocap* System::mocap()
{
    return _system_impl->mocap();
}

Offboard* System::offboard()
{
    return _system_impl->offboard();
}

Param* System::param()
{
    return _system_impl->param();
}

Shell* System::shell()
{
    return _system_impl->shell();
}

Telemetry* System::telemetry()
{
    return _system_impl->telemetry();
}

Tune* System::tune()
{
    return _system_impl->tune();
}

} // namespace mavsdk
