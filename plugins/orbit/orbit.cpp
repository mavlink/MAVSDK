#include "orbit.h"
#include "orbit_impl.h"

namespace dronecore {

Orbit::Orbit(System &system) : PluginBase(), _impl{new OrbitImpl(system)} {}

Orbit::~Orbit() {}

Orbit::Result Orbit::start() const
{
    return _impl->start();
}

Orbit::Result Orbit::stop() const
{
    return _impl->stop();
}

std::string Orbit::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::NO_SYSTEM:
            return "No system";
        case Result::CONNECTION_ERROR:
            return "Connection error";
        case Result::BUSY:
            return "Busy";
        case Result::COMMAND_DENIED:
            return "Command denied";
        case Result::TIMEOUT:
            return "Timeout";
        case Result::NOT_ACTIVE:
            return "Orbit is not active";
        case Result::SET_CONFIG_FAILED:
            return "Failed to set configuration";
        case Result::UNKNOWN:
        // FALLTHROUGH
        default:
            return "Unknown";
    }
}

} // namespace dronecore
