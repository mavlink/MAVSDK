#pragma once

#include <memory>
#include <string>

namespace dronecode_sdk {
namespace backend {

class DronecodeSDKBackend {
public:
    DronecodeSDKBackend();
    ~DronecodeSDKBackend();
    DronecodeSDKBackend(DronecodeSDKBackend &&) = delete;
    DronecodeSDKBackend &operator=(DronecodeSDKBackend &&) = delete;

    void startGRPCServer();
    void connect(const std::string &connection_url = "udp://");
    void wait();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace backend
} // namespace dronecode_sdk
