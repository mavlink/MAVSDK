#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>
#include "handle.hpp"
#include "mavsdk_export.h"

// Forward-declared so this header (the public face of the CallbackList pair) does not pull
// asio in; the full include lives in callback_list_impl.hpp.
namespace asio {
class io_context;
}

namespace mavsdk {

template<typename... Args> class CallbackListImpl;

template<typename... Args> class MAVSDK_PUBLIC CallbackList {
public:
    explicit CallbackList(asio::io_context& io_context);
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
