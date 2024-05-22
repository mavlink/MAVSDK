#pragma once
#include "curl_include.h"
#include <functional>

namespace mavsdk {

enum class HttpStatus { Idle = 0, Downloading = 1, Uploading = 2, Finished = 3, Error = 4 };

using ProgressCallback = std::function<int(int progress, HttpStatus status, CURLcode curl_code)>;

struct UpProgress {
    int progress_in_percentage = 0;
    ProgressCallback progress_callback{nullptr};
};

} // namespace mavsdk
