#pragma once

#include "mavsdk_export.hpp"

namespace mavsdk {

/**
 * @brief Base class for every plugin.
 */
class MAVSDK_PUBLIC PluginBase {
public:
    /**
     * @brief Default Constructor.
     */
    PluginBase() = default;

    /**
     * @brief Default Destructor.
     */
    virtual ~PluginBase() = default;

    /**
     * @brief Copy constructor (object is not copyable).
     */
    PluginBase(const PluginBase&) = delete;

    /**
     * @brief Assign operator (object is not copyable).
     */
    const PluginBase& operator=(const PluginBase&) = delete;
};

} // namespace mavsdk
