#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

/**
 * Class that provides periodic reports on the fake location of a system.
 */
class FakeLocationProvider {
public:
    using LocationCallback = std::function<void(double lat, double lon)>;

    FakeLocationProvider();

    ~FakeLocationProvider();

    void request_location_updates(LocationCallback callback);
    bool is_running() { return !should_exit_; };

private:
    void start();
    void stop();
    void compute_locations();

    std::unique_ptr<std::thread> thread_{};
    std::atomic<bool> should_exit_{false};

    LocationCallback location_callback_{nullptr};
    double latitude_deg_{47.3977419};
    double longitude_deg_{8.5455938};
    size_t count_{0};

    static constexpr double LATITUDE_DEG_PER_METER = 0.000009044;
    static constexpr double LONGITUDE_DEG_PER_METER = 0.000008985;
};
