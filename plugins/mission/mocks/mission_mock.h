#include <gmock/gmock.h>
#include <memory>
#include <vector>

#include "mission/mission.h" // TODO: remove this dependency by moving the datastructs out of Mission
#include "mission/mission_item.h"

namespace dronecore {
namespace testing {

typedef std::function<void(Mission::Result)> mission_result_callback_t;

class MockMission
{
public:
    MOCK_CONST_METHOD2(upload_mission_async, void(const std::vector<std::shared_ptr<MissionItem>> &,
                                                  mission_result_callback_t));
    MOCK_CONST_METHOD1(start_mission_async, void(mission_result_callback_t));
};

} // namespace testing
} // namespace dronecore
