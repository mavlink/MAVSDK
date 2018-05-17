#include "thread_pool.h"

namespace dronecore {

ThreadPool::ThreadPool(unsigned num_threads) :
    _num_threads(num_threads)
{
}

ThreadPool::~ThreadPool()
{
    stop();
}

bool ThreadPool::start()
{
    _should_stop = false;

    for (unsigned i = 0; i < _num_threads; ++i) {
        auto new_thread = std::make_shared<std::thread>(&ThreadPool::worker, this);
        _threads.push_back(new_thread);
    }

    return true;
}

bool ThreadPool::stop()
{
    _work_queue.stop();
    _should_stop = true;
    for (auto it = _threads.begin(); it != _threads.end(); /* ++it */) {
        it->get()->join();
        it = _threads.erase(it);
    }
    return true;
}

void ThreadPool::enqueue(std::function<void()> func)
{
    _work_queue.enqueue(std::move(func));
}

void ThreadPool::worker()
{
    while (!_should_stop) {
        auto func = _work_queue.dequeue();
        if (func) {
            func();
        }
    }
}


} // namespace dronecore
