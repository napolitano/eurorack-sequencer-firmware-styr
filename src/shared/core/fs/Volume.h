/**
 * @file Volume.h
 * @brief Declares the Volume component used by the shared filesystem support.
 *
 * @author Simon Kallweit — original PER|FORMER implementation lineage
 * @author Axel Napolitano — Styr modifications, integration and modernization
 * @copyright 2017-2018 Simon Kallweit
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "Error.h"

#include "drivers/SdCard.h"

#include <cstddef>
#include <cstdint>

namespace fs {

/**
 * @brief Owns and manages a mounted FatFs filesystem volume.
 */
class Volume {
public:
    /**
     * @brief Constructs a Volume instance.
     *
     * @param[in] sdcard SD-card driver used to mount and query the volume.
     */
    Volume(SdCard &sdcard);
    /**
     * @brief Destroys the Volume instance.
     */
    ~Volume();

    /**
     * @brief Returns the sdcard.
     *
     * @return Reference to the sdcard.
     */
    SdCard &sdcard() { return _sdcard; }

    /**
     * @brief Returns the available.
     *
     * @return `true` if available; otherwise `false`.
     */
    bool available();

    /**
     * @brief Returns the format.
     *
     * @return Filesystem status returned by the format operation.
     */
    Error format();

    /**
     * @brief Returns the mount.
     *
     * @return Filesystem status returned by the mount operation.
     */
    Error mount();
    /**
     * @brief Returns the unmount.
     *
     * @return Filesystem status returned by the unmount operation.
     */
    Error unmount();

    /**
     * @brief Returns stats.
     *
     * @param[in] total Destination receiving total storage capacity in bytes.
     * @param[in] free Destination receiving free storage capacity in bytes.
     *
     * @return Result of stats().
     */
    Error stats(size_t *total, size_t *free) const;
    /**
     * @brief Returns the size total.
     *
     * @return Total volume capacity in bytes.
     */
    size_t sizeTotal() const;
    /**
     * @brief Returns the size free.
     *
     * @return Currently available volume capacity in bytes.
     */
    size_t sizeFree() const;

private:
    /**
     * @brief Reference to sdcard owned by another component.
     */
    SdCard &_sdcard; ///< Reference to sdcard owned by another component.
    FATFS _fs; ///< FatFs volume object owned by this mounted-volume wrapper.
};

} // namespace fs
