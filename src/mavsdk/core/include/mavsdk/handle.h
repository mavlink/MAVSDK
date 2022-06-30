#pragma once

namespace mavsdk {

template<typename... Args> class CallbackListImpl;
template<typename... Args> class CallbackList;
template<typename... Args> class FakeHandle;

/**
 * @brief A handle returned from subscribe which allows to unsubscribe again.
 */
template<typename... Args> class Handle {
private:
    Handle() = delete;
    explicit Handle(uint64_t id) : _id(id) {}
    uint64_t _id;
    friend CallbackListImpl<Args...>;
    friend FakeHandle<Args...>;
};

} // namespace mavsdk
