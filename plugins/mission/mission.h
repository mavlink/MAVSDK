#pragma once

#include "mission_item.h"
#include <vector>
#include <memory>
#include <functional>

namespace dronelink {

class MissionImpl;

class Mission
{
public:
    explicit Mission(MissionImpl *impl);
    ~Mission();

    enum class Result {
        SUCCESS = 0,
        ERROR,
        TOO_MANY_MISSION_ITEMS,
        BUSY,
        TIMEOUT,
        INVALID_ARGUMENT,
        UNKNOWN
    };

    static const char *result_str(Result result);

    typedef std::function<void(Result)> result_callback_t;


    void send_mission_async(const std::vector<std::shared_ptr<MissionItem>> &mission_items,
                            result_callback_t callback);

    void start_mission_async(result_callback_t callback);
    void pause_mission_async(result_callback_t callback);

    void set_current_mission_item_async(int current, result_callback_t callback);

    bool mission_finished() const;

    int current_mission_item() const;
    int total_mission_items() const;

    typedef std::function<void(int current, int total)> progress_callback_t;
    void subscribe_progress(progress_callback_t callback);

    // Non-copyable
    Mission(const Mission &) = delete;
    const Mission &operator=(const Mission &) = delete;

private:
    // Underlying implementation, set at instantiation
    MissionImpl *_impl;
};

} // namespace dronelink
