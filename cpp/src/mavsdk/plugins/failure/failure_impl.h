#pragma once

#include "plugins/failure/failure.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class FailureImpl : public PluginImplBase {
public:
    explicit FailureImpl(System& system);
    explicit FailureImpl(std::shared_ptr<System> system);
    ~FailureImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Failure::Result
    inject(Failure::FailureUnit failure_unit, Failure::FailureType failure_type, int32_t instance);

private:
    static Failure::Result
    failure_result_from_command_result(MavlinkCommandSender::Result command_result);

    enum class EnabledState {
        Init,
        Enabled,
        Disabled,
        Unknown,
    };

    std::atomic<EnabledState> _enabled{EnabledState::Unknown};
    static float failure_unit_to_mavlink_enum(const Failure::FailureUnit& failure_unit);
    static float failure_type_to_mavlink_enum(const Failure::FailureType& failure_type);
};

} // namespace mavsdk
