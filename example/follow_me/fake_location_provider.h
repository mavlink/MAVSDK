#pragma once

#include <functional>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * @brief The FakeLocationProvider class
 * This class provides periodic reports on the fake location of the device.
 */
class FakeLocationProvider
{
public:
    typedef std::function<void(double lat, double lon)> location_callback_t;

    FakeLocationProvider(boost::asio::io_context &io)
        : timer_(io, boost::posix_time::seconds(1))
    {
    }

    ~FakeLocationProvider()
    {
        std::cout << "FakeLocationProvider: Done" << std::endl;
    }

    void request_location_updates(location_callback_t callback);

private:

    void compute_next_location();

    boost::asio::deadline_timer timer_;
    location_callback_t location_callback_ = nullptr;
    double latitude_deg_ = 47.3977419;
    double longitude_deg_ = 8.5455938;
    size_t count_ = 0u;

    static const size_t MAX_LOCATIONS;
    static const double LATITUDE_DEG_PER_METER;
    static const double LONGITUDE_DEG_PER_METER;
};
