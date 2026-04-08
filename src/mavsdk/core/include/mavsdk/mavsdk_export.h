#pragma once

/**
 * @brief Marks a class or function as part of the public MAVSDK C++ API.
 *
 * On Unix the library is built with -fvisibility=hidden, so only explicitly
 * annotated symbols are exported from the shared library.
 *
 * On Windows, dllexport/dllimport are only used when building/consuming
 * a shared library (MAVSDK_SHARED is defined). For static builds these
 * macros are no-ops.
 */

#if defined(_WIN32) && defined(MAVSDK_SHARED)
#ifdef MAVSDK_BUILD
#define MAVSDK_PUBLIC __declspec(dllexport)
#else
#define MAVSDK_PUBLIC __declspec(dllimport)
#endif
#elif !defined(_WIN32)
#define MAVSDK_PUBLIC __attribute__((visibility("default")))
#else
#define MAVSDK_PUBLIC
#endif

/**
 * @brief Marks an internal class or function that unit tests need to access.
 */
#if defined(_WIN32) && defined(MAVSDK_SHARED)
#ifdef MAVSDK_BUILD
#define MAVSDK_TEST_EXPORT __declspec(dllexport)
#else
#define MAVSDK_TEST_EXPORT __declspec(dllimport)
#endif
#elif !defined(_WIN32)
#define MAVSDK_TEST_EXPORT __attribute__((visibility("default")))
#else
#define MAVSDK_TEST_EXPORT
#endif

/**
 * @brief Export attribute for explicit template instantiations.
 *
 * On Unix, MAVSDK_PUBLIC on the class template already sets visibility,
 * so explicit instantiations don't need it (and GCC warns if you add it).
 * On Windows shared builds, dllexport on the explicit instantiation is required.
 */
#if defined(_WIN32) && defined(MAVSDK_SHARED)
#define MAVSDK_TEMPL_INST MAVSDK_PUBLIC
#else
#define MAVSDK_TEMPL_INST
#endif
