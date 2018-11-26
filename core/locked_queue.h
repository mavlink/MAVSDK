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

        // With some compilers (e.g. MSVC 2017 for the debug build)
        // the return value optimization does not trigger and the move
        // constructor is called when Guard is returned in guard().
        // This means that the old object is destructed while the member
        // data is passed on to the new object. Since we are unlocking
        // in the destructor, we need to prevent this from happening for
        // the object which is discarded after the move constructor.
        // This is achieved using the _invalidated flag.
        Guard(Guard &other) = delete;
        Guard(const Guard &other) = delete;
        Guard &operator=(const Guard &other) = delete;
        Guard &operator=(Guard &&other) = delete;

        Guard(Guard &&other) : _mutex(other._mutex), _queue(other._queue)
        {
            other._invalidated = true;
        }

        Guard(const Guard &&other) : _mutex(other._mutex), _queue(other._queue)
        {
            other._invalidated = true;
        }

        ~Guard()
        {
            if (_invalidated) {
                return;
            }
            _mutex.unlock();
        }

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
        bool _invalidated{false};
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
