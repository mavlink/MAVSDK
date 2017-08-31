#pragma once

#define UNUSED(x) (void)(x)

#include <sstream>
#include <chrono>
#include <thread>

#if ANDROID
#include <android/log.h>
#else
#include <iostream>
#endif

#ifdef WINDOWS
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
// cmath doesn't contain M_PI
#include <math.h>
#else
#include <cmath>
#endif

#define MIN(x_, y_) ((x_) > (y_)) ? (y_) : (x_)
#define MAX(x_, y_) ((x_) > (y_)) ? (x_) : (y_)

#ifdef WINDOWS
#define STRNCPY strncpy_s
#else
#define STRNCPY strncpy
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

typedef std::chrono::time_point<std::chrono::steady_clock> dl_time_t;

dl_time_t steady_time();
dl_time_t steady_time_in_future(double duration_s);

double elapsed_s();
double elapsed_since_s(const dl_time_t &since);

double to_rad_from_deg(double deg);
double to_deg_from_rad(double rad);

bool are_equal(float one, float two);
bool are_equal(double one, double two);

} // namespace dronecore
