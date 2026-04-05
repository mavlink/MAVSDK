#pragma once

#include "mavsdk_export.hpp"

namespace mavsdk {

/**
 * @brief Base class for every server plugin.
 */
class MAVSDK_PUBLIC ServerPluginBase {
public:
    /**
     * @brief Default Constructor.
     */
    ServerPluginBase() = default;

    /**
     * @brief Default Destructor.
     */
    virtual ~ServerPluginBase() = default;

    /**
     * @brief Copy constructor (object is not copyable).
     */
    ServerPluginBase(const ServerPluginBase&) = delete;

    /**
     * @brief Assign operator (object is not copyable).
     */
    const ServerPluginBase& operator=(const ServerPluginBase&) = delete;
};

} // namespace mavsdk
