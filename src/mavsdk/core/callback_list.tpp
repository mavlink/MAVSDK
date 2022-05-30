// Include guard here, just in case
#pragma once

#include "callback_list.h"
#include "callback_list_impl.h"

namespace mavsdk {

template<typename... Args>
CallbackList<Args...>::CallbackList() :
    _impl(std::make_shared<CallbackListImpl<Args...>>())
{}

template<typename... Args>
Handle<Args...> CallbackList<Args...>::subscribe(const std::function<void(Args...)>& callback)
{
    return _impl->subscribe(callback);
}

template<typename... Args> void CallbackList<Args...>::unsubscribe(Handle<Args...> handle)
{
    _impl->unsubscribe(handle);
}

template<typename... Args> void CallbackList<Args...>::operator()(Args... args)
{
    _impl->exec(args...);
}

} // namespace mavsdk
