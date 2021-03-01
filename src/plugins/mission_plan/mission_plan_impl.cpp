#include "mission_plan_impl.h"

namespace mavsdk {

MissionPlanImpl::MissionPlanImpl(System& system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MissionPlanImpl::MissionPlanImpl(std::shared_ptr<System> system) : PluginImplBase(system)
{
    _parent->register_plugin(this);
}

MissionPlanImpl::~MissionPlanImpl()
{
    _parent->unregister_plugin(this);
}

void MissionPlanImpl::init() {}

void MissionPlanImpl::deinit() {}

void MissionPlanImpl::enable() {}

void MissionPlanImpl::disable() {}

void MissionPlanImpl::upload_mission_plan_async(
    std::string json_plan, const MissionPlan::ResultCallback callback)
{
    UNUSED(json_plan);

    UNUSED(callback);
}

MissionPlan::Result MissionPlanImpl::upload_mission_plan(std::string json_plan)
{
    UNUSED(json_plan);

    // TODO :)
    return {};
}

MissionPlan::Result MissionPlanImpl::cancel_mission_plan_upload()
{
    // TODO :)
    return {};
}

void MissionPlanImpl::clear_mission_plan_async(const MissionPlan::ResultCallback callback)
{
    UNUSED(callback);
}

MissionPlan::Result MissionPlanImpl::clear_mission_plan()
{
    // TODO :)
    return {};
}

void MissionPlanImpl::start_mission_async(const MissionPlan::ResultCallback callback)
{
    UNUSED(callback);
}

MissionPlan::Result MissionPlanImpl::start_mission()
{
    // TODO :)
    return {};
}

void MissionPlanImpl::pause_mission_async(const MissionPlan::ResultCallback callback)
{
    UNUSED(callback);
}

MissionPlan::Result MissionPlanImpl::pause_mission()
{
    // TODO :)
    return {};
}

void MissionPlanImpl::set_current_mission_item_async(
    int32_t index, const MissionPlan::ResultCallback callback)
{
    UNUSED(index);

    UNUSED(callback);
}

MissionPlan::Result MissionPlanImpl::set_current_mission_item(int32_t index)
{
    UNUSED(index);

    // TODO :)
    return {};
}

std::pair<MissionPlan::Result, bool> MissionPlanImpl::is_mission_finished()
{
    // TODO :)
    return {};
}

void MissionPlanImpl::subscribe_mission_progress(MissionPlan::MissionProgressCallback callback)
{
    UNUSED(callback);
}

MissionPlan::MissionProgress MissionPlanImpl::mission_progress() const
{
    return {};
}

} // namespace mavsdk