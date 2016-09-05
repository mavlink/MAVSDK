#include "info_impl.h"

namespace dronelink {

InfoImpl::InfoImpl() :
    _version(0),
    _uuid(0)
{
}

InfoImpl::~InfoImpl()
{
}

bool InfoImpl::is_complete() const
{
    if (_version == 0) {
        return false;
    }
    else if (_uuid == 0) {
        return false;
    }

    return true;
}

unsigned InfoImpl::get_version() const
{
    return _version;
}

uint64_t InfoImpl::get_uuid() const
{
    return _uuid;
}

void InfoImpl::set_version(unsigned version)
{
    _version = version;
}

void InfoImpl::set_uuid(uint64_t uuid)
{
    _uuid = uuid;
}


} // namespace dronelink
