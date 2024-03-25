#pragma once

#include <array>
#include <cmath>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "plugin_base.h"

#include "handle.h"

namespace mavsdk {

class System;
class WinchReelImpl;

/**
 * @brief Allows users to send winch actions to a Reel winch, as well as receive status information 
 * from Reel winch systems.
 */
class WinchReel : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto winch = WinchReel(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit WinchReel(System& system); // deprecated

    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto winch = WinchReel(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit WinchReel(std::shared_ptr<System> system); // new

    /**
     * @brief Destructor (internal use only).
     */
    ~WinchReel() override;

    /**
     * @brief WinchReel Action type.
     */
    enum WinchAction {
        Relaxed = 0, /**< @brief Allow motor to freewheel. */
        Lock = 3, /**< @brief Perform the locking sequence to relieve motor while in the fully retracted
                 position. */
        Deliver = 4, /**< @brief Sequence of drop, slow down, touch down, reel up, lock. */
        Hold = 5, /**< @brief Engage motor and hold current position. */
        Retract = 6, /**< @brief Return the reel to the fully retracted position. */
        LoadLine = 7, /**< @brief Load the reel with line. The winch will calculate the total loaded
                     length and stop when the tension exceeds a threshold. */
        LoadPayload = 9, /**< @brief Spools out just enough to present the hook to the user to load the
                        payload. */
    };

    /**
     * @brief Stream operator to print information about a `WinchReel::WinchAction`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream& operator<<(std::ostream& str, WinchReel::WinchAction const& winch_action);

    /**
     * @brief WinchReel Status enum.
     *
     * Enum values define the (mutually exclusive) state of the winch
     *
     */
    enum StatusEnum {
        Undefined = 0,
        Bootup = 1, /**< @brief Winch is booting up */
        Locked = 2, /**< @brief Winch is locked by locking mechanism */
        Freewheel = 3, /**< @brief Winch is gravity dropping payload */
        Hold = 4, /**< @brief Winch is holding */
        ReelUp = 5, /**< @brief Winch is winding reel up */
        ReelDown = 6, /**< @brief Winch is winding reel down */
        DropAndArrest = 7, /**< @brief Winch is delivering the payload. */
        GroundSense = 8, /**< @brief Winch is using torque measurements to sense the ground */
        ReturnToHome = 9,
        Redeliver = 10, /**< @brief Winch is redelivering the payload. This is a failover state if
                             the line tension goes above a threshold during RETRACTING. */
        AbandonLine = 11, /**< @brief Winch is abandoning the line and possibly payload. Winch
                                unspools the entire calculated line length. This is a failover state
                                from REDELIVER if the number of attempts exceeds a threshold. */
        Locking = 12, /**< @brief Winch is engaging the locking mechanism */
        LoadPackage = 13, /**< @brief Winch is loading a payload */
        LoadLine = 14, /**< @brief Winch is spooling on line */
        Failsafe = 15, /**< @brief Winch is in a failsafe state */
        Unlocking = 16 /**< @brief Winch is disengaging the locking mechanism */
    };

    /**
     * @brief Stream operator to print information about a `WinchReel::StatusEnum`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream& operator<<(std::ostream& str, WinchReel::StatusEnum const& status_enum);

    /**
     * @brief Status type.
     */
    struct Status {
        uint64_t time_usec{}; /**< @brief Time in usec */
        float line_length_m{}; /**< @brief Length of the line in meters */
        float speed_m_s{}; /**< @brief Speed in meters per second */
        float tension_kg{}; /**< @brief Tension in kilograms */
        float voltage_v{}; /**< @brief Voltage in volts */
        float current_a{}; /**< @brief Current in amperes */
        int32_t temperature_c{}; /**< @brief Temperature in Celsius */
        StatusEnum status_enum{}; /**< @brief Status enum */
    };

    /**
     * @brief Equal operator to compare two `WinchReel::Status` objects.
     *
     * @return `true` if items are equal.
     */
    friend bool operator==(const WinchReel::Status& lhs, const WinchReel::Status& rhs);

    /**
     * @brief Stream operator to print information about a `WinchReel::Status`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream& operator<<(std::ostream& str, WinchReel::Status const& status);

    /**
     * @brief Possible results returned for winch action requests.
     */
    enum class Result {
        Unknown, /**< @brief Unknown result. */
        Success, /**< @brief Request was successful. */
        NoSystem, /**< @brief No system is connected. */
        Busy, /**< @brief Temporarily rejected. */
        Timeout, /**< @brief Request timed out. */
        Unsupported, /**< @brief Action not supported. */
        Failed, /**< @brief Action failed. */
    };

    /**
     * @brief Stream operator to print information about a `WinchReel::Result`.
     *
     * @return A reference to the stream.
     */
    friend std::ostream& operator<<(std::ostream& str, WinchReel::Result const& result);

    /**
     * @brief Callback type for asynchronous Winch calls.
     */
    using ResultCallback = std::function<void(Result)>;

    /**
     * @brief Callback type for subscribe_status.
     */
    using StatusCallback = std::function<void(Status)>;

    /**
     * @brief Handle type for subscribe_status.
     */
    using StatusHandle = Handle<Status>;

    /**
     * @brief Subscribe to 'winch status' updates.
     */
    StatusHandle subscribe_status(const StatusCallback& callback);

    /**
     * @brief Unsubscribe from subscribe_status
     */
    void unsubscribe_status(StatusHandle handle);

    /**
     * @brief Poll for 'Status' (blocking).
     *
     * @return One Status update.
     */
    Status status() const;

    /**
     * @brief Allow motor to freewheel.
     *
     * This function is non-blocking. See 'relax' for the blocking counterpart.
     */
    void relax_async(uint32_t instance, const ResultCallback callback);

    /**
     * @brief Allow motor to freewheel.
     *
     * This function is blocking. See 'relax_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    Result relax(uint32_t instance) const;

    /**
     * @brief Perform the locking sequence to relieve motor while in the fully retracted position.
     *
     * This function is non-blocking. See 'lock' for the blocking counterpart.
     */
    void lock_async(uint32_t instance, const ResultCallback callback);

    /**
     * @brief Perform the locking sequence to relieve motor while in the fully retracted position.
     *
     * This function is blocking. See 'lock_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    Result lock(uint32_t instance) const;

    /**
     * @brief Sequence of drop, slow down, touch down, reel up, lock.
     *
     * This function is non-blocking. See 'deliver' for the blocking counterpart.
     */
    void deliver_async(uint32_t instance, const ResultCallback callback);

    /**
     * @brief Sequence of drop, slow down, touch down, reel up, lock.
     *
     * This function is blocking. See 'deliver_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    Result deliver(uint32_t instance) const;

    /**
     * @brief Engage motor and hold current position.
     *
     * This function is non-blocking. See 'hold' for the blocking counterpart.
     */
    void hold_async(uint32_t instance, const ResultCallback callback);

    /**
     * @brief Engage motor and hold current position.
     *
     * This function is blocking. See 'hold_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    Result hold(uint32_t instance) const;

    /**
     * @brief Return the reel to the fully retracted position.
     *
     * This function is non-blocking. See 'retract' for the blocking counterpart.
     */
    void retract_async(uint32_t instance, const ResultCallback callback);

    /**
     * @brief Return the reel to the fully retracted position.
     *
     * This function is blocking. See 'retract_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    Result retract(uint32_t instance) const;

    /**
     * @brief Load the reel with line.
     *
     * The winch will calculate the total loaded length and stop when the tension exceeds a
     * threshold.
     *
     * This function is non-blocking. See 'load_line' for the blocking counterpart.
     */
    void load_line_async(uint32_t instance, const ResultCallback callback);

    /**
     * @brief Load the reel with line.
     *
     * The winch will calculate the total loaded length and stop when the tension exceeds a
     * threshold.
     *
     * This function is blocking. See 'load_line_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    Result load_line(uint32_t instance) const;

    /**
     * @brief Spools out just enough to present the hook to the user to load the payload.
     *
     * This function is non-blocking. See 'load_payload' for the blocking counterpart.
     */
    void load_payload_async(uint32_t instance, const ResultCallback callback);

    /**
     * @brief Spools out just enough to present the hook to the user to load the payload.
     *
     * This function is blocking. See 'load_payload_async' for the non-blocking counterpart.
     *
     * @return Result of request.
     */
    Result load_payload(uint32_t instance) const;

    /**
     * @brief Copy constructor.
     */
    WinchReel(const WinchReel& other);

    /**
     * @brief Equality operator (object is not copyable).
     */
    const WinchReel& operator=(const WinchReel&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<WinchReelImpl> _impl;
};

} // namespace mavsdk