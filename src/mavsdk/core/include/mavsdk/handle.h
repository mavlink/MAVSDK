#pragma once

#include <cstdint>

namespace mavsdk {

template<typename... Args> class FakeHandle;
template<typename... Args> class HandleFactory;

/**
 * @brief A handle returned from subscribe which allows to unsubscribe again.
 */
template<typename... Args> class Handle {
public:
    Handle() = default;
    ~Handle() = default;

    bool valid() const { return _id != 0; }

    bool operator<(const Handle& other) const { return _id < other._id; }
    bool operator==(const Handle& other) const { return _id == other._id; }

private:
    explicit Handle(uint64_t id) : _id(id) {}
    uint64_t _id{0};

    friend FakeHandle<Args...>;
    template<typename...> friend class HandleFactory;
};

} // namespace mavsdk
