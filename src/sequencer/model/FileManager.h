/**
 * @file FileManager.h
 * @brief Declares the FileManager component used by the sequencer model.
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

#include "FileDefs.h"
#include "Project.h"
#include "UserScale.h"
#include "Settings.h"

#include "core/fs/FileSystem.h"

#include <array>
#include <functional>

#include <cstdint>

/**
 * @brief Stores and manipulates file manager model data.
 */
class FileManager {
public:
    /**
     * @brief Initializes the FileManager and its runtime resources.
     */
    static void init();

    /**
     * @brief Returns the volume available.
     *
     * @return `true` if volume available; otherwise `false`.
     */
    static bool volumeAvailable();
    /**
     * @brief Returns the volume mounted.
     *
     * @return `true` if volume mounted; otherwise `false`.
     */
    static bool volumeMounted();

    /**
     * @brief Returns the format.
     *
     * @return Filesystem status returned by the format operation.
     */
    static fs::Error format();

    /**
     * @brief Writes project.
     *
     * @param[in] project Project model read or modified by the operation.
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return Result of writeProject().
     */
    static fs::Error writeProject(Project &project, int slot);
    /**
     * @brief Reads project.
     *
     * @param[in] project Project model read or modified by the operation.
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return Result of readProject().
     */
    static fs::Error readProject(Project &project, int slot);
    /**
     * @brief Reads last project.
     *
     * @param[in] project Project model read or modified by the operation.
     *
     * @return Result of readLastProject().
     */
    static fs::Error readLastProject(Project &project);

    /**
     * @brief Writes user scale.
     *
     * @param[in] userScale User-defined scale read or modified by the operation.
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return Result of writeUserScale().
     */
    static fs::Error writeUserScale(const UserScale &userScale, int slot);
    /**
     * @brief Reads user scale.
     *
     * @param[in] userScale User-defined scale read or modified by the operation.
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return Result of readUserScale().
     */
    static fs::Error readUserScale(UserScale &userScale, int slot);

    /**
     * @brief Writes project.
     *
     * @param[in] project Project model read or modified by the operation.
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     *
     * @return Result of writeProject().
     */
    static fs::Error writeProject(const Project &project, const char *path);
    /**
     * @brief Reads project.
     *
     * @param[in] project Project model read or modified by the operation.
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     *
     * @return Result of readProject().
     */
    static fs::Error readProject(Project &project, const char *path);

    /**
     * @brief Writes user scale.
     *
     * @param[in] userScale User-defined scale read or modified by the operation.
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     *
     * @return Result of writeUserScale().
     */
    static fs::Error writeUserScale(const UserScale &userScale, const char *path);
    /**
     * @brief Reads user scale.
     *
     * @param[in] userScale User-defined scale read or modified by the operation.
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     *
     * @return Result of readUserScale().
     */
    static fs::Error readUserScale(UserScale &userScale, const char *path);

    /**
     * @brief Writes settings.
     *
     * @param[in] settings Settings model read or modified by the operation.
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     *
     * @return Result of writeSettings().
     */
    static fs::Error writeSettings(const Settings &settings, const char *path);
    /**
     * @brief Reads settings.
     *
     * @param[in] settings Settings model read or modified by the operation.
     * @param[in] path Path naming the file or directory to open, query or enumerate.
     *
     * @return Result of readSettings().
     */
    static fs::Error readSettings(Settings &settings, const char *path);

    // Slot information

    /**
     * @brief Stores and manipulates slot info model data.
     */
    struct SlotInfo {
        /**
         * @brief Whether used is true in the current state.
         */
        bool used; ///< True when this pool/file slot currently contains a live entry.
        char name[FileHeader::NameLength + 1]; ///< Null-terminated slot name expanded from the fixed-width serialized header field.
    };

    /**
     * @brief Returns metadata for the addressed storage slot.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] slot Zero-based project/storage slot index.
     * @param[in] info Controller/file metadata consumed by the operation.
     */
    static void slotInfo(FileType type, int slot, SlotInfo &info);
    /**
     * @brief Returns slot used.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return `true` if slot used; otherwise `false`.
     */
    static bool slotUsed(FileType type, int slot);

    // File tasks

    typedef std::function<fs::Error(void)> TaskExecuteCallback;
    typedef std::function<void(fs::Error)> TaskResultCallback;

    /**
     * @brief Runs the component service/background task body.
     *
     * @param[in] executeCallback Callback invoked when the deferred operation is committed.
     * @param[in] resultCallback Callback invoked with the asynchronous operation result.
     */
    static void task(TaskExecuteCallback executeCallback, TaskResultCallback resultCallback);
    /**
     * @brief Processes task.
     */
    static void processTask();

private:
    /**
     * @brief Writes file.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] slot Zero-based project/storage slot index.
     * @param[in] write Writer/serialization callback used to emit the value.
     *
     * @return Result of writeFile().
     */
    static fs::Error writeFile(FileType type, int slot, std::function<fs::Error(const char *)> write);
    /**
     * @brief Reads file.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] slot Zero-based project/storage slot index.
     * @param[in] read Reader/serialization callback used to restore the value.
     *
     * @return Result of readFile().
     */
    static fs::Error readFile(FileType type, int slot, std::function<fs::Error(const char *)> read);

    /**
     * @brief Writes last project.
     *
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return Result of writeLastProject().
     */
    static fs::Error writeLastProject(int slot);
    /**
     * @brief Reads last project.
     *
     * @param[in] slot Zero-based project/storage slot index.
     *
     * @return Result of readLastProject().
     */
    static fs::Error readLastProject(int &slot);

    /**
     * @brief Returns cached slot.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] slot Zero-based project/storage slot index.
     * @param[in] info Controller/file metadata consumed by the operation.
     *
     * @return `true` if cached slot; otherwise `false`.
     */
    static bool cachedSlot(FileType type, int slot, SlotInfo &info);
    /**
     * @brief Loads and caches metadata for the addressed storage slot.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] slot Zero-based project/storage slot index.
     * @param[in] info Controller/file metadata consumed by the operation.
     */
    static void cacheSlot(FileType type, int slot, const SlotInfo &info);
    /**
     * @brief Invalidates cached metadata for one storage slot.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] slot Zero-based project/storage slot index.
     */
    static void invalidateSlot(FileType type, int slot);
    /**
     * @brief Invalidates all cached storage-slot metadata.
     */
    static void invalidateAllSlots();
    /**
     * @brief Returns the next cached slot ticket.
     *
     * @return Monotonic ticket used to identify the next cached file-slot operation.
     */
    static uint32_t nextCachedSlotTicket();

    /**
     * @brief Stores and manipulates cached slot info model data.
     */
    struct CachedSlotInfo {
        uint32_t ticket = 0; ///< Monotonically assigned cache ticket used to order slot-info entries by recency.
        FileType type; ///< File type associated with the storage operation.
        uint8_t slot; ///< Zero-based storage slot described by this cache entry.
        SlotInfo info; ///< Cached user-visible metadata for the associated file slot.

        /**
         * @brief Implements operator< for this type.
         *
         * @param[in] other Other object/value used for comparison or combination.
         *
         * @return `true` if operator<; otherwise `false`.
         */
        bool operator < (const CachedSlotInfo &other) const {
            return ticket < other.ticket;
        }
    };

    /**
     * @brief Enumerates the supported volume state values.
     */
    enum VolumeState {
        Available   = (1<<0), ///< Selects the available volume state.
        Mounted     = (1<<1), ///< Selects the mounted volume state.
    };

    static uint32_t _volumeState; ///< Bit mask of current SD-volume availability and mount state.
    /**
     * @brief Next volume state check ticks scheduled or expected by this component.
     */
    static uint32_t _nextVolumeStateCheckTicks; ///< Next volume state check ticks scheduled or expected by this component.

    /**
     * @brief Fixed-capacity storage for cached slot infos.
     */
    static std::array<CachedSlotInfo, 4> _cachedSlotInfos; ///< Small fixed cache of recently queried file-slot metadata.
    static uint32_t _cachedSlotInfoTicket; ///< Next cache-recency ticket assigned to a slot-info entry.

    static TaskExecuteCallback _taskExecuteCallback; ///< Callback invoked for task execute activity.
    static TaskResultCallback _taskResultCallback; ///< Callback invoked for task result activity.
    static volatile uint32_t _taskPending; ///< Nonzero while a file-system task request is pending for the worker task.
};
