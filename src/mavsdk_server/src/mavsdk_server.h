#pragma once

#include <memory>
#include <string>

class MavsdkServer {
public:
    MavsdkServer();
    ~MavsdkServer();
    MavsdkServer(MavsdkServer&&) = delete;
    MavsdkServer& operator=(MavsdkServer&&) = delete;

    int startGRPCServer(int port);
    void connect(const std::string& connection_url = "udp://");
    void wait();
    void stop();
    int getPort();

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
