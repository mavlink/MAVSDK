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

    // Let's get the mode first
    camera->get_mode_async([prom](Camera::Result result, Camera::Mode mode) {
        PromiseResult pr {};
        pr.result = result;
        pr.mode = mode;
        prom->set_value(pr);
    });

    // Block now for a while to wait for result.
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
    // FIXME: this should not be required.
    std::this_thread::sleep_for(std::chrono::seconds(1));

    auto prom = std::make_shared<std::promise<void>>();
    auto ret = prom->get_future();

    // Let's get the mode first
    camera->set_mode_async(mode, [mode, prom](Camera::Result result,
    Camera::Mode mode_got) {
        EXPECT_EQ(result, Camera::Result::SUCCESS);
        EXPECT_EQ(mode, mode_got);
        prom->set_value();
    });

    // Block now for a while to wait for result.
    auto status = ret.wait_for(std::chrono::seconds(10));

    EXPECT_EQ(status, std::future_status::ready);

    if (status == std::future_status::ready) {
        ret.get();
    }
}
