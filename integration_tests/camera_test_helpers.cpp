#include <memory>
#include <future>
#include "integration_test_helper.h"
#include "camera_test_helpers.h"

using namespace dronecore;

Camera::Mode get_mode(std::shared_ptr<Camera> camera)
{
    struct PromiseResult {
        Camera::Result result;
        Camera::Mode mode;
    };

    auto prom = std::make_shared<std::promise<PromiseResult>>();
    auto ret = prom->get_future();

    camera->get_mode_async([prom](Camera::Result result, Camera::Mode mode) {
        PromiseResult pr {};
        pr.result = result;
        pr.mode = mode;
        prom->set_value(pr);
    });

    auto status = ret.wait_for(std::chrono::seconds(7));

    EXPECT_EQ(status, std::future_status::ready);

    if (status == std::future_status::ready) {
        PromiseResult new_ret = ret.get();
        EXPECT_EQ(new_ret.result, Camera::Result::SUCCESS);
        EXPECT_NE(new_ret.mode, Camera::Mode::UNKNOWN);
        return new_ret.mode;
    } else {
        return Camera::Mode::UNKNOWN;
    }
}

void set_mode(std::shared_ptr<Camera> camera, Camera::Mode mode)
{
    Camera::Mode mode_already_set = get_mode(camera);

    // FIXME: this should not be required.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    if (mode == mode_already_set) {
        return;
    }

    auto prom = std::make_shared<std::promise<void>>();
    auto ret = prom->get_future();

    camera->set_mode_async(mode, [mode, prom](Camera::Result result,
    Camera::Mode mode_got) {
        EXPECT_EQ(result, Camera::Result::SUCCESS);
        EXPECT_EQ(mode, mode_got);
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


Camera::Result set_setting(std::shared_ptr<Camera> camera,
                           const std::string &setting,
                           const std::string &option)
{
    auto prom = std::make_shared<std::promise<Camera::Result>>();
    auto ret = prom->get_future();

    camera->set_option_async(setting, option,
    [prom](Camera::Result result) {
        prom->set_value(result);
    });

    auto status = ret.wait_for(std::chrono::seconds(1));

    EXPECT_EQ(status, std::future_status::ready);

    if (status == std::future_status::ready) {
        return ret.get();
    }
    return Camera::Result::TIMEOUT;
}

dronecore::Camera::Result get_setting(std::shared_ptr<dronecore::Camera> camera,
                                      const std::string &setting,
                                      std::string &option)
{
    struct PromiseResult {
        Camera::Result result;
        std::string value;
    };

    auto prom = std::make_shared<std::promise<PromiseResult>>();
    auto ret = prom->get_future();

    camera->get_option_async(setting,
    [prom](Camera::Result result, const std::string & value) {
        PromiseResult promise_result;
        promise_result.result = result;
        promise_result.value = value;
        prom->set_value(promise_result);
    });

    auto status = ret.wait_for(std::chrono::seconds(1));

    EXPECT_EQ(status, std::future_status::ready);

    if (status == std::future_status::ready) {
        PromiseResult promise_result = ret.get();
        option = promise_result.value;
        return promise_result.result;
    }
    return Camera::Result::TIMEOUT;

}
