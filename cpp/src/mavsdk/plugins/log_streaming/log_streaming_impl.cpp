#include <future>

#include "log_streaming_impl.h"
#include "log_streaming_backend_px4.h"
#include "log_streaming_backend_ardupilot.h"
#include "plugins/log_streaming/log_streaming.h"
#include "callback_list.tpp"
#include "base64.h"

namespace mavsdk {

template class CallbackList<LogStreaming::LogStreamingRaw>;

LogStreamingImpl::LogStreamingImpl(System& system) : PluginImplBase(system)
{
    _system_impl->register_plugin(this);
}

LogStreamingImpl::LogStreamingImpl(std::shared_ptr<System> system) :
    PluginImplBase(std::move(system))
{
    _system_impl->register_plugin(this);
}

LogStreamingImpl::~LogStreamingImpl()
{
    _system_impl->unregister_plugin(this);
}

void LogStreamingImpl::init()
{
    if (const char* env_p = std::getenv("MAVSDK_LOG_STREAMING_DEBUGGING")) {
        if (std::string(env_p) == "1") {
            LogDebug() << "Log streaming debugging is on.";
            _debugging = true;
        }
    }
}

void LogStreamingImpl::deinit()
{
    std::lock_guard<std::mutex> lock(_mutex);

    // Cancel any pending autopilot type polling
    if (_check_autopilot_cookie) {
        _system_impl->remove_call_every(_check_autopilot_cookie);
        _check_autopilot_cookie = {};
    }
    _start_callback = nullptr;

    if (_backend) {
        _backend->deinit();
        _backend.reset();
    }
}

void LogStreamingImpl::enable() {}

void LogStreamingImpl::disable()
{
    std::lock_guard<std::mutex> lock(_mutex);
    if (_backend) {
        _backend->deinit();
        _backend.reset();
    }
}

bool LogStreamingImpl::maybe_create_backend()
{
    // Already have a backend
    if (_backend) {
        return true;
    }

    auto autopilot = _system_impl->effective_autopilot();

    // Don't create backend yet if autopilot type is unknown
    if (autopilot == Autopilot::Unknown) {
        if (_debugging) {
            LogDebug() << "Autopilot type unknown, cannot create backend yet";
        }
        return false;
    }

    if (autopilot == Autopilot::ArduPilot) {
        if (_debugging) {
            LogDebug() << "Creating ArduPilot log streaming backend";
        }
        _backend = std::make_unique<LogStreamingBackendArdupilot>();
    } else {
        if (_debugging) {
            LogDebug() << "Creating PX4 log streaming backend";
        }
        _backend = std::make_unique<LogStreamingBackendPx4>();
    }

    _backend->set_debugging(_debugging);
    _backend->init(_system_impl.get());
    _backend->set_data_callback([this](const std::vector<uint8_t>& data) { process_data(data); });
    return true;
}

void LogStreamingImpl::process_data(const std::vector<uint8_t>& data)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (_debugging) {
        LogDebug() << "Processing log data with size " << data.size();
    }

    // Convert to base64
    LogStreaming::LogStreamingRaw part;
    std::vector<uint8_t> data_copy = data;
    part.data_base64 = base64_encode(data_copy);

    // Let's pass it to the user.
    if (!_subscription_callbacks.empty()) {
        _subscription_callbacks.queue(
            part, [this](const auto& func) { _system_impl->call_user_callback(func); });
    }
}

void LogStreamingImpl::start_log_streaming_async(const LogStreaming::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    // Try to create backend if not yet created
    if (maybe_create_backend()) {
        _backend->start_log_streaming_async(callback);
        return;
    }

    // Autopilot type unknown - wait for it with polling.
    // Use timeout_s() for the total wait time, polling every 0.1s.
    const float poll_interval_s = 0.1f;
    const unsigned max_polls = static_cast<unsigned>(_system_impl->timeout_s() / poll_interval_s);

    _start_callback = callback;
    _autopilot_poll_count = 0;

    _check_autopilot_cookie = _system_impl->add_call_every(
        [this, max_polls]() {
            std::lock_guard<std::mutex> lock2(_mutex);

            if (maybe_create_backend()) {
                // Success - stop polling and start streaming
                _system_impl->remove_call_every(_check_autopilot_cookie);
                _backend->start_log_streaming_async(_start_callback);
                _start_callback = nullptr;
                return;
            }

            _autopilot_poll_count++;
            if (_autopilot_poll_count >= max_polls) {
                // Timeout - stop polling and report failure
                _system_impl->remove_call_every(_check_autopilot_cookie);
                if (_start_callback) {
                    auto cb = _start_callback;
                    _start_callback = nullptr;
                    _system_impl->call_user_callback(
                        [cb]() { cb(LogStreaming::Result::NoSystem); });
                }
            }
        },
        poll_interval_s);
}

LogStreaming::Result LogStreamingImpl::start_log_streaming()
{
    auto prom = std::promise<LogStreaming::Result>{};
    auto fut = prom.get_future();

    start_log_streaming_async([&](LogStreaming::Result result) { prom.set_value(result); });

    return fut.get();
}

void LogStreamingImpl::stop_log_streaming_async(const LogStreaming::ResultCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);

    if (!_backend) {
        if (callback) {
            _system_impl->call_user_callback(
                [callback]() { callback(LogStreaming::Result::NoSystem); });
        }
        return;
    }

    _backend->stop_log_streaming_async(callback);
}

LogStreaming::Result LogStreamingImpl::stop_log_streaming()
{
    auto prom = std::promise<LogStreaming::Result>{};
    auto fut = prom.get_future();

    stop_log_streaming_async([&](LogStreaming::Result result) { prom.set_value(result); });

    return fut.get();
}

LogStreaming::LogStreamingRawHandle
LogStreamingImpl::subscribe_log_streaming_raw(const LogStreaming::LogStreamingRawCallback& callback)
{
    std::lock_guard<std::mutex> lock(_mutex);
    auto handle = _subscription_callbacks.subscribe(callback);

    return handle;
}

void LogStreamingImpl::unsubscribe_log_streaming_raw(LogStreaming::LogStreamingRawHandle handle)
{
    std::lock_guard<std::mutex> lock(_mutex);
    _subscription_callbacks.unsubscribe(handle);
}

} // namespace mavsdk
