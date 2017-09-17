#pragma once

#include <sstream>

#if ANDROID
#include <android/log.h>
#else
#include <iostream>
#endif

namespace dronecore {

class Debug
{
// TODO: This needs to be split into Debug(), Info(), Warn(), Err().
//       For now, we just print all Debug() even in Release mode.
//#ifdef DEBUG
#if 1
public:
    Debug() : _s() {}

    template <typename T>
    Debug &operator << (const T &x)
    {
        _s << x;
        return *this;
    }

    ~Debug()
    {
#if ANDROID
        __android_log_print(ANDROID_LOG_DEBUG, "DroneCore", "%s", _s.str().c_str());
#else
        std::cout << _s.str() << std::endl;
#endif
    }

private:
    std::stringstream _s;

#else
public:
    template <typename T>
    Debug &operator << (const T &x)
    {
        UNUSED(x);
        return *this;
    }
#endif
};

} // namespace dronecore
