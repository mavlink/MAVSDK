#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace mavsdk {

// Blocking wrappers around async calls, which the owner can release on destruction.
//
// A blocking call waits for a callback that only the io thread delivers. Teardown stops the
// io thread first, so a call still waiting then would wait forever. That only happens when
// the caller destroys the plugin or Mavsdk during the call, which is not supported. The owner
// calls release_all() from its destructor to fail such calls instead of hanging them.
class BlockingCalls {
public:
    BlockingCalls() = default;
    ~BlockingCalls() = default;
    BlockingCalls(const BlockingCalls&) = delete;
    BlockingCalls& operator=(const BlockingCalls&) = delete;

    // Calls start() with a completion function and waits for it to be called, or for
    // release_all(), which completes it with on_release instead.
    template<typename T, typename Start> T wait(Start&& start, T on_release)
    {
        // The waiter may wake up after the owner is gone, so it only touches the shared state.
        auto state = _state;
        auto pending = std::make_shared<Pending<T>>();
        auto complete = [pending](T value) {
            std::call_once(pending->once, [&]() { pending->promise.set_value(std::move(value)); });
        };

        uint64_t id;
        {
            std::lock_guard<std::mutex> lock(state->mutex);
            if (state->released) {
                return on_release;
            }
            id = state->next_id++;
            state->release_functions.emplace(
                id, [complete, on_release]() mutable { complete(std::move(on_release)); });
        }

        start(complete);

        auto result = pending->promise.get_future().get();

        std::lock_guard<std::mutex> lock(state->mutex);
        state->release_functions.erase(id);
        return result;
    }

    // Completes every waiting call with its on_release value, and any later one immediately.
    // Returns how many calls were still waiting.
    std::size_t release_all()
    {
        std::vector<std::function<void()>> to_call;
        {
            std::lock_guard<std::mutex> lock(_state->mutex);
            _state->released = true;
            for (auto& entry : _state->release_functions) {
                to_call.push_back(std::move(entry.second));
            }
            _state->release_functions.clear();
        }
        for (auto& release : to_call) {
            release();
        }
        return to_call.size();
    }

private:
    template<typename T> struct Pending {
        std::promise<T> promise;
        std::once_flag once;
    };

    struct State {
        std::mutex mutex;
        bool released{false};
        uint64_t next_id{0};
        std::unordered_map<uint64_t, std::function<void()>> release_functions;
    };

    std::shared_ptr<State> _state{std::make_shared<State>()};
};

} // namespace mavsdk
