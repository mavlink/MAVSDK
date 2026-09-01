#include "log.hpp"
#include "log_callback.hpp"

#include <atomic>
#include <chrono>
#include <string>
#include <thread>
#include <gtest/gtest.h>

using namespace mavsdk;

// emit_log() has to read the subscribed callback while another thread may be replacing it
// through log::subscribe(). Getting that wrong is not just a data race on the std::function:
// emit_log() used to fetch it twice, once to test it and once to call it, so a subscribe()
// landing in between could invoke an empty std::function -- which throws, and MAVSDK is built
// with -fno-exceptions.
//
// Both callbacks here suppress output, so the test does not also flood the log while it runs.
TEST(LogRace, SubscribeWhileLogging)
{
    std::atomic<bool> stop{false};

    std::thread logger([&stop]() {
        while (!stop) {
            LogDebug("racing the log callback");
        }
    });

    std::thread swapper([&stop]() {
        while (!stop) {
            log::subscribe(
                [](log::Level, const std::string&, const std::string&, int) { return true; });
            log::subscribe(
                [](log::Level, const std::string&, const std::string&, int) { return true; });
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    stop = true;
    logger.join();
    swapper.join();

    // Leave the default logging in place for everything that runs after this.
    log::subscribe({});
}
