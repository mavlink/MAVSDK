#pragma once

#include "handle.h"
#include <atomic>

namespace mavsdk {

/**
 * @brief class to create unique Handle instances (thread-safe).
 */
template<typename... Args> class HandleFactory {
public:
    /**
     * Create a new handle
     */
    Handle<Args...> create()
    {
        return Handle<Args...>(_next_id.fetch_add(1, std::memory_order_relaxed));
    }

    /**
     * Convert from another type of Handle into this one
     */
    template<typename... ArgsOther> Handle<Args...> convert_from(const Handle<ArgsOther...>& other)
    {
        return Handle<Args...>(other._id);
    }

    /**
     * Convert from this type of Handle into another one
     */
    template<typename... ArgsOther> Handle<ArgsOther...> convert_to(const Handle<Args...>& other)
    {
        return Handle<ArgsOther...>(other._id);
    }

private:
    std::atomic_uint64_t _next_id{1}; ///< Start at 1 because 0 is the "null handle"
};

} // namespace mavsdk
