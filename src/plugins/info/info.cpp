#include "plugins/info/info.h"
#include "info_impl.h"

namespace mavsdk {

Info::Info(System& system) : PluginBase(), _impl{new InfoImpl(system)} {}

Info::~Info() {}

std::pair<Info::Result, Info::Identification> Info::get_identification() const
{
    return _impl->get_identification();
}

std::pair<Info::Result, Info::Version> Info::get_version() const
{
    return _impl->get_version();
}

std::pair<Info::Result, Info::Product> Info::get_product() const
{
    return _impl->get_product();
}

std::pair<Info::Result, Info::FlightInfo> Info::get_flight_information() const
{
    return _impl->get_flight_information();
}

std::string Info::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::INFORMATION_NOT_RECEIVED_YET:
            return "Information not received yet";
        default:
            // PASSTHROUGH
        case Result::UNKNOWN:
            return "Unknown";
    }
}

} // namespace mavsdk
