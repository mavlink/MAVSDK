#include <memory>
#include <future>
#include "integration_test_helper.h"
#include "camera_test_helpers.h"

using namespace mavsdk;

Camera::Mode get_mode(std::shared_ptr<Camera> camera)
{
    auto prom = std::make_shared<std::promise<Camera::Mode>>();
    auto ret = prom->get_future();

    camera->mode_async([prom, camera](Camera::Mode mode) {
        prom->set_value(mode);
        camera->mode_async(nullptr);
    });

    auto status = ret.wait_for(std::chrono::seconds(7));

    EXPECT_EQ(status, std::future_status::ready);

    if (status == std::future_status::ready) {
        Camera::Mode mode = ret.get();
        EXPECT_NE(mode, Camera::Mode::Unknown);
        return mode;
    } else {
        return Camera::Mode::Unknown;
    }
}

void set_mode_async(std::shared_ptr<Camera> camera, Camera::Mode mode)
{
    //// FIXME: this should not be required.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto prom = std::make_shared<std::promise<void>>();
    auto ret = prom->get_future();

    camera->set_mode_async(mode, [mode, prom](Camera::Result result) {
        EXPECT_EQ(result, Camera::Result::Success);
        prom->set_value();
    });

    auto status = ret.wait_for(std::chrono::seconds(10));

    EXPECT_EQ(status, std::future_status::ready);

    if (status == std::future_status::ready) {
        ret.get();
    }

    // FIXME: this should not be required.
    std::this_thread::sleep_for(std::chrono::seconds(1));
}

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
