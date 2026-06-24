#pragma once

#include <algorithm>
#include <atomic>
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
//   common case, driven by received-message handlers and timers) and otherwise post the
//   access and wait for it.
// - subscribe()/subscribe_conditional() post their insertion onto the io_context. The
//   handle is created and returned synchronously (the handle factory is thread-safe); the
//   list insertion lands on the next io turn.
// - unsubscribe()/clear() mutate the list. They post the mutation rather than running it
//   inline so they never mutate the list while exec() is iterating it (which is what makes
//   it safe to unsubscribe from inside a callback). When called off the io thread they wait
//   until the mutation has been applied, so no callback can fire afterwards.
template<typename... Args> class CallbackListImpl {
public:
    explicit CallbackListImpl(asio::io_context& io_context) : _io_context(io_context) {}

    Handle<Args...> subscribe(const std::function<void(Args...)>& callback)
    {
        // The handle factory is thread-safe, so hand out the handle synchronously and
        // apply the actual insertion on the io thread.
        auto handle = _handle_factory.create();

        if (callback != nullptr) {
            asio::post(_io_context, [this, handle, callback]() {
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
            asio::post(_io_context, [this, callback]() {
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

        mutate_on_io([this, handle]() {
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
        read_on_io([&]() {
            for (const auto& pair : _list) {
                queue_func([callback = pair.second, args...]() { callback(args...); });
            }
        });
    }

    bool empty() { return _size.load(std::memory_order_acquire) == 0; }

    void clear()
    {
        mutate_on_io([this]() {
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

    // Run a list read/iteration on the io thread: inline when already there, otherwise
    // posted and waited for. Safe to run inline because reads don't mutate the list.
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

    // Run a list mutation on the io thread. Never inline: if we are on the io thread we may
    // be inside an exec() iteration, so the mutation is posted to run after it. Off the io
    // thread we post and wait so the mutation has been applied by the time we return.
    template<typename Func> void mutate_on_io(Func&& func)
    {
        if (_io_context.stopped()) {
            // The io thread is gone (teardown); accessing the list directly is safe.
            func();
            return;
        }
        if (on_io_thread()) {
            asio::post(_io_context, std::forward<Func>(func));
            return;
        }
        std::promise<void> done;
        asio::post(_io_context, [&]() {
            func();
            done.set_value();
        });
        done.get_future().wait();
    }

    asio::io_context& _io_context;
    HandleFactory<Args...> _handle_factory;
    std::vector<std::pair<Handle<Args...>, std::function<void(Args...)>>> _list{};
    std::vector<std::function<bool(Args...)>> _cond_cb_list{};
    std::atomic<std::size_t> _size{0};
};

} // namespace mavsdk
