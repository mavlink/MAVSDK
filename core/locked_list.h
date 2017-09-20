#pragma once

#include <list>
#include <mutex>

namespace dronecore {

template <class T>
class LockedList
{
public:
    LockedList() :
        _list(),
        _mutex()
    {};

    class iterator: public std::list<T>::iterator
    {
    public:
        iterator(typename std::list<T>::iterator c, std::mutex &mutex) :
            std::list<T>::iterator(c),
            _mutex(&mutex)
        {}

        T &operator*()
        {
            std::lock_guard<std::mutex> lock(*_mutex);
            return std::list<T>::iterator::operator *();
        }
    private:
        std::mutex *_mutex;
    };

    iterator begin()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return iterator(_list.begin(), _mutex);
    }

    iterator end()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return iterator(_list.end(), _mutex);
    }

    iterator erase(iterator it)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return iterator(_list.erase(it), _mutex);
    }

    void push_back(T item)
    {
        std::lock_guard<std::mutex> lock(_mutex);

        _list.push_back(item);
    }

    T &front()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        return _list.front();
    }

    void pop_front()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        _list.pop_front();
    }

    size_t size()
    {
        std::lock_guard<std::mutex> lock(_mutex);

        return _list.size();
    }

private:
    std::list<T> _list;
    std::mutex _mutex;
};

} // namespace dronecore
