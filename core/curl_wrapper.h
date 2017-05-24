//#pragma once

#include <string>
#include <memory>
#include "curl_include.h"

#ifdef TESTING
#include <gmock/gmock.h>
using namespace testing;
#endif // TESTING

namespace dronelink {

typedef std::function<int(int progress)> progress_callback_t;

struct dl_up_progress {
    int progress_in_percentage = 0.0;
    progress_callback_t progress_callback;
};

class ICurlWrapper
{
public:
    virtual bool download(const std::string &url, std::string &content) = 0;
    virtual bool downloadAndSaveWithProgress(const std::string &url, const std::string &path,
                                             const progress_callback_t &progress_callback) = 0;
    virtual bool uploadFile(const std::string &url, const std::string &path, const
                            progress_callback_t &progress_callback) = 0;

    virtual ~ICurlWrapper() {}
};

class CurlWrapper : public ICurlWrapper
{
public:
    CurlWrapper();
    ~CurlWrapper();

    // ICurlWrapper
    bool download(const std::string &url, std::string &content) override;
    bool downloadAndSaveWithProgress(const std::string &url, const std::string &path,
                                     const progress_callback_t &progress_callback) override;
    bool uploadFile(const std::string &url, const std::string &path, const
                    progress_callback_t &progress_callback) override;

private:
    std::shared_ptr<CURL> curl;
};

#ifdef TESTING
class CurlWrapperMock : public ICurlWrapper
{
public:
    MOCK_METHOD2(download, bool(const std::string &url, std::string &content));
    MOCK_METHOD3(downloadAndSaveWithProgress, bool(const std::string &url, const std::string &path,
                                                   const progress_callback_t &progress_callback));
    MOCK_METHOD3(uploadFile, bool(const std::string &url, const std::string &path, const
                                  progress_callback_t &progress_callback));
};
#endif // TESTING

} // namespace dronelink
