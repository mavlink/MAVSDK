#include "integration_test_helper.h"
#include "mavsdk.h"
#include <iostream>
#include <functional>
#include <atomic>
#include "plugins/camera/camera.h"

using namespace mavsdk;

TEST(CameraTest, ListPhotos)
{
    Mavsdk mavsdk;

    ConnectionResult ret = mavsdk.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::Success);

    // Wait for system to connect via heartbeat.
    std::this_thread::sleep_for(std::chrono::seconds(2));

    ASSERT_EQ(mavsdk.systems().size(), 1);

    auto system = mavsdk.systems().at(0);
    auto camera = Camera{system};

    camera.prepare();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    const auto result_since = camera.list_photos(Camera::PhotosRange::SinceConnection);
    ASSERT_EQ(result_since.first, Camera::Result::Success);

    std::cout << "Found " << result_since.second.size() << " photos.";

    auto result_all = camera.list_photos(Camera::PhotosRange::All);
    ASSERT_EQ(result_all.first, Camera::Result::Success);

    std::cout << "Found " << result_all.second.size() << " overall photos.";
}
