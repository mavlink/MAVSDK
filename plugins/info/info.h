#pragma once

#include <cstdint>

namespace dronecore {

class InfoImpl;

/**
 * The Info class provides basic info about the hardware and/or software of a device.
 */
class Info
{
public:
    /**
     * Constructor for Info called internally.
     */
    explicit Info(InfoImpl *impl);

    /**
     * Destructor for Info called internally.
     */
    ~Info();

    /**
     * Length of git hash strings.
     *
     * Length is 16 chars + null termination.
     */
    static const unsigned GIT_HASH_STR_LEN = 17;

    /**
     * Type with version information of a device.
     */
    struct Version {
        int flight_sw_major; /**< flight software major version */
        int flight_sw_minor; /**< flight software minor version */
        int flight_sw_patch; /**< flight software patch version */
        int flight_sw_vendor_major; /**< flight software vendor major version */
        int flight_sw_vendor_minor; /**< flight software vendor minor version */
        int flight_sw_vendor_patch; /**< flight software vendor patch version */
        char flight_sw_git_hash[GIT_HASH_STR_LEN]; /**< flight software git hash as string */
        int os_sw_major; /**< operating system software major version */
        int os_sw_minor; /**< operating system software minor version */
        int os_sw_patch; /**< operating system software patch version */
        char os_sw_git_hash[GIT_HASH_STR_LEN];/**< operating system software git hash as string */
        uint16_t vendor_id; /**< ID of board vendor */
        uint16_t product_id; /**< ID of product */
    };

    /**
     * Returns UUID of a device.
     *
     * If possible this should be a unique identifier provided by hardware.
     *
     * @return the UUID
     */
    uint64_t uuid() const;

    /**
     * Tests if this Version object is fully populated from hardware.
     *
     * @return true if all information needed could be received from device
     */
    bool is_complete() const;

    /**
     * Return version information of device.
     *
     * @return version
     */
    Version get_version() const;

    // Non-copyable
    Info(const Info &) = delete;
    const Info &operator=(const Info &) = delete;

private:
    /** @private Underlying implementation, set at instantiation */
    InfoImpl *_impl;
};

} // namespace dronecore
