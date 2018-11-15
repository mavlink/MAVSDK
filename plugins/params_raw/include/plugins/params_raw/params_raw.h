#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "plugin_base.h"

namespace dronecode_sdk {

class System;
class ParamsRawImpl;

/**
 * @brief The ParamsRaw class provides raw access to get and set parameters.
 */
class ParamsRaw : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto info = std::make_shared<ParamsRaw>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit ParamsRaw(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~ParamsRaw();

    /**
     * @brief Possible results returned for params_raw requests.
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
     * @brief Returns a human-readable English string for `ParamsRaw::Result`.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the `ParamsRaw::Result`.
     */
    std::string result_str(Result result);

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
    ParamsRaw(const ParamsRaw &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const ParamsRaw &operator=(const ParamsRaw &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<ParamsRawImpl> _impl;
};

} // namespace dronecode_sdk
