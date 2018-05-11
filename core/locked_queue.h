#pragma once

#include <queue>
#include <mutex>
#include <memory>

namespace dronecore {

template <class T>
class LockedQueue
{
public:
    LockedQueue() {};
    ~LockedQueue() {};

    void push_back(T item)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push_back(item);
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
        return std::make_shared<T>(_queue.front());
    }

    // This allows to return a borrowed queue.
    void return_front()
    {
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
    std::deque<T> _queue {};
    std::mutex _mutex {};
};

} // namespace dronecore
