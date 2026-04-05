#pragma once

#ifndef WINDOWS
#pragma GCC system_header
#endif

#include <curl/curl.h>
#include <curl/easy.h>

// On Windows the build fails if the definition for ERROR is leaked after
// above includes.
//
// The compile error is:
// "illegal token on right side of '::'"
// in Camera::Result::ERROR.
#ifdef ERROR
#undef ERROR
#endif
