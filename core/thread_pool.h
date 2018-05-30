#pragma once

#include <mutex>
#include <functional>
#include <vector>
#include <memory>
#include <thread>
#include <atomic>
#include "global_include.h"
#include "safe_queue.h"

namespace dronecore {

class ThreadPool {
public:
    ThreadPool(unsigned num_threads);
    ~ThreadPool();

    // delete copy and move constructors and assign operators
    ThreadPool(ThreadPool const &) = delete; // Copy construct
    ThreadPool(ThreadPool &&) = delete; // Move construct
    ThreadPool &operator=(ThreadPool const &) = delete; // Copy assign
    ThreadPool &operator=(ThreadPool &&) = delete; // Move assign

    bool start();
    bool stop();
    void enqueue(std::function<void()> func);

private:
    void worker();

    std::atomic<bool> _should_stop{false};
    const unsigned _num_threads;
    std::vector<std::shared_ptr<std::thread>> _threads{};
    SafeQueue<std::function<void()>> _work_queue{};
};

} // namespace dronecore
