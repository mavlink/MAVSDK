#include <future>

#include "mission/mission.grpc.pb.h"

namespace dronecore {
namespace backend {

template <typename Mission>
class MissionServiceImpl final : public dronecore::rpc::mission::MissionService::Service
{
public:
    MissionServiceImpl(Mission &mission)
        : _mission(mission) {}

private:
    Mission &_mission;
};

} // namespace backend
} // namespace dronecore
