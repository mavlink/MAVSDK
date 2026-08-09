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
    bool connect(const std::string& connection_url = "udpin://0.0.0.0:14540");
    void wait();
    void stop();
    int getPort();
    void setMavlinkIds(uint8_t system_id, uint8_t component_id);
    // Call before run(). Returns false if the timeout is invalid, in which
    // case the previous one is kept. 0 disables the watchdog.
    bool setHeartbeatWatchdogTimeout(double timeout_s);

private:
    class Impl;
    std::unique_ptr<Impl> _impl;
};
