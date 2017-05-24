#include "dronelink.h"
#include "curl_wrapper.h"
#include <fstream>
#include <iostream> 

using namespace dronelink;

class CurlTest : public testing::Test
{
protected:
    std::string _file_url_existing_http;
    std::string _file_url_existing_https;
    std::string _file_url_not_existing;
    std::string _local_path;

    virtual void SetUp()
    {
        _file_url_existing_http = "http://s3.eu-central-1.amazonaws.com/404f358a-48b5-4aaf-b5fd-adc84ffb0f31/dronelink_test_file";
        _file_url_existing_https = "https://s3.eu-central-1.amazonaws.com/404f358a-48b5-4aaf-b5fd-adc84ffb0f31/dronelink_test_file";
        _file_url_not_existing = "https://notexisting.file";
        _local_path = "testfile.txt";
    }

    virtual void TearDown()
    {
        clean();
    }

    void clean()
    {
        remove(_local_path.c_str());
    }

    bool check_file_exists(const std::string& file_path)
    {
        std::ifstream infile(file_path.c_str());
        return infile.good();
    }
};

TEST_F(CurlTest, Curl_DownloadText_HTTP_Success)
{
    clean();

    std::string content;

    CurlWrapper curl_wrapper;
    bool success = curl_wrapper.download_text(_file_url_existing_http, content);

    EXPECT_EQ(success, true);
    EXPECT_EQ(content, "content of this file\n");
}

TEST_F(CurlTest, Curl_DownloadText_HTTPS_Success)
{
    clean();

    std::string content;

    CurlWrapper curl_wrapper;
    bool success = curl_wrapper.download_text(_file_url_existing_https, content);

    EXPECT_EQ(success, true);
    EXPECT_EQ(content, "content of this file\n");
}

TEST_F(CurlTest, Curl_DownloadText_FileNotFound)
{
    clean();

    std::string content = "content gets cleared if download fails";

    CurlWrapper curl_wrapper;
    bool success = curl_wrapper.download_text(_file_url_not_existing, content);

    EXPECT_EQ(success, false);
    EXPECT_EQ(content, "");
}

TEST_F(CurlTest, Curl_DownloadFile_WithoutProgressFeedback_Success)
{
    clean();

    CurlWrapper curl_wrapper;

    bool success = curl_wrapper.download_file_to_path(_file_url_existing_https, _local_path, nullptr);
    EXPECT_EQ(success, true);

    bool file_exists = check_file_exists(_local_path);
    EXPECT_EQ(file_exists, true);
}

TEST_F(CurlTest, Curl_DownloadFile_WithoutProgressFeedback_FileNotFound)
{
    clean();

    CurlWrapper curl_wrapper;

    bool success = curl_wrapper.download_file_to_path(_file_url_not_existing, _local_path, nullptr);
    EXPECT_EQ(success, false);

    bool file_exists = check_file_exists(_local_path);
    EXPECT_EQ(file_exists, false);
}

TEST_F(CurlTest, Curl_DownloadFile_ProgressFeedback_Success)
{
    clean();

    int last_progress;
    auto progress = [&last_progress](int progress) -> int { last_progress = progress; return 0; };

    CurlWrapper curl_wrapper;

    bool success = curl_wrapper.download_file_to_path(_file_url_existing_https, _local_path, progress);
    EXPECT_EQ(success, true);
    EXPECT_EQ(last_progress, 100);

    bool file_exists = check_file_exists(_local_path);
    EXPECT_EQ(file_exists, true);
}

TEST_F(CurlTest, Curl_DownloadFile_ProgressFeedback_FileNotFound)
{
    clean();

    int last_progress = -1;
    auto progress = [&last_progress](int progress) -> int { last_progress = progress; return 0; };

    CurlWrapper curl_wrapper;

    bool success = curl_wrapper.download_file_to_path(_file_url_not_existing, _local_path, progress);
    EXPECT_EQ(success, false);
    EXPECT_EQ(last_progress, -1);

    bool file_exists = check_file_exists(_local_path);
    EXPECT_EQ(file_exists, false);
}
