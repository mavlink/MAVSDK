#include <iomanip>

#include "winch_reel_impl.h"
#include "plugins/winch_reel/winch_reel.h"

namespace mavsdk {

using Status = WinchReel::Status;

WinchReel::WinchReel(System& system) : PluginBase(), _impl{std::make_unique<WinchReelImpl>(system)} {}

WinchReel::WinchReel(std::shared_ptr<System> system) :
    PluginBase(),
    _impl{std::make_unique<WinchReelImpl>(system)}
{}

WinchReel::~WinchReel() {}

WinchReel::StatusHandle WinchReel::subscribe_status(const StatusCallback& callback)
{
    return _impl->subscribe_status(callback);
}

void WinchReel::unsubscribe_status(StatusHandle handle)
{
    _impl->unsubscribe_status(handle);
}

WinchReel::Status WinchReel::status() const
{
    return _impl->status();
}

void WinchReel::relax_async(uint32_t instance, const ResultCallback callback)
{
    _impl->relax_async(instance, callback);
}

WinchReel::Result WinchReel::relax(uint32_t instance) const
{
    return _impl->relax(instance);
}

void WinchReel::lock_async(uint32_t instance, const ResultCallback callback)
{
    _impl->lock_async(instance, callback);
}

WinchReel::Result WinchReel::lock(uint32_t instance) const
{
    return _impl->lock(instance);
}

void WinchReel::deliver_async(uint32_t instance, const ResultCallback callback)
{
    _impl->deliver_async(instance, callback);
}

WinchReel::Result WinchReel::deliver(uint32_t instance) const
{
    return _impl->deliver(instance);
}

void WinchReel::hold_async(uint32_t instance, const ResultCallback callback)
{
    _impl->hold_async(instance, callback);
}

WinchReel::Result WinchReel::hold(uint32_t instance) const
{
    return _impl->hold(instance);
}

void WinchReel::retract_async(uint32_t instance, const ResultCallback callback)
{
    _impl->retract_async(instance, callback);
}

WinchReel::Result WinchReel::retract(uint32_t instance) const
{
    return _impl->retract(instance);
}

void WinchReel::load_line_async(uint32_t instance, const ResultCallback callback)
{
    _impl->load_line_async(instance, callback);
}

WinchReel::Result WinchReel::load_line(uint32_t instance) const
{
    return _impl->load_line(instance);
}

void WinchReel::load_payload_async(uint32_t instance, const ResultCallback callback)
{
    _impl->load_payload_async(instance, callback);
}

WinchReel::Result WinchReel::load_payload(uint32_t instance) const
{
    return _impl->load_payload(instance);
}

std::ostream& operator<<(std::ostream& str, WinchReel::StatusEnum const& status_enum)
{
    switch (status_enum) {
        case WinchReel::StatusEnum::Undefined:
            return str << "Undefined";
        case WinchReel::StatusEnum::Bootup:
            return str << "Bootup";
        case WinchReel::StatusEnum::Locked:
            return str << "Locked";
        case WinchReel::StatusEnum::Freewheel:
            return str << "Freewheel";
        case WinchReel::StatusEnum::Hold:
            return str << "Hold";
        case WinchReel::StatusEnum::ReelUp:
            return str << "ReelUp";
        case WinchReel::StatusEnum::ReelDown:
            return str << "ReelDown";
        case WinchReel::StatusEnum::DropAndArrest:
            return str << "DropAndArrest";
        case WinchReel::StatusEnum::GroundSense:
            return str << "GroundSense";
        case WinchReel::StatusEnum::ReturnToHome:
            return str << "ReturnToHome";
        case WinchReel::StatusEnum::Redeliver:
            return str << "Redeliver";
        case WinchReel::StatusEnum::AbandonLine:
            return str << "AbandonLine";
        case WinchReel::StatusEnum::Locking:
            return str << "Locking";
        case WinchReel::StatusEnum::LoadPackage:
            return str << "LoadPackage";
        case WinchReel::StatusEnum::LoadLine:
            return str << "LoadLine";
        case WinchReel::StatusEnum::Failsafe:
            return str << "Failsafe";
        case WinchReel::StatusEnum::Unlocking:
            return str << "Unlocking";
        default:
            return str << "Unknown";
    }
}

bool operator==(const WinchReel::Status& lhs, const WinchReel::Status& rhs)
{
    return (rhs.time_usec == lhs.time_usec) &&
           ((std::isnan(rhs.line_length_m) && std::isnan(lhs.line_length_m)) ||
            rhs.line_length_m == lhs.line_length_m) &&
           ((std::isnan(rhs.speed_m_s) && std::isnan(lhs.speed_m_s)) ||
            rhs.speed_m_s == lhs.speed_m_s) &&
           ((std::isnan(rhs.tension_kg) && std::isnan(lhs.tension_kg)) ||
            rhs.tension_kg == lhs.tension_kg) &&
           ((std::isnan(rhs.voltage_v) && std::isnan(lhs.voltage_v)) ||
            rhs.voltage_v == lhs.voltage_v) &&
           ((std::isnan(rhs.current_a) && std::isnan(lhs.current_a)) ||
            rhs.current_a == lhs.current_a) &&
           (rhs.temperature_c == lhs.temperature_c) && (rhs.status_enum == lhs.status_enum);
}

std::ostream& operator<<(std::ostream& str, WinchReel::Status const& status)
{
    str << std::setprecision(15);
    str << "status:" << '\n' << "{\n";
    str << "    time_usec: " << status.time_usec << '\n';
    str << "    line_length_m: " << status.line_length_m << '\n';
    str << "    speed_m_s: " << status.speed_m_s << '\n';
    str << "    tension_kg: " << status.tension_kg << '\n';
    str << "    voltage_v: " << status.voltage_v << '\n';
    str << "    current_a: " << status.current_a << '\n';
    str << "    temperature_c: " << status.temperature_c << '\n';
    str << "    status_enum: " << status.status_enum << '\n';
    str << '}';
    return str;
}

std::ostream& operator<<(std::ostream& str, WinchReel::WinchAction const& winch_action)
{
    switch (winch_action) {
        case WinchReel::WinchAction::Relaxed:
            return str << "Relaxed";
        case WinchReel::WinchAction::Lock:
            return str << "Lock";
        case WinchReel::WinchAction::Deliver:
            return str << "Deliver";
        case WinchReel::WinchAction::Hold:
            return str << "Hold";
        case WinchReel::WinchAction::Retract:
            return str << "Retract";
        case WinchReel::WinchAction::LoadLine:
            return str << "Load Line";
        case WinchReel::WinchAction::LoadPayload:
            return str << "Load Payload";
        default:
            return str << "Unknown";
    }
}

} // namespace mavsdk