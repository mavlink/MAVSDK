#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "handle.h"

namespace mavsdk {

template<typename... Args> class CallbackListImpl;

template<typename... Args> class CallbackList {
public:
    CallbackList();
    ~CallbackList();

    Handle<Args...> subscribe(const std::function<void(Args...)>& callback);
    void unsubscribe(Handle<Args...> handle);
    void subscribe_conditional(const std::function<bool(Args...)>& callback);
    void operator()(Args... args);
    [[nodiscard]] bool empty();
    void clear();
    void queue(Args... args, const std::function<void(const std::function<void()>&)>& queue_func);

private:
    std::unique_ptr<CallbackListImpl<Args...>> _impl;
};

} // namespace mavsdk
