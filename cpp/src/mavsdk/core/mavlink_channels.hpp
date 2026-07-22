#pragma once

#include <cstdint>
#include <mutex>
#include "mavsdk_export.h"

namespace mavsdk {

class MAVSDK_TEST_EXPORT MavlinkChannels {
public:
    // Defined in the .cpp so shared-library builds have one allocator. An
    // inline Meyers singleton in the header would give the library and each
    // test binary their own channel map while still sharing
    // mavlink_get_channel_status(), which races packing on channel 0.
    static MavlinkChannels& Instance();

    // delete copy and move constructors and assign operators
    MavlinkChannels(MavlinkChannels const&) = delete; // Copy construct
    MavlinkChannels(MavlinkChannels&&) = delete; // Move construct
    MavlinkChannels& operator=(MavlinkChannels const&) = delete; // Copy assign
    MavlinkChannels& operator=(MavlinkChannels&&) = delete; // Move assign

    MavlinkChannels();
    ~MavlinkChannels() = default;

    /**
     * Check out a free channel and mark it as used.
     *
     * @param new_channel: the channel assigned to use
     * @return true if a free channel was available.
     */
    bool checkout_free_channel(uint8_t& new_channel);

    /**
     * Check a used channel back in and mark it as free again.
     *
     * @param used_channel: the channel that is returned
     */
    void checkin_used_channel(uint8_t used_channel);

    static constexpr uint8_t get_max_channels() { return MAX_CHANNELS; }

private:
    // It is not clear what the limit of the number of channels is, except UINT8_MAX.
    static constexpr uint8_t MAX_CHANNELS = 32;

    bool _channels_used[MAX_CHANNELS];
    std::mutex _channels_used_mutex;
};

} // namespace mavsdk
