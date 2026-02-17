#pragma once

#include <queue>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace mavsdk {

template<class T> class LockedQueue {
public:
    LockedQueue() = default;
    ~LockedQueue() = default;

    void push_back(std::shared_ptr<T> item_ptr)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _queue.push_back(item_ptr);
        _condition_var.notify_one();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    void stop()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _should_exit = true;
        _condition_var.notify_all();
    }

    void restart()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _should_exit = false;
    }

    using iterator = typename std::deque<std::shared_ptr<T>>::iterator;
    iterator begin() { return _queue.begin(); }

    iterator end() { return _queue.end(); }

    iterator erase(iterator it) { return _queue.erase(it); }

    // This guard serves the purpose to combine a get_front with a pop_front.
    // Thus, no one can interfere between the two steps.
    class Guard {
    public:
        explicit Guard(LockedQueue& locked_queue) :
            _locked_queue(locked_queue),
            _lock(locked_queue._mutex)
        {}

        ~Guard() = default;

        Guard(Guard& other) = delete;
        Guard(const Guard& other) = delete;
        Guard(Guard&& other) = delete;
        Guard(const Guard&& other) = delete;
        Guard& operator=(const Guard& other) = delete;
        Guard& operator=(Guard&& other) = delete;

        std::shared_ptr<T> get_front()
        {
            if (_locked_queue._queue.size() == 0) {
                return nullptr;
            }
            return _locked_queue._queue.front();
        }

        std::shared_ptr<T> wait_and_pop_front()
        {
            while (_locked_queue._queue.empty()) {
                if (_locked_queue._should_exit) {
                    return std::shared_ptr<T>{};
                }
                _locked_queue._condition_var.wait(_lock);
            }
            if (_locked_queue._should_exit) {
                return std::shared_ptr<T>{};
            }

            auto result = _locked_queue._queue.front();
            _locked_queue._queue.pop_front();
            return result;
        }

        void pop_front() { _locked_queue._queue.pop_front(); }

    private:
        LockedQueue<T>& _locked_queue;
        std::unique_lock<std::mutex> _lock;
    };

private:
    std::deque<std::shared_ptr<T>> _queue{};
    std::mutex _mutex{};
    std::condition_variable _condition_var{};
    bool _should_exit{false};
};

} // namespace mavsdk
