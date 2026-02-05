#include "mavsdk.h"
#include "http_loader.h"
#include "curl_include.h"
#include "curl_wrapper.h"
#include "curl_wrapper_types.h"
#include "unused.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <thread>
#include <future>
#include <atomic>
#include <gtest/gtest.h>

using namespace mavsdk;

class HttpLoaderTest : public testing::Test {
protected:
    const std::string _file_url_1 = "http://subdomain.domain.com/some_folder/some_file";
    const std::string _file_url_2 = "http://subdomain.domain.com/some_folder/another_file.txt";
    const std::string _file_url_3 = "http://subdomain.domain.com/some_folder/yet_another_file.mp4";
    const std::string _file_local_path_1 = "some_file";
    const std::string _file_local_path_2 = "another_file.txt";
    const std::string _file_local_path_3 = "yet_another_file.mp4";

    virtual void SetUp() { clean(); }

    virtual void TearDown() { clean(); }

    void clean()
    {
        remove(_file_local_path_1.c_str());
        remove(_file_local_path_2.c_str());
        remove(_file_local_path_3.c_str());
    }

    bool check_file_exists(const std::string& file_path)
    {
        std::ifstream infile(file_path.c_str());
        return infile.good();
    }

    void write_file(const std::string& path, const std::string& content)
    {
        std::ofstream file;
        file.open(path);
        file << content;
        file.close();
    }

    void expect_all_simulated_downloads_to_succeed(CurlWrapperMock* curl_wrapper)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(_, _, _))
            .WillRepeatedly(Invoke([&](const std::string& /*url*/,
                                       const std::string& path,
                                       const ProgressCallback& progress_callback) {
                for (size_t i = 0; i <= 100; i++) {
                    if (progress_callback != nullptr) {
                        progress_callback(i, HttpStatus::Downloading, CURLcode::CURLE_OK);
                    }
                }

                write_file(path, "downloaded file content\n");

                std::this_thread::sleep_for(std::chrono::milliseconds(20));

                progress_callback(100, HttpStatus::Finished, CURLcode::CURLE_OK);
                return true;
            }));
    }

    void expect_one_simulated_download_to_fail(
        CurlWrapperMock* curl_wrapper, const std::string& url, const std::string& path)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _))
            .WillOnce(Invoke([&](const std::string& /*url*/,
                                 const std::string& /*path*/,
                                 const ProgressCallback& progress_callback) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                progress_callback(0, HttpStatus::Error, CURLcode::CURLE_COULDNT_RESOLVE_HOST);
                return false;
            }));
    }

    void expect_one_simulated_download_to_succeed(
        CurlWrapperMock* curl_wrapper, const std::string& url, const std::string& path)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _))
            .WillOnce(Invoke([&](const std::string& /*url*/,
                                 const std::string& got_path,
                                 const ProgressCallback& progress_callback) {
                for (size_t i = 0; i <= 100; i++) {
                    if (progress_callback != nullptr) {
                        progress_callback(i, HttpStatus::Downloading, CURLcode::CURLE_OK);
                    }
                }

                write_file(got_path, "downloaded file content\n");

                std::this_thread::sleep_for(std::chrono::milliseconds(20));

                progress_callback(100, HttpStatus::Finished, CURLcode::CURLE_OK);
                return true;
            }));
    }
};

TEST_F(HttpLoaderTest, HttpLoader_DownloadAsync_OneBad)
{
    auto curl_wrapper_mock = std::make_unique<CurlWrapperMock>();
    auto* curl_wrapper_raw = curl_wrapper_mock.get();
    auto http_loader = std::make_shared<HttpLoader>(std::move(curl_wrapper_mock));

    expect_one_simulated_download_to_succeed(curl_wrapper_raw, _file_url_1, _file_local_path_1);
    expect_one_simulated_download_to_fail(curl_wrapper_raw, _file_url_2, _file_local_path_2);
    expect_one_simulated_download_to_succeed(curl_wrapper_raw, _file_url_3, _file_local_path_3);

    std::atomic<int> callback_finished_counter{0};
    std::atomic<int> callback_error_counter{0};
    std::atomic<int> total_progress_calls{0};
    std::promise<void> completion_promise;
    auto completion_future = completion_promise.get_future();

    ProgressCallback progress =
        [&callback_finished_counter,
         &callback_error_counter,
         &total_progress_calls,
         &completion_promise](int got_progress, HttpStatus status, CURLcode curl_code) -> int {
        UNUSED(got_progress);

        if (status == HttpStatus::Downloading) {
            total_progress_calls++;
        } else if (
            status == HttpStatus::Error && curl_code == CURLcode::CURLE_COULDNT_RESOLVE_HOST) {
            callback_error_counter++;
        } else if (status == HttpStatus::Finished && curl_code == CURLcode::CURLE_OK) {
            callback_finished_counter++;
        }

        // Signal completion when we have 2 finished + 1 error
        if ((callback_finished_counter + callback_error_counter) >= 3) {
            completion_promise.set_value();
        }

        return 0;
    };

    http_loader->download_async(_file_url_1, _file_local_path_1, progress);
    http_loader->download_async(_file_url_2, _file_local_path_2, progress);
    http_loader->download_async(_file_url_3, _file_local_path_3, progress);

    // Wait for completion using future with timeout
    ASSERT_EQ(
        completion_future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);

    EXPECT_EQ(check_file_exists(_file_local_path_1), true);
    EXPECT_EQ(check_file_exists(_file_local_path_2), false);
    EXPECT_EQ(check_file_exists(_file_local_path_3), true);

    // We expect 2 successful downloads with 101 progress calls each = 202 total
    EXPECT_EQ(total_progress_calls.load(), 2 * 101);
    EXPECT_EQ(callback_finished_counter.load(), 2);
    EXPECT_EQ(callback_error_counter.load(), 1);

    clean();
}

TEST_F(HttpLoaderTest, HttpLoader_DownloadAsync_AllGood)
{
    auto curl_wrapper_mock = std::make_unique<CurlWrapperMock>();
    auto* curl_wrapper_raw = curl_wrapper_mock.get();
    auto http_loader = std::make_shared<HttpLoader>(std::move(curl_wrapper_mock));

    expect_all_simulated_downloads_to_succeed(curl_wrapper_raw);

    std::atomic<int> callback_finished_counter{0};
    std::atomic<int> callback_error_counter{0};
    std::atomic<int> total_progress_calls{0};
    std::promise<void> completion_promise;
    auto completion_future = completion_promise.get_future();

    ProgressCallback progress =
        [&callback_finished_counter,
         &callback_error_counter,
         &total_progress_calls,
         &completion_promise](int got_progress, HttpStatus status, CURLcode curl_code) -> int {
        UNUSED(got_progress);

        if (status == HttpStatus::Downloading) {
            total_progress_calls++;
        } else if (
            status == HttpStatus::Error && curl_code == CURLcode::CURLE_COULDNT_RESOLVE_HOST) {
            callback_error_counter++;
        } else if (status == HttpStatus::Finished && curl_code == CURLcode::CURLE_OK) {
            callback_finished_counter++;
        }

        // Signal completion when we have 3 finished downloads
        if (callback_finished_counter >= 3) {
            completion_promise.set_value();
        }

        return 0;
    };

    http_loader->download_async(_file_url_1, _file_local_path_1, progress);
    http_loader->download_async(_file_url_2, _file_local_path_2, progress);
    http_loader->download_async(_file_url_3, _file_local_path_3, progress);

    // Wait for completion using future with timeout
    ASSERT_EQ(
        completion_future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);

    EXPECT_EQ(check_file_exists(_file_local_path_1), true);
    EXPECT_EQ(check_file_exists(_file_local_path_2), true);
    EXPECT_EQ(check_file_exists(_file_local_path_3), true);

    // We expect 3 successful downloads with 101 progress calls each = 303 total
    EXPECT_EQ(total_progress_calls.load(), 3 * 101);
    EXPECT_EQ(callback_finished_counter.load(), 3);
    EXPECT_EQ(callback_error_counter.load(), 0);

    clean();
}
