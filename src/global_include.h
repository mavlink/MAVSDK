#pragma once

#define UNUSED(x) (void)(x)

#include <iostream>
#include <sstream>

namespace dronelink {

class Debug
{
#ifdef DEBUG
public:
    Debug() : _s() {}

    template <typename T>
    Debug &operator << (const T &x) {
        _s << x;
        return *this;
    }

    ~Debug() {
        std::cout << _s.str() << std::endl;
    }

private:
    std::stringstream _s;

#else
public:
    template <typename T>
    Debug &operator << (const T &x) {
        UNUSED(x);
        return *this;
    }
#endif
};



} // namespace dronelink
