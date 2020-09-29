#pragma once

#include "plugins/failure/failure.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class FailureImpl : public PluginImplBase {
public:
    explicit FailureImpl(System& system);
    explicit FailureImpl(std::shared_ptr<System> system);
    ~FailureImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Failure::Result
    inject(Failure::FailureUnit failure_unit, Failure::FailureType failure_type, int32_t instance);

private:
    Failure::Result failure_result_from_command_result(MAVLinkCommands::Result command_result);

    enum class EnabledState {
        Init,
        Enabled,
        Disabled,
        Unknown,
    };

    std::atomic<EnabledState> _enabled{EnabledState::Unknown};
};

} // namespace mavsdk
