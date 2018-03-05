#include "backend_api.h"

#include "backend.h"

int runBackend(const int mavlink_listen_port)
{
    dronecore::backend::DroneCoreBackend backend;
    bool had_error = backend.run(mavlink_listen_port);

    if (had_error) {
        return 1;
    } else {
        return 0;
    }
}
