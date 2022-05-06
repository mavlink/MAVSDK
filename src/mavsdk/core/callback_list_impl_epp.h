#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "callback_list.h"
#include "eventpp/callbacklist.h"

namespace mavsdk {

template<typename... Args> class CallbackListImplEpp {
public:
    Handle<Args...> subscribe(const std::function<void(Args...)> callback)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        auto handle = Handle<Args...>(_last_id++);
        auto epp_handle = _epp_callback_list.append(callback);
        _epp_handle_map.emplace(std::make_pair(handle._id, epp_handle));

        return handle;
    }

    void unsubscribe(Handle<Args...> handle)
    {
        auto epp_handle = _epp_handle_map.find(handle._id);
        if (epp_handle != _epp_handle_map.end()) {
            _epp_callback_list.remove(epp_handle->second);
        }
    }

    void exec(Args... args) { _epp_callback_list(args...); }

private:
    mutable std::mutex _mutex{};
    uint64_t _last_id{0};
    using epp_handle_t = typename eventpp::CallbackList<void(Args...)>::Handle;

    std::map<uint64_t, epp_handle_t> _epp_handle_map;
    eventpp::CallbackList<void(Args...)> _epp_callback_list;
};

} // namespace mavsdk
