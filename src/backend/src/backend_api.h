#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WINDOWS
#define DLLExport __declspec(dllexport)
#else
#define DLLExport __attribute__((visibility("default")))
#endif

DLLExport void
runBackend(const char* connection_url, void (*onServerStarted)(void*), void* context);

#ifdef __cplusplus
}
#endif
