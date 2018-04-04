#include <gmock/gmock.h>
#include <utility>
#include <vector>

#include "mission/mission_service_impl.h"
#include "mission/mocks/mission_mock.h"

namespace {

using testing::NiceMock;
using testing::Return;

using MockMission = NiceMock<dronecore::testing::MockMission>;
using MissionServiceImpl = dronecore::backend::MissionServiceImpl<MockMission>;

using MissionResult = dronecore::rpc::mission::MissionResult;
using InputPair = std::pair<std::string, dronecore::Mission::Result>;

std::vector<InputPair> generateInputPairs();

class MissionServiceImplTest : public ::testing::TestWithParam<InputPair> {};

TEST_P(MissionServiceImplTest, uploadResultIsTranslatedCorrectly)
{
    // TODO
}

INSTANTIATE_TEST_CASE_P(MissionResultCorrespondences,
                        MissionServiceImplTest,
                        ::testing::ValuesIn(generateInputPairs()));

std::vector<InputPair> generateInputPairs()
{
    std::vector<InputPair> input_pairs;
    input_pairs.push_back(std::make_pair("UNKNOWN", dronecore::Mission::Result::UNKNOWN));
    input_pairs.push_back(std::make_pair("SUCCESS", dronecore::Mission::Result::SUCCESS));
    input_pairs.push_back(std::make_pair("ERROR", dronecore::Mission::Result::ERROR));
    input_pairs.push_back(std::make_pair("TOO_MANY_MISSION_ITEMS",
                                         dronecore::Mission::Result::TOO_MANY_MISSION_ITEMS));
    input_pairs.push_back(std::make_pair("BUSY", dronecore::Mission::Result::BUSY));
    input_pairs.push_back(std::make_pair("TIMEOUT", dronecore::Mission::Result::TIMEOUT));
    input_pairs.push_back(std::make_pair("INVALID_ARGUMENT",
                                         dronecore::Mission::Result::INVALID_ARGUMENT));
    input_pairs.push_back(std::make_pair("UNSUPPORTED", dronecore::Mission::Result::UNSUPPORTED));
    input_pairs.push_back(std::make_pair("NO_MISSION_AVAILABLE",
                                         dronecore::Mission::Result::NO_MISSION_AVAILABLE));
    input_pairs.push_back(std::make_pair("FAILED_TO_OPEN_QGC_PLAN",
                                         dronecore::Mission::Result::FAILED_TO_OPEN_QGC_PLAN));
    input_pairs.push_back(std::make_pair("FAILED_TO_PARSE_QGC_PLAN",
                                         dronecore::Mission::Result::FAILED_TO_PARSE_QGC_PLAN));
    input_pairs.push_back(std::make_pair("UNSUPPORTED_MISSION_CMD",
                                         dronecore::Mission::Result::UNSUPPORTED_MISSION_CMD));

    return input_pairs;
}

} // namespace
