#pragma once

#include "plugins/server_utility/server_utility.h"
#include "plugin_impl_base.h"
#include <atomic>
#include <optional>

namespace mavsdk {

class ServerUtilityImpl : public PluginImplBase {
public:
    explicit ServerUtilityImpl(System& system);
    explicit ServerUtilityImpl(std::shared_ptr<System> system);
    ~ServerUtilityImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    ServerUtility::Result send_status_text(ServerUtility::StatusTextType type, std::string text);

    std::optional<MAV_SEVERITY>
    mav_severity_from_status_text_type(ServerUtility::StatusTextType type);

private:
    std::atomic<uint16_t> _unique_send_id{1};
};

} // namespace mavsdk
