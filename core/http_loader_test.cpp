#include "dronelink.h"
#include "http_loader.h"
#include "curl_wrapper.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>

using namespace dronelink;

class HttpLoaderTest : public testing::Test
{
protected:
    const std::string _file_url_1 = "http://subdomain.domain.com/some_folder/some_file";
    const std::string _file_url_2 = "http://subdomain.domain.com/some_folder/another_file.txt";
    const std::string _file_url_3 = "http://subdomain.domain.com/some_folder/yet_another_file.mp4";
    const std::string _file_local_path_1 = "some_file";
    const std::string _file_local_path_2 = "another_file.txt";
    const std::string _file_local_path_3 = "yet_another_file.mp4";

    virtual void SetUp()
    {
        clean();
    }

    virtual void TearDown()
    {
        clean();
    }

    void clean()
    {
        remove(_file_local_path_1.c_str());
        remove(_file_local_path_2.c_str());
        remove(_file_local_path_3.c_str());
    }

    bool check_file_exists(const std::string &file_path)
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

    void expect_all_simulated_downloads_to_succeed(const std::shared_ptr<CurlWrapperMock> &curl_wrapper)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(_, _, _))
            .WillRepeatedly(Invoke([&](const std::string &/*url*/, const std::string &path,
                                      const progress_callback_t & progress_callback) {
                for (size_t i = 0; i <= 100; i++) {
                    if (progress_callback != nullptr) { 
                        progress_callback(i);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                write_file(path, "downloaded file content\n");
                return true;
            }));
    }

    void expect_one_simulated_download_to_fail(const std::shared_ptr<CurlWrapperMock> &curl_wrapper, const std::string& url, const std::string& path)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _))
            .WillOnce(Invoke([&](const std::string &/*url*/, const std::string &/*path*/,
                                      const progress_callback_t & /*progress_callback*/) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                return false;
            }));
    }

    void expect_one_simulated_download_to_succeed(const std::shared_ptr<CurlWrapperMock> &curl_wrapper, const std::string& url, const std::string& path)
    {
        EXPECT_CALL(*curl_wrapper, download_file_to_path(url, path, _))
            .WillOnce(Invoke([&](const std::string &/*url*/, const std::string &path,
                                      const progress_callback_t & progress_callback) {
                for (size_t i = 0; i <= 100; i++) {
                    if (progress_callback != nullptr) { 
                        progress_callback(i);
                    }
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(50));

                write_file(path, "downloaded file content\n");
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

    std::vector<int> callback_results;

    progress_callback_t progress = [&callback_results](int progress) -> int { callback_results.push_back(progress); return 0; };
    http_loader->download_async(_file_url_1, _file_local_path_1, progress);
    http_loader->download_async(_file_url_2, _file_local_path_2, progress);
    http_loader->download_async(_file_url_3, _file_local_path_3, progress);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    EXPECT_EQ(check_file_exists(_file_local_path_1), true);
    EXPECT_EQ(check_file_exists(_file_local_path_2), false);
    EXPECT_EQ(check_file_exists(_file_local_path_3), true);

    EXPECT_EQ(callback_results.size(), 2*101);
    
    int callback_results_sum = std::accumulate(callback_results.begin(), callback_results.end(), 0);
    EXPECT_EQ(callback_results_sum, 2*5050 + 0*(-1));

    clean();
}

TEST_F(HttpLoaderTest, HttpLoader_DownloadAsync_AllGood)
{
    auto curl_wrapper_mock = std::make_shared<CurlWrapperMock>(); 
    auto http_loader = std::make_shared<HttpLoader>(curl_wrapper_mock); 

    expect_all_simulated_downloads_to_succeed(curl_wrapper_mock);

    std::vector<int> callback_results;

    progress_callback_t progress = [&callback_results](int progress) -> int { callback_results.push_back(progress); return 0; };
    http_loader->download_async(_file_url_1, _file_local_path_1, progress);
    http_loader->download_async(_file_url_2, _file_local_path_2, progress);
    http_loader->download_async(_file_url_3, _file_local_path_3, progress);

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    EXPECT_EQ(check_file_exists(_file_local_path_1), true);
    EXPECT_EQ(check_file_exists(_file_local_path_2), true);
    EXPECT_EQ(check_file_exists(_file_local_path_3), true);

    EXPECT_EQ(callback_results.size(), 3*101);
    
    int callback_results_sum = std::accumulate(callback_results.begin(), callback_results.end(), 0);
    EXPECT_EQ(callback_results_sum, 3*5050 + 0*(-1));

    clean();
}
