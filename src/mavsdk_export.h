#ifndef CMAVSDK_MAVSDK_EXPORT_H
#define CMAVSDK_MAVSDK_EXPORT_H

#ifdef _WIN32
#  ifdef CMAVSDK_BUILD
#    define CMAVSDK_EXPORT __declspec(dllexport)
#  else
#    define CMAVSDK_EXPORT __declspec(dllimport)
#  endif
#else
#  define CMAVSDK_EXPORT __attribute__((visibility("default")))
#endif

#endif // CMAVSDK_MAVSDK_EXPORT_H
