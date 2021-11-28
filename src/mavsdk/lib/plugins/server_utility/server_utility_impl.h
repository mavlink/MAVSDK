#pragma once

#include "plugins/server_utility/server_utility.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class ServerUtilityImpl : public PluginImplBase {
public:
    explicit ServerUtilityImpl(System& system);
    explicit ServerUtilityImpl(std::shared_ptr<System> system);
    ~ServerUtilityImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;





    ServerUtility::Result send_status_text(ServerUtility::StatusTextType type, std::string text);



private:
};

} // namespace mavsdk