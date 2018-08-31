#include <iostream>
#include <future>
#include "dronecode_sdk.h"
#include "integration_test_helper.h"
#include "plugins/log_files/log_files.h"

using namespace dronecode_sdk;

static void download_log_file(std::shared_ptr<LogFiles> log_files, unsigned id);

TEST_F(SitlTest, LogFiles)
{
    DronecodeSDK dc;

    // ConnectionResult ret = dc.add_serial_connection("/dev/ttyACM0");
    ConnectionResult ret = dc.add_udp_connection();
    ASSERT_EQ(ret, ConnectionResult::SUCCESS);

    std::this_thread::sleep_for(std::chrono::seconds(2));

    System &system = dc.system();
    ASSERT_TRUE(system.has_autopilot());
    auto log_files = std::make_shared<LogFiles>(system);

    std::pair<LogFiles::Result, std::vector<LogFiles::Entry>> entry_result =
        log_files->get_entries();

    EXPECT_EQ(entry_result.first, LogFiles::Result::SUCCESS);

    if (entry_result.first == LogFiles::Result::SUCCESS) {
        for (auto &entry : entry_result.second) {
            float size_mib = entry.size_bytes / 1024.0f / 1024.0f;
            LogInfo() << "Entry " << entry.id << ": " << size_mib
                      << " MiB, bytes: " << entry.size_bytes;
            if (size_mib > 1.0f) {
                download_log_file(log_files, entry.id);
            }
        }
    }
}

void download_log_file(std::shared_ptr<LogFiles> log_files, unsigned id)
{
    auto prom = std::make_shared<std::promise<void>>();
    auto future_result = prom->get_future();

    log_files->download_log_file_async(id, [prom](LogFiles::Result result, float progress) {
        if (result == LogFiles::Result::PROGRESS) {
            LogDebug() << "Progress: " << 100.0f * progress << " %";
        } else {
            EXPECT_EQ(result, LogFiles::Result::SUCCESS);
            prom->set_value();
        }
    });

    future_result.wait();
}
