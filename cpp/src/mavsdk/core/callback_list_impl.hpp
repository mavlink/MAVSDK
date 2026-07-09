#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <cstddef>
#include <functional>
#include <future>
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
// The list is owned by a plugin, which the user may destroy while the io thread is still
// running. The posted mutations capture `this`, so the destructor waits (once) for the
// io_context to flush any that are still queued before the list is torn down.
template<typename... Args> class CallbackListImpl {
public:
    explicit CallbackListImpl(asio::io_context& io_context) : _io_context(io_context) {}

    ~CallbackListImpl() { drain(); }

    Handle<Args...> subscribe(const std::function<void(Args...)>& callback)
    {
        // The handle factory is thread-safe, so hand out the handle synchronously and
        // apply the actual insertion on the io thread.
        auto handle = _handle_factory.create();

        if (callback != nullptr) {
            post_mutation([this, handle, callback]() {
                _list.emplace_back(handle, callback);
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

    void unsubscribe(Handle<Args...> handle)
    {
        // Ignore null handle.
        if (!handle.valid()) {
            LogErr("Invalid null handle");
            return;
        }

        post_mutation([this, handle]() {
            _list.erase(
                std::remove_if(
                    _list.begin(), _list.end(), [&](auto& pair) { return pair.first == handle; }),
                _list.end());
            update_size();
        });
    }

    void exec(Args... args)
    {
        read_on_io([&]() {
            for (const auto& pair : _list) {
                pair.second(args...);
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
            for (const auto& pair : _list) {
                queue_func([callback = pair.second, args...]() { callback(args...); });
            }
            return;
        }
        asio::post(_io_context, [this, args..., queue_func]() {
            for (const auto& pair : _list) {
                queue_func([callback = pair.second, args...]() { callback(args...); });
            }
        });
    }

    bool empty() { return _size.load(std::memory_order_acquire) == 0; }

    void clear()
    {
        post_mutation([this]() {
            _list.clear();
            _cond_cb_list.clear();
            update_size();
        });
    }

private:
    // Always called on the io thread, where the list is mutated.
    void update_size()
    {
        _size.store(_list.size() + _cond_cb_list.size(), std::memory_order_release);
    }

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
    std::vector<std::pair<Handle<Args...>, std::function<void(Args...)>>> _list{};
    std::vector<std::function<bool(Args...)>> _cond_cb_list{};
    std::atomic<std::size_t> _size{0};
};

} // namespace mavsdk
