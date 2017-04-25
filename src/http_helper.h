//#pragma once

#include <string>
#include <memory>
#include "curl_include.h"

namespace dronelink {

class HttpHelper
{
public:
    HttpHelper();
    ~HttpHelper();

    std::string download(const std::string url);
    bool downloadAndSave(const std::string url, const std::string path);
    bool uploadFile(const std::string url, const std::string path);

private:
    std::shared_ptr<CURL> curl;
};

} // namespace dronelink
