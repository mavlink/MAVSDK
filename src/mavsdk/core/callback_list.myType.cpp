// Consider adding "anti-guard" to make sure it's not included in other translation units
#if __INCLUDE_LEVEL__
#error "Don't include this file"
#endif

// Yes, we include the .cpp file
#include "callback_list.cpp"
namespace mavsdk {
template class CallbackList<int, double>;
template class CallbackList<>;
} // namespace mavsdk