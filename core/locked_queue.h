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

    size_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    class Guard {
    public:
        Guard(std::mutex &mutex, std::deque<std::shared_ptr<T>> &queue) :
            _mutex(mutex),
            _queue(queue)
        {
            _mutex.lock();
        }

        ~Guard() { _mutex.unlock(); }

        std::shared_ptr<T> get_front()
        {
            if (_queue.size() == 0) {
                return nullptr;
            }
            return _queue.front();
        }

        void pop_front() { _queue.pop_front(); }

    private:
        std::mutex &_mutex;
        std::deque<std::shared_ptr<T>> &_queue;
    };

    Guard guard()
    {
        Guard new_guard{_mutex, _queue};
        return new_guard;
    }

private:
    std::deque<std::shared_ptr<T>> _queue{};
    std::mutex _mutex{};
};

} // namespace dronecode_sdk
