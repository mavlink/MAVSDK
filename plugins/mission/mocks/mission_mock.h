#include <gmock/gmock.h>
#include <memory>
#include <vector>

#include "plugins/mission/mission.h"
#include "plugins/mission/mission_item.h"

namespace dronecode_sdk {
namespace testing {

class MockMission {
public:
    MOCK_CONST_METHOD2(upload_mission_async,
                       void(const std::vector<std::shared_ptr<MissionItem>> &,
                            Mission::result_callback_t)){};
    MOCK_CONST_METHOD1(download_mission_async,
                       void(Mission::mission_items_and_result_callback_t)){};
    MOCK_CONST_METHOD1(start_mission_async, void(Mission::result_callback_t)){};
    MOCK_CONST_METHOD1(pause_mission_async, void(Mission::result_callback_t)){};
    MOCK_CONST_METHOD2(set_current_mission_item_async,
                       void(int current, Mission::result_callback_t)){};
    MOCK_CONST_METHOD0(current_mission_item, int()){};
    MOCK_CONST_METHOD0(total_mission_items, int()){};
    MOCK_CONST_METHOD0(mission_finished, bool()){};
    MOCK_CONST_METHOD1(subscribe_progress, void(Mission::progress_callback_t)){};
    MOCK_CONST_METHOD0(get_return_to_launch_after_mission, bool()){};
    MOCK_CONST_METHOD1(set_return_to_launch_after_mission, void(bool)){};
};

} // namespace testing
} // namespace dronecode_sdk
