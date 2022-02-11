#include <memory>
#include <future>
#include "integration_test_helper.h"
#include "camera_test_helpers.h"

using namespace mavsdk;

Camera::Result set_setting(
    std::shared_ptr<Camera> camera, const std::string& setting_id, const std::string& option_id)
{
    Camera::Setting new_setting{};
    new_setting.setting_id = setting_id;
    new_setting.option.option_id = option_id;
    return camera->set_setting(new_setting);
}

mavsdk::Camera::Result get_setting(
    std::shared_ptr<mavsdk::Camera> camera, const std::string& setting_id, std::string& option_id)
{
    Camera::Setting new_setting{};
    new_setting.setting_id = setting_id;
    auto result_pair = camera->get_setting(new_setting);
    option_id = result_pair.second.option.option_id;
    return result_pair.first;
}
