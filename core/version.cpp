#include "log.h"
#include "version.h"
#include <iostream>

namespace dronecode_sdk {

__attribute__((constructor)) void init(void)
{
    // ostream is not initialized yet during this early init call, therefore
    // we need to manually initialize it.
    // Found in: https://github.com/catchorg/Catch2/issues/461
    std::ios_base::Init m_ostreamInit;

    LogInfo() << "DronecodeSDK version: " << version;
}

__attribute__((destructor)) void fini(void) {}

} // namespace dronecode_sdk
