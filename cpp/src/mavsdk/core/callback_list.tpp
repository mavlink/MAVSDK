// Include guard here, just in case
#pragma once

#include "callback_list.h"
#include "callback_list_impl.h"

namespace mavsdk {

template<typename... Args>
CallbackList<Args...>::CallbackList() :
    _impl(std::make_unique<CallbackListImpl<Args...>>())
{}

template<typename... Args>
CallbackList<Args...>::~CallbackList() = default;

template<typename... Args>
Handle<Args...> CallbackList<Args...>::subscribe(const std::function<void(Args...)>& callback)
{
    return _impl->subscribe(callback);
}

template<typename... Args> void CallbackList<Args...>::unsubscribe(Handle<Args...> handle)
{
    _impl->unsubscribe(handle);
}

template<typename... Args>
void CallbackList<Args...>::subscribe_conditional(const std::function<bool(Args...)>& callback)
{
    _impl->subscribe_conditional(callback);
}

template<typename... Args> void CallbackList<Args...>::operator()(Args... args)
{
    _impl->exec(args...);
}

template<typename... Args> bool CallbackList<Args...>::empty()
{
    return _impl->empty();
}

template<typename... Args> void CallbackList<Args...>::clear()
{
    _impl->clear();
}

template<typename... Args> void CallbackList<Args...>::queue(Args... args, const std::function<void(const std::function<void()>&)>& queue_func)
{
    _impl->queue(args..., queue_func);
}

} // namespace mavsdk
