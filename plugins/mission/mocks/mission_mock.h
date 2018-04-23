#include <gmock/gmock.h>
#include <memory>
#include <vector>

#include "mission/mission.h"
#include "mission/mission_item.h"

namespace dronecore {
namespace testing {

class MockMission
{
public:
    MOCK_CONST_METHOD2(upload_mission_async, void(const std::vector<std::shared_ptr<MissionItem>> &,
                                                  Mission::result_callback_t));
    MOCK_CONST_METHOD1(start_mission_async, void(Mission::result_callback_t));
};

} // namespace testing
} // namespace dronecore
