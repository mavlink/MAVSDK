#pragma once

#include "plugins/mission_plan/mission_plan.h"
#include "plugin_impl_base.h"

namespace mavsdk {

class MissionPlanImpl : public PluginImplBase {
public:
    explicit MissionPlanImpl(System& system);
    explicit MissionPlanImpl(std::shared_ptr<System> system);
    ~MissionPlanImpl();

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    void
    upload_mission_plan_async(std::string json_plan, const MissionPlan::ResultCallback callback);

    MissionPlan::Result upload_mission_plan(std::string json_plan);

    MissionPlan::Result cancel_mission_plan_upload();

    void clear_mission_plan_async(const MissionPlan::ResultCallback callback);

    MissionPlan::Result clear_mission_plan();

    void start_mission_async(const MissionPlan::ResultCallback callback);

    MissionPlan::Result start_mission();

    void pause_mission_async(const MissionPlan::ResultCallback callback);

    MissionPlan::Result pause_mission();

    void set_current_mission_item_async(int32_t index, const MissionPlan::ResultCallback callback);

    MissionPlan::Result set_current_mission_item(int32_t index);

    std::pair<MissionPlan::Result, bool> is_mission_finished();

    void subscribe_mission_progress(MissionPlan::MissionProgressCallback callback);

    MissionPlan::MissionProgress mission_progress() const;

private:
};

} // namespace mavsdk