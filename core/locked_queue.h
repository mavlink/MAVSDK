#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include <cassert>

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
        // Should not be possible because _mutex.lock()
        assert(_queue_borrowed == false); // double borrow
        if (_queue.size() == 0) {
            // We couldn't borrow anything, therefore don't keep the lock.
            _mutex.unlock();
            return nullptr;
        }
        _queue_borrowed = true;
        return _queue.front();
    }

    // This allows to return a borrowed queue.
    void return_front()
    {
        assert(_queue_borrowed == true); // return without borrow
        _queue_borrowed = false;
        _mutex.unlock();
    }

    void pop_front()
    {
        assert(_queue_borrowed == true); // pop without borrow
        _queue.pop_front();
        _queue_borrowed = false;
        _mutex.unlock();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

private:
    std::deque<std::shared_ptr<T>> _queue{};
    std::mutex _mutex{};
    bool _queue_borrowed = false;
};

} // namespace dronecode_sdk
