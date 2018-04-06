#pragma once

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default"))) void runBackend(int mavlink_listen_port,
                                                       void (*onServerStarted)(void *), void *context);

#ifdef __cplusplus
}
#endif
