// Include guard here, just in case
#pragma once

#include "callback_list.h"
#include "callback_list_impl.h"
#include "callback_list_impl_epp.h"

namespace mavsdk {
template<typename... Args>
CallbackList<Args...>::CallbackList() :
    _impl(std::make_shared<CallbackListImpl<Args...>>()),
    _impl_epp(std::make_shared<CallbackListImplEpp<Args...>>())
{}

template<typename... Args>
Handle<Args...> CallbackList<Args...>::subscribe(const std::function<void(Args...)>&& callback)
{
    _impl_epp->subscribe(callback);
    return _impl->subscribe(callback);
}

template<typename... Args> void CallbackList<Args...>::unsubscribe(Handle<Args...> handle)
{
    _impl_epp->unsubscribe(handle);
    _impl->unsubscribe(handle);
}

template<typename... Args> void CallbackList<Args...>::operator()(Args... args)
{
    _impl_epp->exec(args...);
    _impl->exec(args...);
}

// template class CallbackList<int, double>;
// template class CallbackList<>;
} // namespace mavsdk
