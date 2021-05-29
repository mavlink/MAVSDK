#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

/**
 * @brief The FakeLocationProvider class
 * This class provides periodic reports on the fake location of the system.
 */
class FakeLocationProvider {
public:
    typedef std::function<void(double lat, double lon)> location_callback_t;

    FakeLocationProvider();

    ~FakeLocationProvider();

    void request_location_updates(location_callback_t callback);
    bool is_running() { return !should_exit_; };

private:
    void start();
    void stop();
    void compute_locations();

    std::unique_ptr<std::thread> thread_{};
    std::atomic<bool> should_exit_{false};

    location_callback_t location_callback_ = nullptr;
    double latitude_deg_ = 47.3977419;
    double longitude_deg_ = 8.5455938;
    size_t count_ = 0u;

    static const size_t MAX_LOCATIONS;
    static const double LATITUDE_DEG_PER_METER;
    static const double LONGITUDE_DEG_PER_METER;
};
