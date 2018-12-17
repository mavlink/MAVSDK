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
        Guard(LockedQueue &locked_queue) : _locked_queue(locked_queue)
        {
            _locked_queue._mutex.lock();
        }

        ~Guard() { _locked_queue._mutex.unlock(); }

        Guard(Guard &other) = delete;
        Guard(const Guard &other) = delete;
        Guard(Guard &&other) = delete;
        Guard(const Guard &&other) = delete;
        Guard &operator=(const Guard &other) = delete;
        Guard &operator=(Guard &&other) = delete;

        std::shared_ptr<T> get_front()
        {
            if (_locked_queue._queue.size() == 0) {
                return nullptr;
            }
            return _locked_queue._queue.front();
        }

        void pop_front() { _locked_queue._queue.pop_front(); }

    private:
        LockedQueue<T> &_locked_queue;
    };

private:
    std::deque<std::shared_ptr<T>> _queue{};
    std::mutex _mutex{};
};

} // namespace dronecode_sdk
