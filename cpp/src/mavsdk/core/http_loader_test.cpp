#include "mavsdk.hpp"
#include "http_loader.hpp"
#include "curl_include.hpp"
#include "curl_wrapper.hpp"
#include "curl_wrapper_types.hpp"
#include "unused.hpp"
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

// Runs callbacks straight on the HttpLoader work thread.
static void run_inline(const std::function<void()>& task, bool /*wait*/)
{
    task();
}

class HttpLoaderFixture : public testing::Test {
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
        EXPECT_CALL(*curl_wrapper, download_file_to_path(_, _, _, _))
            .WillRepeatedly(Invoke([&](const std::string& /*url*/,
                                       const std::string& path,
                                       const ProgressCallback& progress_callback,
                                       curl_off_t /*max_download_size_bytes*/) {
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
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _, _))
            .WillOnce(Invoke([&](const std::string& /*url*/,
                                 const std::string& /*path*/,
                                 const ProgressCallback& progress_callback,
                                 curl_off_t /*max_download_size_bytes*/) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                progress_callback(0, HttpStatus::Error, CURLcode::CURLE_COULDNT_RESOLVE_HOST);
                return false;
            }));
    }

    void expect_one_simulated_download_to_succeed(
        CurlWrapperMock* curl_wrapper, const std::string& url, const std::string& path)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _, _))
            .WillOnce(Invoke([&](const std::string& /*url*/,
                                 const std::string& got_path,
                                 const ProgressCallback& progress_callback,
                                 curl_off_t /*max_download_size_bytes*/) {
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

TEST_F(HttpLoaderFixture, DownloadSync_ForwardsSizeLimit)
{
    constexpr curl_off_t max_download_size_bytes = 4096;
    auto curl_wrapper_mock = std::make_unique<CurlWrapperMock>();
    auto* curl_wrapper_raw = curl_wrapper_mock.get();
    auto http_loader = std::make_shared<HttpLoader>(std::move(curl_wrapper_mock), run_inline);

    EXPECT_CALL(
        *curl_wrapper_raw,
        download_file_to_path(_file_url_1, _file_local_path_1, _, max_download_size_bytes))
        .WillOnce(Return(true));

    EXPECT_TRUE(
        http_loader->download_sync(_file_url_1, _file_local_path_1, max_download_size_bytes));
}

TEST_F(HttpLoaderFixture, DownloadAsync_OneBad)
{
    auto curl_wrapper_mock = std::make_unique<CurlWrapperMock>();
    auto* curl_wrapper_raw = curl_wrapper_mock.get();
    auto http_loader = std::make_shared<HttpLoader>(std::move(curl_wrapper_mock), run_inline);

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

TEST_F(HttpLoaderFixture, DownloadAsync_AllGood)
{
    auto curl_wrapper_mock = std::make_unique<CurlWrapperMock>();
    auto* curl_wrapper_raw = curl_wrapper_mock.get();
    auto http_loader = std::make_shared<HttpLoader>(std::move(curl_wrapper_mock), run_inline);

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

TEST_F(HttpLoaderFixture, DownloadAsync_QueueFromCallback)
{
    auto curl_wrapper_mock = std::make_unique<CurlWrapperMock>();
    auto* curl_wrapper_raw = curl_wrapper_mock.get();
    auto http_loader = std::make_shared<HttpLoader>(std::move(curl_wrapper_mock), run_inline);

    expect_all_simulated_downloads_to_succeed(curl_wrapper_raw);

    std::promise<void> completion_promise;
    auto completion_future = completion_promise.get_future();

    ProgressCallback second_progress =
        [&completion_promise](int /*progress*/, HttpStatus status, CURLcode /*curl_code*/) {
            if (status == HttpStatus::Finished) {
                completion_promise.set_value();
            }
            return 0;
        };

    // Queueing the next download from a callback must not deadlock: the callback runs on
    // the work thread, which must not hold the queue lock while it does.
    ProgressCallback first_progress =
        [&](int /*progress*/, HttpStatus status, CURLcode /*curl_code*/) {
            if (status == HttpStatus::Finished) {
                http_loader->download_async(_file_url_2, _file_local_path_2, second_progress);
            }
            return 0;
        };

    http_loader->download_async(_file_url_1, _file_local_path_1, first_progress);

    if (completion_future.wait_for(std::chrono::milliseconds(1000)) != std::future_status::ready) {
        // The work thread is stuck and can't be joined, so leak the loader rather than
        // hang the test run in its destructor.
        (void)new std::shared_ptr<HttpLoader>(http_loader);
        FAIL() << "Queueing a download from a callback deadlocked";
    }

    EXPECT_EQ(check_file_exists(_file_local_path_1), true);
    EXPECT_EQ(check_file_exists(_file_local_path_2), true);

    clean();
}

TEST_F(HttpLoaderFixture, DownloadAsync_CallbacksGoThroughExecutor)
{
    std::mutex tasks_mutex;
    std::vector<std::function<void()>> tasks;
    auto queue_task = [&](const std::function<void()>& task, bool wait) {
        if (wait) {
            task();
            return;
        }
        std::lock_guard<std::mutex> lock(tasks_mutex);
        tasks.push_back(task);
    };
    auto run_queued_tasks = [&]() {
        std::vector<std::function<void()>> to_run;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            to_run.swap(tasks);
        }
        for (auto& task : to_run) {
            task();
        }
    };

    auto curl_wrapper_mock = std::make_unique<CurlWrapperMock>();
    auto* curl_wrapper_raw = curl_wrapper_mock.get();
    auto http_loader = std::make_unique<HttpLoader>(std::move(curl_wrapper_mock), queue_task);

    std::promise<void> download_done_promise;
    auto download_done_future = download_done_promise.get_future();

    EXPECT_CALL(*curl_wrapper_raw, download_file_to_path(_file_url_1, _file_local_path_1, _, _))
        .WillOnce(Invoke([&](const std::string& /*url*/,
                             const std::string& /*path*/,
                             const ProgressCallback& progress_callback,
                             curl_off_t /*max_download_size_bytes*/) {
            progress_callback(100, HttpStatus::Finished, CURLcode::CURLE_OK);
            download_done_promise.set_value();
            return true;
        }));
    EXPECT_CALL(*curl_wrapper_raw, download_text(_file_url_2, _)).WillOnce(Return(true));

    std::atomic<int> finished_counter{0};
    std::atomic<int> text_counter{0};

    http_loader->download_async(
        _file_url_1,
        _file_local_path_1,
        [&](int /*progress*/, HttpStatus status, CURLcode /*curl_code*/) {
            if (status == HttpStatus::Finished) {
                finished_counter++;
            }
        });

    ASSERT_EQ(
        download_done_future.wait_for(std::chrono::milliseconds(1000)), std::future_status::ready);

    // The download has finished, but its callback only runs once the executor runs it.
    EXPECT_EQ(finished_counter.load(), 0);
    run_queued_tasks();
    EXPECT_EQ(finished_counter.load(), 1);

    http_loader->download_text_async(
        _file_url_2, [&](bool /*success*/, const std::string& /*content*/) { text_counter++; });

    // Wait until the text callback has been handed to the executor.
    bool text_callback_queued = false;
    for (int i = 0; i < 100 && !text_callback_queued; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::lock_guard<std::mutex> lock(tasks_mutex);
        text_callback_queued = !tasks.empty();
    }
    ASSERT_TRUE(text_callback_queued);

    // A callback that is still queued when the loader goes away must not run.
    http_loader.reset();
    run_queued_tasks();
    EXPECT_EQ(text_counter.load(), 0);
}
