#pragma once

#include <string>
#include <memory>
#include "curl_include.h"
#include "curl_wrapper_types.h"

#ifdef TESTING
#include <gmock/gmock.h>
using namespace testing;
#endif // TESTING

namespace mavsdk {

class ICurlWrapper {
public:
    ICurlWrapper() = default;
    virtual ~ICurlWrapper() = default;
    virtual bool download_text(const std::string& url, std::string& content) = 0;
    virtual bool download_file_to_path(
        const std::string& url,
        const std::string& path,
        const ProgressCallback& progress_callback) = 0;
};

class CurlWrapper : public ICurlWrapper {
public:
    // ICurlWrapper
    CurlWrapper() = default;
    virtual ~CurlWrapper() = default;
    bool download_text(const std::string& url, std::string& content) override;
    bool download_file_to_path(
        const std::string& url,
        const std::string& path,
        const ProgressCallback& progress_callback) override;
};

#ifdef TESTING
class CurlWrapperMock : public ICurlWrapper {
public:
    MOCK_METHOD2(download_text, bool(const std::string& url, std::string& content));
    MOCK_METHOD3(
        download_file_to_path,
        bool(
            const std::string& url,
            const std::string& path,
            const ProgressCallback& progress_callback));
};
#endif // TESTING

} // namespace mavsdk
