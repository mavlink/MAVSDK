#include "camera_definition.h"
#include "log.h"
#include <gtest/gtest.h>
#include <vector>
#include <map>
#include <memory>
#include <fstream>

using namespace dronecore;

static const char *e90_unit_test_file = "plugins/camera/e90_unit_test.xml";

TEST(CameraDefinition, LoadE90InfoFile)
{
    CameraDefinition cd;
    cd.load_file(e90_unit_test_file);
    EXPECT_STREQ(cd.get_vendor(), "Yuneec");
    EXPECT_STREQ(cd.get_model(), "E90");
}

TEST(CameraDefinition, LoadE90InfoString)
{
    std::ifstream file_stream(e90_unit_test_file);
    ASSERT_TRUE(file_stream.is_open());

    // Read whole file into string
    std::string content;
    std::getline(file_stream, content, '\0');

    CameraDefinition cd;
    cd.load_string(content);
    EXPECT_STREQ(cd.get_vendor(), "Yuneec");
    EXPECT_STREQ(cd.get_model(), "E90");
}

TEST(CameraDefinition, LoadE90All)
{
    // Run this from root.
    CameraDefinition cd;
    cd.load_file(e90_unit_test_file);

    CameraDefinition::parameter_map_t parameters;
    EXPECT_TRUE(cd.get_parameters(parameters, false));

    //auto before = steady_time();
#if 0
    for (auto &parameter : parameters) {
        LogDebug() << parameter.first << " => " << parameter.second->description;
        for (auto &option : parameter.second->options) {
            LogDebug() << option->name << ": "
                       << ((option->value.type == CameraDefinition::ParameterValue::Type::FLOAT) ?
                           option->value.value.as_float : option->value.value.as_uint32);
        }
    }

    EXPECT_EQ(parameters.size(), 12);
    EXPECT_EQ(parameters["CAM_WBMODE"]->options.size(), 7);
#endif
    //LogDebug() << "Elapsed: " << elapsed_since_s(before) << " s";
}


TEST(CameraDefinition, LoadE90Exclude)
{
    // Run this from root.
    CameraDefinition cd;
    cd.load_file(e90_unit_test_file);

#if 0
    CameraDefinition::ParameterValue value;
    value.value.as_uint32 = 1;
    cd.update_setting("CAM_MODE", value);

    CameraDefinition::parameter_map_t parameters;
    EXPECT_TRUE(cd.get_parameters(parameters, true));

    for (auto &parameter : parameters) {
        LogDebug() << parameter.first << " => " << parameter.second->description;
        for (auto &option : parameter.second->options) {
            LogDebug() << option->name << ": "
                       << ((option->value.type == CameraDefinition::ParameterValue::Type::FLOAT) ?
                           option->value.value.as_float : option->value.value.as_uint32);
        }
    }

    // In video mode, this param shoud not be available.
    EXPECT_FALSE(parameters.find("CAM_PHOTOFMT") != parameters.end());
#endif
}
