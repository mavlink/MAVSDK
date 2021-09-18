#include "system.h"
#include "mavsdk_impl.h"
#include "system_impl.h"
#include "plugin_impl_base.h"
#include <functional>
#include <utility>
#include "px4_custom_mode.h"

// Set to 1 to log incoming/outgoing mavlink messages.
#define MESSAGE_DEBUGGING 0

namespace mavsdk {

using namespace std::placeholders; // for `_1`

System::System(MavsdkImpl& parent) : _system_impl(std::make_shared<SystemImpl>(parent)) {}

System::~System() = default;

void System::init(uint8_t system_id, uint8_t component_id, bool connected) const
{
    return _system_impl->init(system_id, component_id, connected);
}

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

uint8_t System::get_system_id() const
{
    return _system_impl->get_system_id();
}

std::vector<uint8_t> System::component_ids() const
{
    return _system_impl->component_ids();
}

void System::subscribe_is_connected(IsConnectedCallback callback)
{
    return _system_impl->subscribe_is_connected(std::move(callback));
}

void System::register_component_discovered_callback(DiscoverCallback callback) const
{
    return _system_impl->register_component_discovered_callback(std::move(callback));
}

void System::enable_timesync()
{
    _system_impl->enable_timesync();
}

void System::add_capabilities(uint64_t add_capabilities)
{
    _system_impl->add_capabilities(add_capabilities);
}

void System::set_flight_sw_version(uint32_t flight_sw_version)
{
    _system_impl->set_flight_sw_version(flight_sw_version);
}

void System::set_middleware_sw_version(uint32_t middleware_sw_version)
{
    _system_impl->set_middleware_sw_version(middleware_sw_version);
}

void System::set_os_sw_version(uint32_t os_sw_version)
{
    _system_impl->set_os_sw_version(os_sw_version);
}

void System::set_board_version(uint32_t board_version)
{
    _system_impl->set_board_version(board_version);
}

void System::set_vendor_id(uint16_t vendor_id)
{
    _system_impl->set_vendor_id(vendor_id);
}

void System::set_product_id(uint16_t product_id)
{
    _system_impl->set_product_id(product_id);
}

bool System::set_uid2(std::string uid2)
{
    return _system_impl->set_uid2(std::move(uid2));
}

System::AutopilotVersion System::get_autopilot_version_data()
{
    return _system_impl->get_autopilot_version_data();
}

} // namespace mavsdk
