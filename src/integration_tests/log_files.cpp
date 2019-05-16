#include <iostream>
#include <future>
#include <sstream>
#include "dronecode_sdk.h"
#include "integration_test_helper.h"
#include "plugins/log_files/log_files.h"

using namespace dronecode_sdk;

TEST(HardwareTest, LogFiles)
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

    unsigned num_downloaded_files = 0;

    if (entry_result.first == LogFiles::Result::SUCCESS) {
        for (auto &entry : entry_result.second) {
            float size_mib = entry.size_bytes / 1024.0f / 1024.0f;
            LogInfo() << "Entry " << entry.id << ": "
                      << " at " << entry.date << ", " << size_mib
                      << " MiB, bytes: " << entry.size_bytes;
            std::stringstream file_path_stream;
            file_path_stream << "/tmp/logfile_" << entry.id << ".ulog";
            LogFiles::Result download_ret =
                log_files->download_log_file(entry.id, file_path_stream.str());
            EXPECT_EQ(download_ret, LogFiles::Result::SUCCESS);
            if (++num_downloaded_files >= 2) {
                break;
            }
        }
    }
}
