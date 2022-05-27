#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <utility>
#include <vector>

namespace mavsdk {

template<typename... Args> class CallbackListImpl;
template<typename... Args> class CallbackListImplEpp;
template<typename... Args> class CallbackList;

template<typename... Args> class Handle {
public:
    explicit Handle(uint64_t id) : _id(id) {}

protected:
    uint64_t _id;

private:
    friend CallbackListImpl<Args...>;
    friend CallbackListImplEpp<Args...>;
};

template<typename... Args> class CallbackList {
public:
    CallbackList();
    ~CallbackList() = default;

    Handle<Args...> subscribe(const std::function<void(Args...)>& callback);
    void unsubscribe(Handle<Args...> handle);
    void operator()(Args... args);

private:
    std::shared_ptr<CallbackListImpl<Args...>> _impl;
    //std::shared_ptr<CallbackListImplEpp<Args...>> _impl_epp;
};

} // namespace mavsdk
