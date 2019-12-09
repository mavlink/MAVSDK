#pragma once

#include <memory>
#include <string>

namespace mavsdk {
namespace backend {

class MavsdkBackend {
public:
    MavsdkBackend();
    ~MavsdkBackend();
    MavsdkBackend(MavsdkBackend&&) = delete;
    MavsdkBackend& operator=(MavsdkBackend&&) = delete;

    int startGRPCServer(int port);
    void connect(const std::string& connection_url = "udp://");
    void wait();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace backend
} // namespace mavsdk
