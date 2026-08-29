#pragma once

#include <cstddef>
#include <deque>
#include <optional>
#include <utility>

namespace mavsdk {

// Outbound queue backing a connection's async write chain.
//
// Exactly one item is in flight at a time: start() moves the next item out of the queue
// into a separate slot which stays put until finish(), so the buffer handed to
// async_write() stays valid even if the queue is trimmed in the meantime.
//
// The queue is bounded. When it is full the oldest waiting item is dropped rather than
// the newest, so a link that has stalled (a wedged TCP peer, an unplugged serial adapter)
// costs a bounded amount of memory and loses the stalest traffic, which for MAVLink is
// the traffic worth losing.
//
// Threading: no locking. Every method must be called on the io_context thread -- which is
// where connections enqueue (via asio::dispatch) and where the write handlers run.
template<typename Item> class TxQueue {
public:
    explicit TxQueue(std::size_t max_items) : _max_items(max_items) {}

    // Enqueue an item, returning how many stale items had to be dropped to make room
    // (normally 0).
    std::size_t push(Item item)
    {
        std::size_t dropped = 0;
        while (_queue.size() >= _max_items) {
            _queue.pop_front();
            ++dropped;
        }
        _queue.push_back(std::move(item));
        _dropped_total += dropped;
        return dropped;
    }

    // True while a write is in flight, i.e. between start() and finish().
    [[nodiscard]] bool busy() const { return _in_flight.has_value(); }

    // Refers to the items waiting, not counting the one in flight.
    [[nodiscard]] bool empty() const { return _queue.empty(); }
    [[nodiscard]] std::size_t size() const { return _queue.size(); }

    [[nodiscard]] std::size_t dropped_total() const { return _dropped_total; }

    // Move the next item into the in-flight slot. The returned reference stays valid
    // until finish(), so it is safe to hand to an async write.
    const Item& start()
    {
        _in_flight = std::move(_queue.front());
        _queue.pop_front();
        return *_in_flight;
    }

    void finish() { _in_flight.reset(); }

    // Drop everything still waiting. The in-flight item is deliberately left alone: its
    // buffer may still be referenced by an async operation that has not completed yet.
    void clear() { _queue.clear(); }

private:
    std::deque<Item> _queue{};
    std::optional<Item> _in_flight{};
    std::size_t _max_items;
    std::size_t _dropped_total{0};
};

} // namespace mavsdk
