#include "log.h"
#include "curl_wrapper.h"
#include "unused.h"
#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>

namespace mavsdk {

// converts curl output to string
// taken from
// https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
static size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp)
{
    reinterpret_cast<std::string*>(userp)->append(reinterpret_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

bool CurlWrapper::download_text(const std::string& url, std::string& content)
{
    auto curl = std::shared_ptr<CURL>(curl_easy_init(), curl_easy_cleanup);
    std::string readBuffer;

    if (nullptr != curl) {
        CURLcode res;

        curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl.get());
        content = readBuffer;

        if (res == CURLcode::CURLE_OK) {
            return true;
        } else {
            LogErr() << "Error while downloading text, curl error code: "
                     << curl_easy_strerror(res);
            return false;
        }
    } else {
        LogErr() << "Error: cannot start uploading because of curl initialization error. ";
        return false;
    }
}

size_t get_file_size(const std::string& path)
{
    std::streampos begin, end;
    std::ifstream my_file(path.c_str(), std::ios::binary);
    begin = my_file.tellg();
    my_file.seekg(0, std::ios::end);
    end = my_file.tellg();
    my_file.close();
    return ((end - begin) > 0) ? (end - begin) : 0;
}

template<typename T> std::string to_string(T value)
{
    std::ostringstream os;
    os << value;
    return os.str();
}

static int download_progress_update(
    void* p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    UNUSED(ultotal);
    UNUSED(ulnow);

    auto* myp = reinterpret_cast<struct UpProgress*>(p);

    if (myp->progress_callback == nullptr) {
        return 0;
    }

    if (dltotal == 0 || dlnow == 0) {
        return myp->progress_callback(0, HttpStatus::Idle, CURLcode::CURLE_OK);
    }

    int percentage = static_cast<int>(100 / dltotal * dlnow);

    if (percentage > myp->progress_in_percentage) {
        myp->progress_in_percentage = percentage;
        return myp->progress_callback(percentage, HttpStatus::Downloading, CURLcode::CURLE_OK);
    }

    return 0;
}

bool CurlWrapper::download_file_to_path(
    const std::string& url, const std::string& path, const ProgressCallback& progress_callback)
{
    auto curl = std::shared_ptr<CURL>(curl_easy_init(), curl_easy_cleanup);
    FILE* fp;

    if (nullptr != curl) {
        CURLcode res;
        struct UpProgress progress;
        progress.progress_callback = progress_callback;

        fp = fopen(path.c_str(), "wb");
        curl_easy_setopt(curl.get(), CURLOPT_CONNECTTIMEOUT, 5L);
        curl_easy_setopt(curl.get(), CURLOPT_XFERINFOFUNCTION, download_progress_update);
        curl_easy_setopt(curl.get(), CURLOPT_PROGRESSDATA, &progress);
        curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl.get(), CURLOPT_NOPROGRESS, 0L);
        res = curl_easy_perform(curl.get());
        fclose(fp);

        if (res == CURLcode::CURLE_OK) {
            if (nullptr != progress_callback) {
                progress_callback(100, HttpStatus::Finished, res);
            }
            return true;
        } else {
            if (nullptr != progress_callback) {
                progress_callback(0, HttpStatus::Error, res);
            }
            remove(path.c_str());
            LogErr() << "Error while downloading file, curl error code: "
                     << curl_easy_strerror(res);
            return false;
        }
    } else {
        LogErr() << "Error: cannot start downloading file because of curl initialization error. ";
        return false;
    }
}

} // namespace mavsdk
