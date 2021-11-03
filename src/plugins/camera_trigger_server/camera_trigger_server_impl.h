#pragma once

#include "plugins/camera_trigger_server/camera_trigger_server.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class CameraTriggerServerImpl : public PluginImplBase {
public:
    explicit CameraTriggerServerImpl(System& system);
    explicit CameraTriggerServerImpl(std::shared_ptr<System> system);
    ~CameraTriggerServerImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void subscribe_capture(CameraTriggerServer::CaptureCallback callback);

private:
    struct {
        CameraTriggerServer::CaptureCallback callback;
        std::mutex mutex;
        uint32_t prev_sequence_num;
    } _capture;

};

} // namespace mavsdk