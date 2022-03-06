#pragma once

/*
 * Include windows.h without Windows Sockets 1.1 to prevent conflicts with
 * Windows Sockets 2.0.
 */
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/*
 * Remove min and max macros.
 */
#ifndef NONINMAX
#define NOMINMAX
#endif

#include <windows.h>
