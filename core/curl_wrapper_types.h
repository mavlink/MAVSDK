#pragma once
#include "curl_include.h"
#include <functional>

namespace dronecore {

enum class Status { Idle = 0, Downloading = 1, Uploading = 2, Finished = 3, Error = 4 };

typedef std::function<int(int progress, Status status, CURLcode curl_code)> progress_callback_t;

struct dl_up_progress {
    int progress_in_percentage = 0;
    progress_callback_t progress_callback;
};

} // namespace dronecore
