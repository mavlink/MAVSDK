#pragma once


#include <queue>
#include <mutex>

namespace dronecore {

template <class T>
class LockedQueue
{
public:
    LockedQueue() :
        _queue(),
        _mutex()
    {};


    void push_back(T item)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        _queue.push_back(item);
    }

    T &front()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        return _queue.front();
    }

    void pop_front()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        _queue.pop_front();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        return _queue.size();
    }

private:
    std::deque<T> _queue;
    std::mutex _mutex;
};

} // namespace dronecore
