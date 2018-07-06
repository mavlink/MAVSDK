#pragma once

#include <memory>

namespace dronecode_sdk {
namespace backend {

class DronecodeSDKBackend {
public:
    DronecodeSDKBackend();
    ~DronecodeSDKBackend();
    DronecodeSDKBackend(DronecodeSDKBackend &&) = delete;
    DronecodeSDKBackend &operator=(DronecodeSDKBackend &&) = delete;

    void startGRPCServer();
    void connect(const int mavlink_listen_port = 14540);
    void wait();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};

} // namespace backend
} // namespace dronecode_sdk
