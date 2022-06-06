#include <gmock/gmock.h>
#include <memory>
#include <vector>

#include "plugins/mission/mission.h"

namespace mavsdk {
namespace testing {

class MockMission {
public:
    MOCK_CONST_METHOD2(
        upload_mission_async, void(Mission::MissionPlan&, Mission::ResultCallback)){};
    MOCK_CONST_METHOD1(upload_mission, Mission::Result(Mission::MissionPlan)){};
    MOCK_CONST_METHOD2(
        upload_mission_with_progress_async,
        void(Mission::MissionPlan, Mission::UploadMissionWithProgressCallback)){};
    MOCK_CONST_METHOD1(download_mission_async, void(Mission::DownloadMissionCallback)){};
    MOCK_CONST_METHOD0(download_mission, std::pair<Mission::Result, Mission::MissionPlan>()){};
    MOCK_CONST_METHOD1(
        download_mission_with_progress_async, void(Mission::DownloadMissionWithProgressCallback)){};
    MOCK_CONST_METHOD0(cancel_mission_upload, Mission::Result()){};
    MOCK_CONST_METHOD0(cancel_mission_download, Mission::Result()){};
    MOCK_CONST_METHOD1(start_mission_async, void(Mission::ResultCallback)){};
    MOCK_CONST_METHOD0(start_mission, Mission::Result()){};
    MOCK_CONST_METHOD1(pause_mission_async, void(Mission::ResultCallback)){};
    MOCK_CONST_METHOD0(pause_mission, Mission::Result()){};
    MOCK_CONST_METHOD1(clear_mission_async, void(Mission::ResultCallback)){};
    MOCK_CONST_METHOD0(clear_mission, Mission::Result()){};
    MOCK_CONST_METHOD2(
        set_current_mission_item_async, void(int current, Mission::ResultCallback)){};
    MOCK_CONST_METHOD1(set_current_mission_item, Mission::Result(int current)){};
    MOCK_CONST_METHOD0(current_mission_item, int()){};
    MOCK_CONST_METHOD0(total_mission_items, int()){};
    MOCK_CONST_METHOD0(is_mission_finished, std::pair<Mission::Result, bool>()){};
    MOCK_CONST_METHOD1(
        subscribe_mission_progress,
        Mission::MissionProgressHandle(Mission::MissionProgressCallback)){};
    MOCK_CONST_METHOD1(unsubscribe_mission_progress, void(Mission::MissionProgressHandle)){};
    MOCK_CONST_METHOD0(get_return_to_launch_after_mission, std::pair<Mission::Result, bool>()){};
    MOCK_CONST_METHOD1(set_return_to_launch_after_mission, Mission::Result(bool)){};

    MOCK_CONST_METHOD1(
        import_qgroundcontrol_mission,
        std::pair<Mission::Result, Mission::MissionPlan>(std::string)){};
};

} // namespace testing
} // namespace mavsdk
