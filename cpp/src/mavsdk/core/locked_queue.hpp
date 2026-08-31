#pragma once

#include <algorithm>
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

    // Outcome of push_back_bounded().
    enum class BoundedPush {
        Pushed, // There was room.
        PushedAfterDropping, // Room was made by dropping an older droppable item.
        Rejected, // No room and nothing droppable to evict, so the item was not pushed.
    };

    // Push, but keep the queue to max_size entries worth of droppable work. When it is
    // already that long, make room by dropping the *oldest* item that pred accepts --
    // oldest, so that what does get through stays current rather than lagging further and
    // further behind. If nothing in the queue may be dropped, the new item is refused
    // instead, and the caller decides what that means.
    template<class Predicate>
    BoundedPush push_back_bounded(std::shared_ptr<T> item_ptr, size_t max_size, Predicate pred)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        auto result = BoundedPush::Pushed;

        if (_queue.size() >= max_size) {
            auto it = std::find_if(_queue.begin(), _queue.end(), [&](const auto& entry) {
                return entry != nullptr && pred(*entry);
            });
            if (it == _queue.end()) {
                return BoundedPush::Rejected;
            }
            _queue.erase(it);
            result = BoundedPush::PushedAfterDropping;
        }

        _queue.push_back(item_ptr);
        _condition_var.notify_one();
        return result;
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
