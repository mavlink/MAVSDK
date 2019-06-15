#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "plugin_base.h"

namespace mavsdk {

class System;
class ParamImpl;

/**
 * @brief The Param class provides raw access to get and set parameters.
 */
class Param : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto param = std::make_shared<Param>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Param(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Param();

    /**
     * @brief Possible results returned for param requests.
     */
    enum class Result {
        UNKNOWN, /**< @brief Unknown error. */
        SUCCESS, /**< @brief %Request succeeded. */
        TIMEOUT, /**< @brief Request timed out. */
        CONNECTION_ERROR, /**< @brief Connection error. */
        WRONG_TYPE, /**< @brief Error. */
        PARAM_NAME_TOO_LONG /**< @brief Parameter name too long (> 16). */
    };

    /**
     * @brief Returns a human-readable English string for `Param::Result`.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the `Param::Result`.
     */
    static std::string result_str(Result result);

    /**
     * @brief Get an int parameter.
     *
     * If the type is wrong, the result will be `Result::WRONG_TYPE`.
     *
     * @return a pair of the result of the request and the param (if successful).
     */
    std::pair<Result, int32_t> get_param_int(const std::string &name);

    /**
     * @brief Set an int parameter.
     *
     * If the type is wrong, the result will be `Result::WRONG_TYPE`.
     *
     * @return result of the request.
     */
    Result set_param_int(const std::string &name, int32_t value);

    /**
     * @brief Get a float parameter.
     *
     * If the type is wrong, the result will be `Result::WRONG_TYPE`.
     *
     * @return a pair of the result of the request and the param (if successful).
     */
    std::pair<Result, float> get_param_float(const std::string &name);

    /**
     * @brief Set a float parameter.
     *
     * If the type is wrong, the result will be `Result::WRONG_TYPE`.
     *
     * @return result of the request.
     */
    Result set_param_float(const std::string &name, float value);

    /**
     * @brief Copy Constructor (object is not copyable).
     */
    Param(const Param &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Param &operator=(const Param &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<ParamImpl> _impl;
};

} // namespace mavsdk
