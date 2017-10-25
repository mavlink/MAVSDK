#pragma once

#define UNUSED(x) (void)(x)

#include <chrono>
#include <thread>

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

#ifndef M_PI_F
#define M_PI_F float(M_PI)
#endif


namespace dronecore {

typedef std::chrono::time_point<std::chrono::steady_clock> dl_time_t;

dl_time_t steady_time();
dl_time_t steady_time_in_future(double duration_s);

double elapsed_s();
double elapsed_since_s(const dl_time_t &since);

double to_rad_from_deg(double deg);
double to_deg_from_rad(double rad);

float to_rad_from_deg(float deg);
float to_deg_from_rad(float rad);

bool are_equal(float one, float two);
bool are_equal(double one, double two);

} // namespace dronecore
