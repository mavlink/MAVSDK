#pragma once

#include <memory>

namespace dronecore {
namespace backend {

class DroneCoreBackend
{
public:
    DroneCoreBackend();
    ~DroneCoreBackend();
    DroneCoreBackend(DroneCoreBackend &&) = delete;
    DroneCoreBackend &operator=(DroneCoreBackend &&) = delete;

    void startGRPCServer();
    void connect(const int mavlink_listen_port = 14540);
    void wait();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace backend
} // namespace dronecore
