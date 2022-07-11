#include "mavsdk.h"
#include "http_loader.h"
#include "curl_include.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
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

    void
    expect_all_simulated_downloads_to_succeed(const std::shared_ptr<CurlWrapperMock>& curl_wrapper)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(_, _, _))
            .WillRepeatedly(Invoke([&](const std::string& /*url*/,
                                       const std::string& path,
                                       const ProgressCallback& progress_callback) {
                for (size_t i = 0; i <= 100; i++) {
                    if (progress_callback != nullptr) {
                        progress_callback(i, Status::Downloading, CURLcode::CURLE_OK);
                    }
                }

                write_file(path, "downloaded file content\n");

                std::this_thread::sleep_for(std::chrono::milliseconds(20));

                progress_callback(100, Status::Finished, CURLcode::CURLE_OK);
                return true;
            }));
    }

    void expect_one_simulated_download_to_fail(
        const std::shared_ptr<CurlWrapperMock>& curl_wrapper,
        const std::string& url,
        const std::string& path)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _))
            .WillOnce(Invoke([&](const std::string& /*url*/,
                                 const std::string& /*path*/,
                                 const ProgressCallback& progress_callback) {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
                progress_callback(0, Status::Error, CURLcode::CURLE_COULDNT_RESOLVE_HOST);
                return false;
            }));
    }

    void expect_one_simulated_download_to_succeed(
        const std::shared_ptr<CurlWrapperMock>& curl_wrapper,
        const std::string& url,
        const std::string& path)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _))
            .WillOnce(Invoke([&](const std::string& /*url*/,
                                 const std::string& got_path,
                                 const ProgressCallback& progress_callback) {
                for (size_t i = 0; i <= 100; i++) {
                    if (progress_callback != nullptr) {
                        progress_callback(i, Status::Downloading, CURLcode::CURLE_OK);
                    }
                }

                write_file(got_path, "downloaded file content\n");

                std::this_thread::sleep_for(std::chrono::milliseconds(20));

                progress_callback(100, Status::Finished, CURLcode::CURLE_OK);
                return true;
            }));
    }
};

TEST_F(HttpLoaderTest, HttpLoader_DownloadAsync_OneBad)
{
    auto curl_wrapper_mock = std::make_shared<CurlWrapperMock>();
    auto http_loader = std::make_shared<HttpLoader>(curl_wrapper_mock);

    expect_one_simulated_download_to_succeed(curl_wrapper_mock, _file_url_1, _file_local_path_1);
    expect_one_simulated_download_to_fail(curl_wrapper_mock, _file_url_2, _file_local_path_2);
    expect_one_simulated_download_to_succeed(curl_wrapper_mock, _file_url_3, _file_local_path_3);

    std::vector<int> callback_results_progress;
    int callback_finished_counter = 0;
    int callback_error_counter = 0;

    ProgressCallback progress =
        [&callback_results_progress, &callback_finished_counter, &callback_error_counter](
            int got_progress, Status status, CURLcode curl_code) -> int {
        if (status == Status::Downloading) {
            callback_results_progress.push_back(got_progress);
        } else if (status == Status::Error && curl_code == CURLcode::CURLE_COULDNT_RESOLVE_HOST) {
            callback_error_counter++;
        } else if (status == Status::Finished && curl_code == CURLcode::CURLE_OK) {
            callback_finished_counter++;
        }
        return 0;
    };

    http_loader->download_async(_file_url_1, _file_local_path_1, progress);
    http_loader->download_async(_file_url_2, _file_local_path_2, progress);
    http_loader->download_async(_file_url_3, _file_local_path_3, progress);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    EXPECT_EQ(check_file_exists(_file_local_path_1), true);
    EXPECT_EQ(check_file_exists(_file_local_path_2), false);
    EXPECT_EQ(check_file_exists(_file_local_path_3), true);

    int callback_results_sum =
        std::accumulate(callback_results_progress.begin(), callback_results_progress.end(), 0);
    EXPECT_EQ(callback_results_sum, 2 * 5050);
    EXPECT_EQ(callback_results_progress.size(), 2 * 101);
    EXPECT_EQ(callback_finished_counter, 2);
    EXPECT_EQ(callback_error_counter, 1);

    clean();
}

TEST_F(HttpLoaderTest, HttpLoader_DownloadAsync_AllGood)
{
    auto curl_wrapper_mock = std::make_shared<CurlWrapperMock>();
    auto http_loader = std::make_shared<HttpLoader>(curl_wrapper_mock);

    expect_all_simulated_downloads_to_succeed(curl_wrapper_mock);

    std::vector<int> callback_results_progress;
    int callback_finished_counter = 0;
    int callback_error_counter = 0;

    ProgressCallback progress =
        [&callback_results_progress, &callback_finished_counter, &callback_error_counter](
            int got_progress, Status status, CURLcode curl_code) -> int {
        if (status == Status::Downloading) {
            callback_results_progress.push_back(got_progress);
        } else if (status == Status::Error && curl_code == CURLcode::CURLE_COULDNT_RESOLVE_HOST) {
            callback_error_counter++;
        } else if (status == Status::Finished && curl_code == CURLcode::CURLE_OK) {
            callback_finished_counter++;
        }
        return 0;
    };

    http_loader->download_async(_file_url_1, _file_local_path_1, progress);
    http_loader->download_async(_file_url_2, _file_local_path_2, progress);
    http_loader->download_async(_file_url_3, _file_local_path_3, progress);

    std::this_thread::sleep_for(std::chrono::milliseconds(300));

    EXPECT_EQ(check_file_exists(_file_local_path_1), true);
    EXPECT_EQ(check_file_exists(_file_local_path_2), true);
    EXPECT_EQ(check_file_exists(_file_local_path_3), true);

    int callback_results_sum =
        std::accumulate(callback_results_progress.begin(), callback_results_progress.end(), 0);
    EXPECT_EQ(callback_results_sum, 3 * 5050);
    EXPECT_EQ(callback_results_progress.size(), 3 * 101);
    EXPECT_EQ(callback_finished_counter, 3);
    EXPECT_EQ(callback_error_counter, 0);

    clean();
}
