
#include "fake_location_provider.h"
#include <chrono> // for seconds()
#include <thread> // for sleep_for()

using std::this_thread::sleep_for;
using std::chrono::seconds;

FakeLocationProvider::FakeLocationProvider() {}

FakeLocationProvider::~FakeLocationProvider()
{
    stop();
}

void FakeLocationProvider::request_location_updates(LocationCallback callback)
{
    location_callback_ = callback;
    stop();
    start();
}

void FakeLocationProvider::start()
{
    should_exit_ = false;
    thread_ = std::make_unique<std::thread>(&FakeLocationProvider::compute_locations, this);
}

void FakeLocationProvider::stop()
{
    should_exit_ = true;

    if (thread_) {
        thread_->join();
        thread_.reset();
    }
}

void FakeLocationProvider::compute_locations()
{
    // Draws a square.

    while (!should_exit_) {
        if (count_ < 10) {
            location_callback_(latitude_deg_, longitude_deg_);
            latitude_deg_ -= LATITUDE_DEG_PER_METER * 4;
        } else if (count_ < 20) {
            location_callback_(latitude_deg_, longitude_deg_);
            longitude_deg_ += LONGITUDE_DEG_PER_METER * 4;
        } else if (count_ < 30) {
            location_callback_(latitude_deg_, longitude_deg_);
            latitude_deg_ += LATITUDE_DEG_PER_METER * 4;
        } else if (count_ < 40) {
            location_callback_(latitude_deg_, longitude_deg_);
            longitude_deg_ -= LONGITUDE_DEG_PER_METER * 4;
        } else {
            // We're done.
            should_exit_ = true;
        }
        sleep_for(seconds(1));
        ++count_;
    }
}
