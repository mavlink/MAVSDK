#pragma once

#include <functional>
#include <memory>
#include <string>
#include <array>
#include <limits>

#include "plugin_base.h"

namespace mavsdk {

class ShellImpl;
class System;

/**
 * @brief This class allow users to communicate with vehicle's system shell.
 */
class Shell : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto shell = std::make_shared<Shell>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Shell(System& system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Shell();

    /**
     * @brief Shell Message type
     */
    struct ShellMessage {
        bool need_response; /**< @brief Set if wants the receiver to send a response. */
        uint16_t timeout; /**< @brief Timeout for reply data in ms. */
        std::string data; /**< @brief Serial data. */
    };

    /**
     * @brief Shell requests results type.
     */
    struct Result {
        /**
         * @brief Shell Resul Code enum
         */
        enum class ResultCode {
            UNKNOWN = 0, /**< @brief Unknown error. */
            SUCCESS, /**< @brief %Request succeeded. */
            NO_SYSTEM, /**< @brief No system connected. */
            CONNECTION_ERROR, /**< @brief %Connection error. */
            DATA_TOO_LONG, /**< @brief Request Data too long. */
            NO_RESPONSE /**< @brief Response does not received */
        };

        ResultCode result_code; /**< @brief Result code. */
        ShellMessage response; /**< @brief Response (nullptr if not presented). */
    };

    /**
     * @brief Get human-readable English string for Shell::Result::ResultCode.
     *
     * @param result The enum value for which string is needed.
     * @return Human readable string for the Shell::Result::ResultCode.
     */
    static const char* result_code_str(Result::ResultCode result);

    /**
     * @brief Send the shell message.
     *
     * @param shell_message Shell `struct`.
     */
    void shell_command(ShellMessage shell_message);

    /**
     * @brief Callback type for shell requests.
     */
    typedef std::function<void(Result)> result_callback_t;

    /**
     * @brief Set Shell message Response callback (asynchronous).
     *
     * @param callback Function to call with responses.
     */
    void shell_command_response_async(result_callback_t callback);

    /**
     * @brief Copy constructor (object is not copyable).
     */
    Shell(const Shell&) = delete;

    /**
     * @brief Equality operator (object is not copyable).
     */
    const Shell& operator=(const Shell&) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<ShellImpl> _impl;
};

/**
 * @brief Equal operator to compare two `Shell::Quaternion` objects.
 *
 * @return `true` if items are equal.
 */
bool operator==(const Shell::ShellMessage& lhs, const Shell::ShellMessage& rhs);

/**
 * @brief Stream operator to print information about a `Shell::Quaternion`.
 *
 * @return A reference to the stream.
 */
std::ostream& operator<<(std::ostream& str, Shell::ShellMessage const& shell_message);

} // namespace mavsdk