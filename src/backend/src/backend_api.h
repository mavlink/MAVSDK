#pragma once

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default"))) void
runBackend(const char *connection_url, void (*onServerStarted)(void *), void *context);

#ifdef __cplusplus
}
#endif
