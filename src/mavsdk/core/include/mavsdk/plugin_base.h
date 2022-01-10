#pragma once

namespace mavsdk {

/**
 * @brief Creates an overloaded callable object from multiple other callables
 */
template<class... Ts> struct overloaded : Ts... {
    using Ts::operator()...;
};

/**
 * @brief Template deduction helper for `overloaded`
 */
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

/**
 * @brief Base class for every plugin.
 */
class PluginBase {
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
