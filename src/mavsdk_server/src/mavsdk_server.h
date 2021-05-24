#pragma once

#include <memory>
#include <string>

// This is a struct because it is also exported to the C interface.
struct MavsdkServer {
public:
    MavsdkServer();
    ~MavsdkServer();
    MavsdkServer(MavsdkServer&&) = delete;
    MavsdkServer& operator=(MavsdkServer&&) = delete;

    int startGrpcServer(int port);
    void connect(const std::string& connection_url = "udp://");
    void wait();
    void stop();
    int getPort();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
