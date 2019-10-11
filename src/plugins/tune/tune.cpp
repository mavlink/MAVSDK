#include "plugins/tune/tune.h"
#include "tune_impl.h"
#include <vector>

namespace mavsdk {

Tune::Tune(System& system) : PluginBase(), _impl{new TuneImpl(system)} {}

Tune::~Tune() {}

void Tune::play_tune_async(
    const std::vector<SongElement>& tune, const int tempo, result_callback_t callback)
{
    _impl->play_tune_async(tune, tempo, callback);
}

const char* Tune::result_str(Result result)
{
    switch (result) {
        case Result::SUCCESS:
            return "Success";
        case Result::INVALID_TEMPO:
            return "Invalid tempo: must be in range 32-255";
        case Result::TUNE_TOO_LONG:
            return "Invalid tune: tune string must be 230 chars or less";
        case Result::ERROR:
            return "Error";
        default:
            return "Unknown";
    }
}

} // namespace mavsdk
