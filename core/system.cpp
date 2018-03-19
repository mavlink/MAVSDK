#include "system.h"
#include "global_include.h"
#include "dronecore_impl.h"
#include "mavlink_include.h"
#include "mavlink_system.h"
#include "plugin_impl_base.h"
#include <functional>
#include <algorithm>
#include "px4_custom_mode.h"

// Set to 1 to log incoming/outgoing mavlink messages.
#define MESSAGE_DEBUGGING 0

namespace dronecore {

using namespace std::placeholders; // for `_1`

System::System(DroneCoreImpl &parent,
               uint8_t system_id,
               uint8_t component_id)
{
    _mavlink_system = std::make_shared<MAVLinkSystem>(parent,
                                                      system_id, component_id);
}

System::~System()
{
}

bool System::is_standalone() const
{
    return _mavlink_system->is_standalone();
}

bool System::is_autopilot() const
{
    return _mavlink_system->is_autopilot();
}

bool System::has_camera(uint8_t camera_id) const
{
    return _mavlink_system->has_camera(camera_id);
}

bool System::has_gimbal() const
{
    return _mavlink_system->has_gimbal();
}

void System::add_new_component(uint8_t component_id)
{
    return _mavlink_system->add_new_component(component_id);
}

void System::process_mavlink_message(const mavlink_message_t &message)
{
    return _mavlink_system->process_mavlink_message(message);
}

void System::set_system_id(uint8_t system_id)
{
    return _mavlink_system->set_system_id(system_id);
}

bool System::is_connected() const
{
    return _mavlink_system->is_connected();
}

uint64_t System::get_uuid() const
{
    // We want to support UUIDs if the autopilot tells us.
    return _mavlink_system->get_uuid();
}

uint8_t System::get_system_id() const
{
    return _mavlink_system->get_system_id();
}

} // namespace dronecore
