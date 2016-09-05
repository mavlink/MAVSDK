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

bool Info::is_complete() const
{
    return _impl->is_complete();
}

unsigned Info::get_version() const
{
    return _impl->get_version();
}

uint64_t Info::get_uuid() const
{
    return _impl->get_uuid();
}


} // namespace dronelink
