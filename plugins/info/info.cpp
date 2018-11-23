#include "plugins/info/info.h"
#include "info_impl.h"

namespace dronecode_sdk {

Info::Info(System &system) : PluginBase(), _impl{new InfoImpl(system)} {}

Info::~Info() {}

std::pair<Info::Result, uint64_t> Info::uuid() const
{
    return _impl->get_uuid();
}

std::pair<Info::Result, Info::Version> Info::get_version() const
{
    return _impl->get_version();
}

std::pair<Info::Result, Info::Product> Info::get_product() const
{
    return _impl->get_product();
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

} // namespace dronecode_sdk
