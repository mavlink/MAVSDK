#pragma once

#include <queue>
#include <mutex>
#include <memory>

namespace dronecode_sdk {

template<class T> class LockedQueue {
public:
    LockedQueue(){};
    ~LockedQueue(){};

    void push_back(T item)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push_back(std::make_shared<T>(item));
    }

    // This allows to get access to the front and keep others
    // from using it. This blocks if the front is already borrowed.
    std::shared_ptr<T> borrow_front()
    {
        _mutex.lock();
        if (_queue.size() == 0) {
            // We couldn't borrow anything, therefore don't keep the lock.
            _mutex.unlock();
            return nullptr;
        }
        return _queue.front();
    }

    // This allows to return a borrowed queue.
    void return_front()
    {
        // We don't know if the mutex is locked and Windows doesn't let us
        // unlock an unowned mutex.
        _mutex.try_lock();
        _mutex.unlock();
    }

    void pop_front()
    {
        // In case it's not returned, do that now.
        return_front();

        std::lock_guard<std::mutex> lock(_mutex);
        if (_queue.size() == 0) {
            return;
        }
        _queue.pop_front();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

private:
    std::deque<std::shared_ptr<T>> _queue{};
    std::mutex _mutex{};
};

} // namespace dronecode_sdk
