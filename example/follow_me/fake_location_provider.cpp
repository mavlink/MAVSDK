
#include "fake_location_provider.h"
#include <chrono> // for seonds()
#include <thread> // for sleep_for()

using std::this_thread::sleep_for;
using std::chrono::seconds;

void FakeLocationProvider::request_location_updates(location_callback_t callback)
{
    location_callback_ = callback;
    timer_.async_wait(std::bind(&FakeLocationProvider::compute_next_location, this));
}

// Rudimentary location provider whose successive lat, lon combination
// makes Drone revolve in a semi-circular path.
void FakeLocationProvider::compute_next_location()
{
    if (count_++ < 10) {
        location_callback_(latitude_deg_, longitude_deg_);
        latitude_deg_ -= LATITUDE_DEG_PER_METER * 4;
        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
        timer_.async_wait(std::bind(&FakeLocationProvider::compute_next_location, this));
        sleep_for(seconds(1));
    }
    if (count_++ < 20) {
        location_callback_(latitude_deg_, longitude_deg_);
        longitude_deg_ += LONGITUDE_DEG_PER_METER * 4;
        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
        timer_.async_wait(std::bind(&FakeLocationProvider::compute_next_location, this));
        sleep_for(seconds(1));
    }
    if (count_++ < 30) {
        location_callback_(latitude_deg_, longitude_deg_);
        latitude_deg_ += LATITUDE_DEG_PER_METER * 4;
        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
        timer_.async_wait(std::bind(&FakeLocationProvider::compute_next_location, this));
        sleep_for(seconds(1));
    }
    if (count_++ < 40) {
        location_callback_(latitude_deg_, longitude_deg_);
        longitude_deg_ -= LONGITUDE_DEG_PER_METER * 4;
        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
        timer_.async_wait(std::bind(&FakeLocationProvider::compute_next_location, this));
        sleep_for(seconds(1));
    }
    if (count_++ < 50) {
        location_callback_(latitude_deg_, longitude_deg_);
        latitude_deg_ -= LATITUDE_DEG_PER_METER * 3;
        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
        timer_.async_wait(std::bind(&FakeLocationProvider::compute_next_location, this));
        sleep_for(seconds(1));
    }
    if (count_++ < MAX_LOCATIONS) {
        location_callback_(latitude_deg_, longitude_deg_);
        longitude_deg_ += LONGITUDE_DEG_PER_METER * 3;
        timer_.expires_at(timer_.expires_at() + boost::posix_time::seconds(1));
        timer_.async_wait(std::bind(&FakeLocationProvider::compute_next_location, this));
        sleep_for(seconds(1));
    }
}

const size_t FakeLocationProvider::MAX_LOCATIONS = 60u;
const double FakeLocationProvider::LATITUDE_DEG_PER_METER = 0.000009044;
const double FakeLocationProvider::LONGITUDE_DEG_PER_METER = 0.000008985;
