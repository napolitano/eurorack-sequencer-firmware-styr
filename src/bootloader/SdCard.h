/**
 * @file SdCard.h
 * @brief Declares the SdCard component used by the bootloader.
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

#include <cstddef>
#include <cstdint>

/**
 * @brief Provides low-level SD-card initialization, command and block-I/O operations.
 */
class SdCard {
public:
    /**
     * @brief Initializes the SdCard and its runtime resources.
     */
    static void init();
    /**
     * @brief Releases runtime resources owned by the SdCard.
     */
    static void deinit();

    /**
     * @brief Returns the available.
     *
     * @return `true` if available; otherwise `false`.
     */
    static bool available();

    /**
     * @brief Reads data from the underlying source.
     *
     * @param[in] buf Buffer supplying or receiving data.
     * @param[in] sector Flash/SD-card sector index.
     * @param[in] count Number of items covered by the operation.
     *
     * @return `true` if read; otherwise `false`.
     */
    static bool read(uint8_t *buf, uint32_t sector, uint8_t count);

private:
    /**
     * @brief Enumerates the supported error values.
     */
    enum Error {
        Success, ///< Selects the success error.
        InProgress, ///< Selects the in progress error.
        Timeout, ///< Selects the timeout error.
        CRCFail, ///< Selects the crc fail error.
        Unknown, ///< Selects the unknown error.
    };

    /**
     * @brief Describes metadata associated with the enclosing component.
     */
    struct CardInfo {
        // Relative Card Address, used when sending certain commands.
        /**
         * @brief Bootloader value representing rca.
         */
        uint16_t rca = 0; ///< SD-card relative card address assigned during card initialization.
        // If ccs true then the card is a SDHC/SDXC card and uses block based
        // addressing. Otherwise it is an SDSC card and uses byte based addresing.
        /**
         * @brief Whether ccs is true in the current state.
         */
        bool ccs = false; ///< SD-card Card Capacity Status bit; true selects block addressing for high-capacity cards.
        // Size is in 512 byte blocks. Max size is 2T or 0x100000000.
        /**
         * @brief Size of the associated data in bytes/elements as defined by this type.
         */
        uint32_t size = 0; ///< Size of the associated data in bytes/elements as defined by this type.
    };

    /**
     * @brief Returns the card detect.
     *
     * @return `true` if card detect; otherwise `false`.
     */
    static bool cardDetect();

    /**
     * @brief Enables the powered state for this component.
     */
    static void powerOn();
    /**
     * @brief Disables the powered state for this component.
     */
    static void powerOff();

    /**
     * @brief Sends command.
     *
     * @param[in] cmd Command byte/value sent to the target interface.
     * @param[in] arg Opaque/user argument passed through to the callback or operation.
     */
    static void sendCommand(uint32_t cmd, uint32_t arg);
    /**
     * @brief Returns the command result.
     *
     * @return Status returned by the most recent SD-card command.
     */
    static Error commandResult();
    /**
     * @brief Sends command wait.
     *
     * @param[in] cmd Command byte/value sent to the target interface.
     * @param[in] arg Opaque/user argument passed through to the callback or operation.
     *
     * @return Result of sendCommandWait().
     */
    static Error sendCommandWait(uint32_t cmd, uint32_t arg);
    /**
     * @brief Sends command retry.
     *
     * @param[in] cmd Command byte/value sent to the target interface.
     * @param[in] arg Opaque/user argument passed through to the callback or operation.
     * @param[in] maxRetries Maximum number of retry attempts.
     *
     * @return Result of sendCommandRetry().
     */
    static Error sendCommandRetry(uint32_t cmd, uint32_t arg, int maxRetries = 5);
    /**
     * @brief Sends app command.
     *
     * @param[in] cmd Command byte/value sent to the target interface.
     * @param[in] arg Opaque/user argument passed through to the callback or operation.
     * @param[in] maxRetries Maximum number of retry attempts.
     *
     * @return Result of sendAppCommand().
     */
    static Error sendAppCommand(uint32_t cmd, uint32_t arg, int maxRetries = 5);

    /**
     * @brief Initializes card.
     *
     * @return `true` if init card; otherwise `false`.
     */
    static bool initCard();
    /**
     * @brief Returns the wait data ready.
     *
     * @return `true` if wait data ready; otherwise `false`.
     */
    static bool waitDataReady();

    /**
     * @brief Reads block.
     *
     * @param[in] address Byte address in the target flash/storage address space.
     * @param[out] buffer Buffer that supplies or receives the bytes/elements processed by the operation.
     *
     * @return `true` if read block; otherwise `false`.
     */
    static bool readBlock(uint32_t address, void *buffer);

    static bool _initialized; ///< Whether the driver/resource completed initialization successfully.
    static CardInfo _cardInfo; ///< SD-card metadata discovered during initialization.
};
