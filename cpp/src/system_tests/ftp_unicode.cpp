#include "log.hpp"
#include "mavsdk.hpp"
#include <algorithm>
#include <filesystem>
#include <gtest/gtest.h>
#include <chrono>
#include <future>
#include <optional>
#include <string>
#include <thread>
#include <vector>
#include "plugins/ftp/ftp.hpp"
#include "plugins/ftp_server/ftp_server.hpp"
#include "fs_helpers.hpp"

using namespace mavsdk;

static constexpr double reduced_timeout_s = 0.1;

// TODO: make this compatible for Windows using GetTempPath2

static const fs::path temp_dir_provided = "/tmp/mavsdk_systemtest_temp_data/provided";
static const fs::path temp_dir_to_upload = "/tmp/mavsdk_systemtest_temp_data/to_upload";
static const fs::path temp_dir_downloaded = "/tmp/mavsdk_systemtest_temp_data/downloaded";

// All of these are valid UTF-8 filenames: emojis (including one made up of
// several code points), non-Latin scripts, and combining accents.
static const std::string emoji_file = "🚁 flight-log 🛰️.bin";
static const std::string emoji_dir = "flights 🛫";
static const std::string japanese_file = "飛行記録.bin";
static const std::string cyrillic_dir = "полёт";
// Mixes a decomposed "u" plus combining diaeresis (NFD) with a precomposed "\u00f6"
// (NFC) to check that names are passed on byte for byte, without normalization.
static const std::string combining_file = "u\u0308ber-h\u00f6he.txt";

namespace {

// Both instances plus the plugins, so that each test doesn't have to repeat
// the whole setup.
class FtpTestSetup {
public:
    bool connect(const fs::path& root_dir)
    {
        _mavsdk_groundstation.set_timeout_s(reduced_timeout_s);
        _mavsdk_autopilot.set_timeout_s(reduced_timeout_s);

        if (_mavsdk_groundstation.add_any_connection("udpin://0.0.0.0:17000") !=
            ConnectionResult::Success) {
            return false;
        }

        if (_mavsdk_autopilot.add_any_connection("udpout://127.0.0.1:17000") !=
            ConnectionResult::Success) {
            return false;
        }

        _ftp_server.emplace(_mavsdk_autopilot.server_component());
        _ftp_server->set_root_dir(root_dir.string());

        auto maybe_system = _mavsdk_groundstation.first_autopilot(10.0);
        if (!maybe_system) {
            return false;
        }

        if (!maybe_system.value()->has_autopilot()) {
            return false;
        }

        _ftp.emplace(maybe_system.value());

        return true;
    }

    Ftp& ftp() { return _ftp.value(); }

private:
    Mavsdk _mavsdk_groundstation{Mavsdk::Configuration{ComponentType::GroundStation}};
    Mavsdk _mavsdk_autopilot{Mavsdk::Configuration{ComponentType::Autopilot}};
    std::optional<FtpServer> _ftp_server{};
    std::optional<Ftp> _ftp{};
};

Ftp::Result upload(Ftp& ftp, const std::string& local_file_path, const std::string& remote_dir)
{
    auto prom = std::promise<Ftp::Result>();
    auto fut = prom.get_future();
    ftp.upload_async(
        local_file_path, remote_dir, [&prom](Ftp::Result result, Ftp::ProgressData) {
            if (result != Ftp::Result::Next) {
                prom.set_value(result);
            }
        });

    if (fut.wait_for(std::chrono::seconds(5)) != std::future_status::ready) {
        return Ftp::Result::Timeout;
    }

    return fut.get();
}

Ftp::Result download(Ftp& ftp, const std::string& remote_file_path, const std::string& local_dir)
{
    auto prom = std::promise<Ftp::Result>();
    auto fut = prom.get_future();
    ftp.download_async(
        remote_file_path, local_dir, false, [&prom](Ftp::Result result, Ftp::ProgressData) {
            if (result != Ftp::Result::Next) {
                prom.set_value(result);
            }
        });

    if (fut.wait_for(std::chrono::seconds(5)) != std::future_status::ready) {
        return Ftp::Result::Timeout;
    }

    return fut.get();
}

} // namespace

TEST(FtpUnicode, UploadAndDownloadFile)
{
    ASSERT_TRUE(reset_directories(temp_dir_provided));
    ASSERT_TRUE(reset_directories(temp_dir_downloaded));
    ASSERT_TRUE(reset_directories(temp_dir_to_upload));
    ASSERT_TRUE(create_temp_file(temp_dir_to_upload / fs::path(emoji_file), 50));

    FtpTestSetup setup;
    ASSERT_TRUE(setup.connect(temp_dir_provided));
    auto& ftp = setup.ftp();

    // The remote directory has an emoji in it as well.
    EXPECT_EQ(ftp.create_directory(emoji_dir), Ftp::Result::Success);
    EXPECT_TRUE(file_exists(temp_dir_provided / fs::path(emoji_dir)));

    EXPECT_EQ(
        upload(ftp, (temp_dir_to_upload / fs::path(emoji_file)).string(), emoji_dir),
        Ftp::Result::Success);
    EXPECT_TRUE(file_exists(temp_dir_provided / fs::path(emoji_dir) / fs::path(emoji_file)));

    const auto remote_file_path = emoji_dir + "/" + emoji_file;

    // The CRC32 of the remote file should match the local one.
    EXPECT_EQ(
        ftp.are_files_identical((temp_dir_to_upload / fs::path(emoji_file)).string(),
                                remote_file_path),
        std::make_pair(Ftp::Result::Success, true));

    EXPECT_EQ(
        download(ftp, remote_file_path, temp_dir_downloaded.string()), Ftp::Result::Success);
    EXPECT_TRUE(are_files_identical(
        temp_dir_to_upload / fs::path(emoji_file), temp_dir_downloaded / fs::path(emoji_file)));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(FtpUnicode, ListDirectory)
{
    ASSERT_TRUE(reset_directories(temp_dir_provided));

    const std::vector<std::string> truth_files{emoji_file, japanese_file, combining_file};
    const std::vector<std::string> truth_dirs{emoji_dir, cyrillic_dir};

    for (const auto& file : truth_files) {
        ASSERT_TRUE(create_temp_file(temp_dir_provided / fs::path(file), 10));
    }
    for (const auto& dir : truth_dirs) {
        ASSERT_TRUE(reset_directories(temp_dir_provided / fs::path(dir)));
    }

    FtpTestSetup setup;
    ASSERT_TRUE(setup.connect(temp_dir_provided));
    auto& ftp = setup.ftp();

    auto ret = ftp.list_directory("./");
    ASSERT_EQ(ret.first, Ftp::Result::Success);

    std::vector<std::string> found_files;
    std::vector<std::string> found_dirs;
    for (const auto& entry : ret.second.entries) {
        if (entry.entry_type == Ftp::FilesystemEntry::EntryType::File) {
            found_files.push_back(entry.name);
            EXPECT_EQ(entry.size_bytes, 10u);
        } else if (entry.entry_type == Ftp::FilesystemEntry::EntryType::Directory) {
            found_dirs.push_back(entry.name);
        }
    }

    auto sorted = [](std::vector<std::string> v) {
        std::sort(v.begin(), v.end());
        return v;
    };

    EXPECT_EQ(sorted(found_files), sorted(truth_files));
    EXPECT_EQ(sorted(found_dirs), sorted(truth_dirs));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(FtpUnicode, RenameAndRemove)
{
    ASSERT_TRUE(reset_directories(temp_dir_provided));
    ASSERT_TRUE(create_temp_file(temp_dir_provided / fs::path("data.bin"), 50));

    FtpTestSetup setup;
    ASSERT_TRUE(setup.connect(temp_dir_provided));
    auto& ftp = setup.ftp();

    // Rename from ASCII to Unicode, and then from Unicode to Unicode.
    EXPECT_EQ(ftp.rename("data.bin", emoji_file), Ftp::Result::Success);
    EXPECT_TRUE(file_exists(temp_dir_provided / fs::path(emoji_file)));

    EXPECT_EQ(ftp.rename(emoji_file, japanese_file), Ftp::Result::Success);
    EXPECT_TRUE(file_exists(temp_dir_provided / fs::path(japanese_file)));
    EXPECT_FALSE(file_exists(temp_dir_provided / fs::path(emoji_file)));

    EXPECT_EQ(ftp.remove_file(japanese_file), Ftp::Result::Success);
    EXPECT_FALSE(file_exists(temp_dir_provided / fs::path(japanese_file)));

    EXPECT_EQ(ftp.create_directory(cyrillic_dir), Ftp::Result::Success);
    EXPECT_TRUE(file_exists(temp_dir_provided / fs::path(cyrillic_dir)));

    EXPECT_EQ(ftp.remove_directory(cyrillic_dir), Ftp::Result::Success);
    EXPECT_FALSE(file_exists(temp_dir_provided / fs::path(cyrillic_dir)));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(FtpUnicode, NameTooLongIsRejected)
{
    ASSERT_TRUE(reset_directories(temp_dir_provided));

    FtpTestSetup setup;
    ASSERT_TRUE(setup.connect(temp_dir_provided));
    auto& ftp = setup.ftp();

    // The payload can hold 239 bytes, and each of these emojis takes 4 bytes, so
    // this does not fit. It must be rejected rather than truncated in the middle
    // of a code point.
    std::string too_long;
    for (unsigned i = 0; i < 60; ++i) {
        too_long += "🚁";
    }
    ASSERT_GT(too_long.size(), 239u);

    EXPECT_EQ(ftp.create_directory(too_long), Ftp::Result::InvalidParameter);
    EXPECT_FALSE(file_exists(temp_dir_provided / fs::path(too_long)));

    // A name which just about fits should still work.
    std::string just_fits;
    for (unsigned i = 0; i < 59; ++i) {
        just_fits += "🚁";
    }
    ASSERT_LT(just_fits.size(), 239u);

    EXPECT_EQ(ftp.create_directory(just_fits), Ftp::Result::Success);
    EXPECT_TRUE(file_exists(temp_dir_provided / fs::path(just_fits)));

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST(FtpUnicode, ListDirectoryWithNameTooLongForPayload)
{
    ASSERT_TRUE(reset_directories(temp_dir_provided));

    // A name of 60 emojis is 240 bytes, which is fine for the filesystem but does
    // not fit into one FTP payload of 239 bytes. Such an entry has to be skipped,
    // and it must not hide the entries that come after it.
    std::string too_long_for_payload;
    for (unsigned i = 0; i < 60; ++i) {
        too_long_for_payload += "🚁";
    }

    ASSERT_TRUE(create_temp_file(temp_dir_provided / fs::path(too_long_for_payload), 10));
    ASSERT_TRUE(create_temp_file(temp_dir_provided / fs::path(japanese_file), 10));
    ASSERT_TRUE(create_temp_file(temp_dir_provided / fs::path(combining_file), 10));

    FtpTestSetup setup;
    ASSERT_TRUE(setup.connect(temp_dir_provided));
    auto& ftp = setup.ftp();

    auto ret = ftp.list_directory("./");
    ASSERT_EQ(ret.first, Ftp::Result::Success);

    std::vector<std::string> found_files;
    for (const auto& entry : ret.second.entries) {
        found_files.push_back(entry.name);
    }
    std::sort(found_files.begin(), found_files.end());

    std::vector<std::string> expected_files{japanese_file, combining_file};
    std::sort(expected_files.begin(), expected_files.end());

    EXPECT_EQ(found_files, expected_files);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
