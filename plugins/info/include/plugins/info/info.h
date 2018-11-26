#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include "plugin_base.h"

namespace dronecode_sdk {

class System;
class InfoImpl;

/**
 * @brief The Info class provides basic infomation about the hardware and/or software of a system.
 */
class Info : public PluginBase {
public:
    /**
     * @brief Constructor. Creates the plugin for a specific System.
     *
     * The plugin is typically created as shown below:
     *
     *     ```cpp
     *     auto info = std::make_shared<Info>(system);
     *     ```
     *
     * @param system The specific system associated with this plugin.
     */
    explicit Info(System &system);

    /**
     * @brief Destructor (internal use only).
     */
    ~Info();

    /**
     * @brief Possible results returned for requests.
     *
     * @note DronecodeSDK does not throw exceptions. Instead a result of this
     * type will be returned.
     */
    enum class Result {
        UNKNOWN, /**< @brief Unspecified error. */
        SUCCESS, /**< @brief The request was successful. */
        INFORMATION_NOT_RECEIVED_YET /**< @brief The information has not been received yet. */
    };

    /**
     * @brief Length of git hash strings.
     *
     * Length is 16 chars + null termination.
     */
    static const unsigned GIT_HASH_STR_LEN = 17;

    /**
     * @brief Type containing system version information.
     */
    struct Version {
        int flight_sw_major; /**< @brief Flight software major version. */
        int flight_sw_minor; /**<  @brief Flight software minor version. */
        int flight_sw_patch; /**<  @brief Flight software patch version. */
        int flight_sw_vendor_major; /**< @brief Flight software vendor major version. */
        int flight_sw_vendor_minor; /**< @brief Flight software vendor minor version. */
        int flight_sw_vendor_patch; /**< @brief Flight software vendor patch version. */
        char
            flight_sw_git_hash[GIT_HASH_STR_LEN]; /**< @brief Flight software git hash as string. */
        int os_sw_major; /**< @brief Operating system software major version. */
        int os_sw_minor; /**< @brief Operating system software minor version. */
        int os_sw_patch; /**< @brief Operating system software patch version. */
        char os_sw_git_hash[GIT_HASH_STR_LEN]; /**< @brief Operating system software git hash as
                                                  string. */
    };

    /**
     * @brief Type containing system product information.
     */
    struct Product {
        int vendor_id; /**< @brief ID of board vendor. */
        char vendor_name[32]; /**< @brief Name of vendor. */
        int product_id; /**< @brief ID of product. */
        char product_name[32]; /**< @brief Name of product. */
    };

    /**
     * @brief Gets the UUID of the system.
     *
     * If possible this will be a unique identifier provided by hardware.
     *
     * @return a pair containing the result of the request and if successful,
     * the UUID of the system.
     */
    std::pair<Result, uint64_t> uuid() const;

    /**
     * @brief Get system version information.
     *
     * @return a pair containing the result of the request and if successful,
     * the version information about the system.
     */
    std::pair<Result, Version> get_version() const;

    /**
     * @brief Get system product information.
     *
     * @return a pair containing the result of the request and if successful,
     * the product information about the system.
     */
    std::pair<Result, Product> get_product() const;

    /**
     * @brief Returns a human-readable English string for an Result.
     *
     * @param result The enum value for which a human readable string is required.
     * @return Human readable string for the Result.
     */
    static std::string result_str(Result result);

    /**
     * @brief Copy Constructor (object is not copyable).
     */
    Info(const Info &) = delete;
    /**
     * @brief Equality operator (object is not copyable).
     */
    const Info &operator=(const Info &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    std::unique_ptr<InfoImpl> _impl;
};

} // namespace dronecode_sdk
