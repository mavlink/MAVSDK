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
    _system_impl(std::make_shared<SystemImpl>(parent, system_id, component_id, connected))
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

void System::register_component_discovered_callback(discover_callback_t callback) const
{
    return _system_impl->register_component_discovered_callback(callback);
}

void System::enable_timesync()
{
    _system_impl->enable_timesync();
}

} // namespace mavsdk
