#pragma once
#include "curl_include.hpp"
#include <functional>

namespace mavsdk {

enum class HttpStatus { Idle = 0, Downloading = 1, Uploading = 2, Finished = 3, Error = 4 };

using ProgressCallback = std::function<void(int progress, HttpStatus status, CURLcode curl_code)>;

struct UpProgress {
    int progress_in_percentage = 0;
    ProgressCallback progress_callback{nullptr};
    curl_off_t max_download_size_bytes{0};
};

} // namespace mavsdk
