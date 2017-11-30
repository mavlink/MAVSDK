#include "device_impl.h"
#include "global_include.h"
#include "plugin_impl_base.h"


namespace dronecore {

PluginImplBase::PluginImplBase() :
    _parent()
{
}

void PluginImplBase::set_parent(DeviceImpl *parent)
{
    _parent = parent;
}

// TODO: remove this again
#ifdef __GNUG__ // GNU C++ compiler
#include <cxxabi.h>
#include <stdlib.h>
#include <string>
std::string demangle(const char *mangled_name)
{
    std::string result;
    std::size_t len = 0;
    int status = 0;
    char *ptr = __cxxabiv1::__cxa_demangle(mangled_name, nullptr, &len, &status);
    if (status == 0) {
        result = ptr;
    } else {
        result = "demangle error";
    }
    ::free(ptr);
    return result;
}
#else
#include <string>
std::string demangle(const char *name)
{
    return name;
}
#endif // __GNUG__

void PluginImplBase::enable()
{
    LogWarn() << "Plugin " << demangle(typeid(*this).name()) << " does not implement enable()";
}

void PluginImplBase::disable()
{
    LogWarn() << "Plugin " << demangle(typeid(*this).name()) << " does not implement disable()";
}

} // namespace dronecore
