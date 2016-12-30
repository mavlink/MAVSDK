#include "info.h"
#include "info_impl.h"

namespace dronelink {

Info::Info(InfoImpl *impl) :
    _impl(impl)
{
}

Info::~Info()
{
}

uint64_t Info::uuid() const
{
    return _impl->get_uuid();
}

bool Info::is_complete() const
{
    return _impl->is_complete();
}

Info::Version Info::get_version() const
{
    return _impl->get_version();
}

} // namespace dronelink
