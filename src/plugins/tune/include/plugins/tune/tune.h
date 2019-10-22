#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "plugin_base.h"

namespace mavsdk {

class TuneImpl;
class System;

/**
 * @brief The Tune class allows to send a tune to be played by the autopilot.
 */
class Tune : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto tune = std::make_shared<Tune>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Tune(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Tune();

    /**
     * @brief Possible results returned for tune requests.
     */
    enum class Result {
        SUCCESS = 0, /**< @brief %Request succeeded. */
        INVALID_TEMPO, /**< @brief %Invalid tempo (range: 32 - 255). */
        TUNE_TOO_LONG, /**< @brief %Invalid tune: encoded string must be at most 230 chars. */
        ERROR, /**< @brief %Failed to send the request. */
    };

    /**
     * @brief Gets a human-readable English string for an Tune::Result.
     *
     * @param result Enum for which string is required.
     * @return Human readable string for the Tune::Result.
     */
    static const char* result_str(Result result);

    /**
     * @brief Callback type for async tune calls.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Enum representing a song element.
     */
    enum class SongElement {
        STYLE_LEGATO,
        STYLE_NORMAL,
        STYLE_STACCATO,
        DURATION_1,
        DURATION_2,
        DURATION_4,
        DURATION_8,
        DURATION_16,
        DURATION_32,
        NOTE_A,
        NOTE_B,
        NOTE_C,
        NOTE_D,
        NOTE_E,
        NOTE_F,
        NOTE_G,
        NOTE_PAUSE,
        SHARP,
        FLAT,
        OCTAVE_UP,
        OCTAVE_DOWN,
    };

    /**
     * @brief Send a tune to be played by the system (asynchronous).
     *
     * @param tune Reference to a vector of song elements.
     * @param tempo Tempo in quarter notes per minute (32 - 255).
     * @param callback Callback to receive result of this request.
     */
    void play_tune_async(
        const std::vector<SongElement>& tune, const int tempo, result_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Tune(const Tune&) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Tune& operator=(const Tune&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<TuneImpl> _impl;
};

} // namespace mavsdk
