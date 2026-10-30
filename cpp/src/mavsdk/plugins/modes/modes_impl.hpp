#pragma once

#include "plugins/modes/modes.hpp"
#include "modes_fallback.hpp"
#include "plugin_impl_base.hpp"
#include "callback_list.hpp"
#include "mavlink_command_sender.hpp"

#include <map>
#include <mutex>
#include <optional>
#include <vector>

namespace mavsdk {

class ModesImpl : public PluginImplBase {
public:
    explicit ModesImpl(System& system);
    explicit ModesImpl(std::shared_ptr<System> system);
    ~ModesImpl() override;

    void init() override;
    void deinit() override;

    void enable() override;
    void disable() override;

    Modes::AvailableModesHandle
    subscribe_available_modes(const Modes::AvailableModesCallback& callback);
    void unsubscribe_available_modes(Modes::AvailableModesHandle handle);
    std::vector<Modes::Mode> available_modes() const;

    Modes::CurrentModeHandle subscribe_current_mode(const Modes::CurrentModeCallback& callback);
    void unsubscribe_current_mode(Modes::CurrentModeHandle handle);
    Modes::Mode current_mode() const;

    void set_mode_async(const std::string& name, const Modes::ResultCallback& callback);
    Modes::Result set_mode(const std::string& name);

private:
    enum class Source {
        None, // Nothing known yet
        AvailableModes, // Received using AVAILABLE_MODES
        Fallback, // Autopilot doesn't support AVAILABLE_MODES, using known modes
    };

    void start_download();
    void request_next_mode();
    void handle_available_modes_result(
        unsigned generation, MavlinkCommandSender::Result result, const mavlink_message_t& message);
    void handle_available_modes_monitor(const mavlink_message_t& message);
    void handle_current_mode(const mavlink_message_t& message);
    void handle_heartbeat(const mavlink_message_t& message);

    // These need _mutex to be held.
    void finish_download();
    void use_fallback();
    void publish_available_modes();
    void update_current_mode(uint32_t custom_mode, std::optional<uint8_t> maybe_standard_mode);
    std::vector<Modes::Mode> modes() const;

    void set_custom_mode(uint32_t custom_mode, const Modes::ResultCallback& callback);
    void call_result_callback(const Modes::ResultCallback& callback, Modes::Result result);
    static Modes::Result result_from_command_result(MavlinkCommandSender::Result result);
    static void ensure_unique_names(std::vector<ModeEntry>& entries);

    mutable std::mutex _mutex{};

    Source _source{Source::None};
    std::vector<ModeEntry> _entries{};

    // Download state
    bool _downloading{false};
    unsigned _generation{0};
    unsigned _restarts{0};
    uint8_t _number_modes{0};
    std::optional<uint8_t> _maybe_download_seq{};
    std::map<uint8_t, ModeEntry> _received_modes{};
    std::optional<uint8_t> _maybe_last_seq{};

    bool _current_mode_received{false};
    std::optional<uint32_t> _maybe_custom_mode{};
    std::optional<uint8_t> _maybe_standard_mode{};
    Modes::Mode _current_mode{};

    CallbackList<std::vector<Modes::Mode>> _available_modes_callbacks;
    CallbackList<Modes::Mode> _current_mode_callbacks;

    void* _message_cookie{};
};

} // namespace mavsdk
