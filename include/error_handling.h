#pragma once

namespace dronelink {

enum class Result : unsigned
{
    SUCCESS = 0,
    TIMEOUT,
    WRONG_ARGUMENT,
    CONNECTION_ERROR,
    NOT_IMPLEMENTED,
    DEVICE_NOT_CONNECTED,
    DEVICE_BUSY,
    COMMAND_DENIED,
    DESTINATION_IP_UNKNOWN,
};

const char *result_str(Result res);

typedef void (*result_callback_t)(Result result, void *user);

} // namespace dronelink
