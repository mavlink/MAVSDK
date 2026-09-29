#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <future>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <asio/io_context.hpp>
#include <asio/post.hpp>
#include "log.hpp"
#include "callback_list.hpp"
#include "handle_factory.hpp"

namespace mavsdk {

// Threading: the callback list is only ever touched on the io_context thread.
//
// - exec()/queue() iterate the list; they run inline when already on the io thread (the
//   common case, driven by received-message handlers and timers). Off it, queue() posts the
//   access without waiting (it only hands callbacks to a queue, so it need not be
//   synchronous), while exec() posts and waits (it invokes the callbacks directly, so the
//   caller's arguments must stay alive until they have run). exec() must therefore not be
//   called off the io thread while holding a lock that the io thread needs.
// - subscribe()/subscribe_conditional()/unsubscribe()/clear() post their list mutation
//   without waiting. Posting (rather than running inline) means they never mutate the list
//   while exec() is iterating it, so it is safe to (un)subscribe from inside a callback; not
//   waiting means they never block, so it is also safe to call them while holding a lock (a
//   blocking round-trip onto the io thread would deadlock if the io thread needed that same
//   lock). The mutation lands on the next io turn.
//
// Unsubscribing cannot rely on the list alone. queue() hands a *copy* of the callback to a
// queue that another thread drains, so removing the list entry says nothing about copies
// already handed over. Each subscription carries a small shared state instead, which those
// copies hold on to:
//
// - unsubscribe() marks it dead synchronously, before returning. Every dispatch checks it
//   under the state's own lock, so once unsubscribe() returns the callback will not be
//   invoked again -- whether or not the list entry has been erased yet, and whether the
//   invocation was going to come from exec() or from a queue.
// - unsubscribe_blocking() additionally waits for an invocation that is already running.
//   Together that is "neither running nor going to run", which is what a caller about to
//   release whatever the callback captured needs.
//
// The state outlives the list, being shared with the queued copies, so destroying the list
// gives the same guarantee as unsubscribing everything in it.
//
// The list is owned by a plugin, which the user may destroy while the io thread is still
// running. The posted mutations capture `this`, so the destructor waits (once) for the
// io_context to flush any that are still queued before the list is torn down.
template<typename... Args> class CallbackListImpl {
public:
    explicit CallbackListImpl(asio::io_context& io_context) : _io_context(io_context) {}

    ~CallbackListImpl()
    {
        // Before draining, so that a copy still sitting in someone's queue does nothing when it
        // is eventually run.
        mark_all_dead();
        drain();
    }

    Handle<Args...> subscribe(const std::function<void(Args...)>& callback)
    {
        // The handle factory is thread-safe, so hand out the handle synchronously and
        // apply the actual insertion on the io thread.
        auto handle = _handle_factory.create();

        if (callback != nullptr) {
            // Tracked here as well as in _list, because unsubscribe() has to reach it from
            // whatever thread it is called on, possibly before the insertion below has run.
            auto subscription = std::make_shared<Subscription>();
            {
                std::lock_guard<std::mutex> lock(_subscriptions_mutex);
                _subscriptions.insert({handle, subscription});
            }

            post_mutation([this, handle, callback, subscription]() {
                _list.push_back(Entry{handle, callback, subscription});
                update_size();
            });
        } else {
            LogErr("Use new unsubscribe methods instead of subscribe(nullptr). "
                   "See: https://mavsdk.mavlink.io/main/en/cpp/api_changes.html#unsubscribe");
            clear();
        }

        return handle;
    }

    void subscribe_conditional(const std::function<bool(Args...)>& callback)
    {
        if (callback != nullptr) {
            post_mutation([this, callback]() {
                _cond_cb_list.emplace_back(callback);
                update_size();
            });
        } else {
            clear();
        }
    }

    // Once this returns the callback will not be invoked again. One that is running right now
    // keeps running; use unsubscribe_blocking() when that matters.
    void unsubscribe(Handle<Args...> handle)
    {
        // Ignore null handle.
        if (!handle.valid()) {
            LogErr("Invalid null handle");
            return;
        }

        if (take_subscription(handle) == nullptr) {
            // Not ours, or already unsubscribed.
            return;
        }

        post_mutation(make_erase(handle));
    }

    // Blocking variant of unsubscribe(): also waits for an invocation that is already running,
    // so once it returns the callback is neither running nor going to run and whatever it
    // captured can be destroyed. Do not call it while holding a lock the callback itself takes.
    // Calling it from inside the callback is fine, it then skips the wait.
    void unsubscribe_blocking(Handle<Args...> handle)
    {
        // Ignore null handle.
        if (!handle.valid()) {
            LogErr("Invalid null handle");
            return;
        }

        auto subscription = take_subscription(handle);
        if (subscription == nullptr) {
            // Not ours, or already unsubscribed.
            return;
        }

        wait_until_idle(*subscription);

        auto erase = make_erase(handle);

        // The io thread is gone, so nothing can be iterating the list.
        if (_io_context.stopped()) {
            erase();
            return;
        }

        // Already on the io thread means we are inside a callback, so exec() is iterating _list
        // right now. Erasing would leave it calling a moved-from std::function, and waiting
        // would be waiting for ourselves, so defer like unsubscribe() does.
        if (on_io_thread()) {
            post_mutation(erase);
            return;
        }

        std::promise<void> done;
        asio::post(_io_context, [&]() {
            erase();
            done.set_value();
        });
        done.get_future().wait();
    }

    void exec(Args... args)
    {
        read_on_io([&]() {
            for (const auto& entry : _list) {
                Invocation invocation{*entry.subscription};
                if (invocation.entered()) {
                    entry.callback(args...);
                }
            }

            for (auto it = _cond_cb_list.begin(); it != _cond_cb_list.end();) {
                if ((*it)(args...)) {
                    // If the callback returns true, remove it based on the iterator.
                    it = _cond_cb_list.erase(it);
                } else {
                    // Otherwise, move to the next element.
                    ++it;
                }
            }
            update_size();
        });
    }

    void queue(Args... args, const std::function<void(const std::function<void()>&)>& queue_func)
    {
        // queue() only hands the callbacks to queue_func (which enqueues them to run later), so
        // unlike exec() it never needs to run synchronously. Posting it without waiting (rather
        // than blocking on the io thread) means it is safe to call while holding a lock that the
        // io thread also needs -- a blocking round-trip there would deadlock.
        if (_io_context.stopped() || on_io_thread()) {
            hand_to(queue_func, args...);
            return;
        }
        asio::post(_io_context, [this, args..., queue_func]() { hand_to(queue_func, args...); });
    }

    bool empty() { return _size.load(std::memory_order_acquire) == 0; }

    void clear()
    {
        mark_all_dead();

        post_mutation([this]() {
            _list.clear();
            _cond_cb_list.clear();
            update_size();
        });
    }

private:
    // Shared between the list entry and every copy of the callback handed to a queue, so that
    // unsubscribing reaches all of them and keeps working after the list itself is gone.
    struct Subscription {
        std::mutex mutex;
        std::condition_variable idle;
        bool alive{true};
        unsigned in_flight{0};
    };

    struct Entry {
        Handle<Args...> handle;
        std::function<void(Args...)> callback;
        std::shared_ptr<Subscription> subscription;
    };

    // Subscriptions whose callback this thread is currently inside. Lets unsubscribe_blocking()
    // called from within a callback skip the wait instead of waiting for itself.
    static std::vector<const Subscription*>& active_on_this_thread()
    {
        static thread_local std::vector<const Subscription*> active;
        return active;
    }

    // Claims the right to invoke a callback, or reports that the subscription is gone. The check
    // and the count happen under one lock, so a subscription cannot be marked dead in between
    // and have the invocation go ahead anyway.
    class Invocation {
    public:
        explicit Invocation(Subscription& subscription) : _subscription(subscription)
        {
            {
                std::lock_guard<std::mutex> lock(_subscription.mutex);
                if (!_subscription.alive) {
                    return;
                }
                ++_subscription.in_flight;
                _entered = true;
            }
            active_on_this_thread().push_back(&_subscription);
        }

        ~Invocation()
        {
            if (!_entered) {
                return;
            }
            active_on_this_thread().pop_back();
            {
                std::lock_guard<std::mutex> lock(_subscription.mutex);
                --_subscription.in_flight;
            }
            _subscription.idle.notify_all();
        }

        bool entered() const { return _entered; }

        Invocation(const Invocation&) = delete;
        Invocation& operator=(const Invocation&) = delete;

    private:
        Subscription& _subscription;
        bool _entered{false};
    };

    // Marks the subscription dead and stops tracking it. Returns nullptr for a handle that was
    // never ours or has already been unsubscribed, which makes a second unsubscribe a no-op.
    std::shared_ptr<Subscription> take_subscription(Handle<Args...> handle)
    {
        std::shared_ptr<Subscription> subscription;
        {
            std::lock_guard<std::mutex> lock(_subscriptions_mutex);
            auto it = _subscriptions.find(handle);
            if (it == _subscriptions.end()) {
                return nullptr;
            }
            subscription = it->second;
            _subscriptions.erase(it);
        }

        std::lock_guard<std::mutex> lock(subscription->mutex);
        subscription->alive = false;
        return subscription;
    }

    void mark_all_dead()
    {
        std::map<Handle<Args...>, std::shared_ptr<Subscription>> subscriptions;
        {
            std::lock_guard<std::mutex> lock(_subscriptions_mutex);
            subscriptions.swap(_subscriptions);
        }

        for (auto& entry : subscriptions) {
            std::lock_guard<std::mutex> lock(entry.second->mutex);
            entry.second->alive = false;
        }
    }

    void wait_until_idle(Subscription& subscription)
    {
        const auto& active = active_on_this_thread();
        if (std::find(active.begin(), active.end(), &subscription) != active.end()) {
            // We are inside this very callback, so the only invocation to wait for is us.
            return;
        }

        std::unique_lock<std::mutex> lock(subscription.mutex);
        subscription.idle.wait(lock, [&subscription]() { return subscription.in_flight == 0; });
    }

    // The returned mutation runs on the io thread, where the list is mutated.
    std::function<void()> make_erase(Handle<Args...> handle)
    {
        return [this, handle]() {
            _list.erase(
                std::remove_if(
                    _list.begin(),
                    _list.end(),
                    [&](const Entry& entry) { return entry.handle == handle; }),
                _list.end());
            update_size();
        };
    }

    // Always called on the io thread, where the list is read.
    void hand_to(const std::function<void(const std::function<void()>&)>& queue_func, Args... args)
    {
        for (const auto& entry : _list) {
            queue_func(
                [callback = entry.callback, subscription = entry.subscription, args...]() {
                    Invocation invocation{*subscription};
                    if (invocation.entered()) {
                        callback(args...);
                    }
                });
        }
    }

    // Always called on the io thread, where the list is mutated.
    void update_size()
    {
        _size.store(_list.size() + _cond_cb_list.size(), std::memory_order_release);
    }

    // INVARIANT: a CallbackList must be driven by a single dedicated io thread (the one that
    // calls io_context::run(), as MavsdkImpl does). Do NOT attach one to an io_context that is
    // polled from a foreign thread (e.g. a test thread calling io_context::poll()).
    //
    // This is why: running_in_this_thread() reads an asio thread-local that, because asio is
    // header-only and MAVSDK builds with hidden visibility, is instantiated separately in each
    // DSO. When the io_context is run() by a dedicated thread this is reliable, but when it is
    // poll()ed from a thread whose asio call-stack lives in a different DSO than libmavsdk.so,
    // this check can misreport. Every read_on_io()/queue()/drain()/unsubscribe path branches on
    // on_io_thread() to decide between running inline and posting-and-waiting, so a wrong answer
    // there either skips the synchronising round-trip (reintroducing a teardown use-after-free)
    // or deadlocks waiting on a thread that only polls intermittently.
    //
    // With the dedicated-io-thread invariant this is correct. If a polled/foreign-thread
    // CallbackList is ever actually needed, replace this with an explicit comparison of
    // std::this_thread::get_id() against the id recorded for the io thread rather than relying
    // on running_in_this_thread().
    bool on_io_thread() const { return _io_context.get_executor().running_in_this_thread(); }

    // Run a list read/iteration on the io thread: inline when already there, otherwise posted
    // and waited for. Safe to run inline because reads don't mutate the list, and safe to
    // capture by reference because we block until the posted access has run (which also keeps
    // the arguments, e.g. a borrowed buffer, alive for the duration).
    //
    // Waiting relies on the io_context staying alive and running: it is only stopped in
    // ~MavsdkImpl, after which the stopped() fast path applies. The stopped() check is not
    // synchronized with that teardown, so using or destroying a plugin concurrently with the
    // Mavsdk instance itself is not supported -- the posted access could then never run and
    // this wait would hang. The same applies to drain() below.
    template<typename Func> void read_on_io(Func&& func)
    {
        if (_io_context.stopped() || on_io_thread()) {
            func();
            return;
        }
        std::promise<void> done;
        asio::post(_io_context, [&]() {
            func();
            done.set_value();
        });
        done.get_future().wait();
    }

    // Post a list mutation onto the io thread, without waiting (see the class comment). When
    // the io_context is already stopped (teardown) the io thread is gone, so we apply the
    // mutation directly.
    template<typename Func> void post_mutation(Func&& func)
    {
        if (_io_context.stopped()) {
            func();
            return;
        }
        asio::post(_io_context, std::forward<Func>(func));
    }

    // Wait until the io_context has run every mutation posted so far. Used by the destructor:
    // the posted mutations capture `this`, so they must not outlive the object. If the
    // io_context is stopped the io thread is gone and nothing will run; if we are on the io
    // thread we cannot wait on ourselves (and this would only happen if a callback destroyed
    // its own list, which we don't support).
    void drain()
    {
        if (_io_context.stopped()) {
            return;
        }
        // Destroying the list from the io thread (i.e. from within a callback) is not
        // supported: we cannot wait on ourselves, and any still-queued mutations capturing
        // `this` would then run after the list is gone.
        assert(!on_io_thread());
        if (on_io_thread()) {
            return;
        }
        std::promise<void> done;
        asio::post(_io_context, [&done]() { done.set_value(); });
        done.get_future().wait();
    }

    asio::io_context& _io_context;
    HandleFactory<Args...> _handle_factory;
    std::vector<Entry> _list{};
    std::vector<std::function<bool(Args...)>> _cond_cb_list{};
    std::atomic<std::size_t> _size{0};

    // Reachable from any thread, unlike _list.
    std::mutex _subscriptions_mutex{};
    std::map<Handle<Args...>, std::shared_ptr<Subscription>> _subscriptions{};
};

} // namespace mavsdk
