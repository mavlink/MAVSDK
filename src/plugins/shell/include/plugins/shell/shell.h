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
     * @brief Shell Result Code enum
     */
    enum class Result {
        UNKNOWN = 0, /**< @brief Unknown error. */
        SUCCESS, /**< @brief %Request succeeded. */
        NO_SYSTEM, /**< @brief No system connected. */
        CONNECTION_ERROR, /**< @brief %Connection error. */
        NO_RESPONSE, /**< @brief Response does not received */
        BUSY, /**< @brief Shell busy (transfer in progress) */
    };

    /**
     * @brief Get human-readable English string for Shell::Result::ResultCode.
     *
     * @param result The enum value for which string is needed.
     * @return Human readable string for the Shell::Result::ResultCode.
     */
    static const char* result_code_str(Result result);

    /**
     * @brief Send the shell message.
     *
     * If shell_message.data string have not trailing newline - it will be added.
     *
     * If response data looks like not completed try to increase timeout value in request.
     *
     * @param shell_message Shell `struct`.
     */
    Shell::Result shell_command(ShellMessage shell_message);

    /**
     * @brief Callback type for shell requests.
     */
    typedef std::function<void(Result result, ShellMessage response)> result_callback_t;

    /**
     * @brief Set Shell message Response callback (asynchronous).
     *
     * @param callback Function to call with responses.
     */
    Shell::Result shell_command_response_async(result_callback_t callback);

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
