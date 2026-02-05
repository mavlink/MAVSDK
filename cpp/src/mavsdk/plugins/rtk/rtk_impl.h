#pragma once

#include "plugins/rtk/rtk.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class RtkImpl : public PluginImplBase {
public:
    explicit RtkImpl(System& system);
    explicit RtkImpl(std::shared_ptr<System> system);
    ~RtkImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Rtk::Result send_rtcm_data(Rtk::RtcmData rtcm_data);

private:
    unsigned _sequence{0};
};

} // namespace mavsdk
