#include "backend.h"

int main(int argc, char **argv)
{
    dronecore::backend::DroneCoreBackend backend;

    if (!backend.run()) {
        return 1;
    } else {
        return 0;
    }
}
