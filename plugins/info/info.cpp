#include "info.h"
#include "info_impl.h"

namespace dronelink {

Info::Info(InfoImpl *impl) :
    PluginBase(),
    _impl(impl)
{
}

Info::~Info()
{
}

bool Info::is_complete() const
{
    return _impl->is_complete();
}

unsigned Info::get_version() const
{
    return _impl->get_version();
}

} // namespace dronelink
