/**
 * @file Dac.h
 * @brief Declares the Dac component used by the STM32 sequencer driver.
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

#include "SystemConfig.h"

#include <cstdint>
#include <cstdlib>

/**
 * @brief Provides the dac hardware/platform abstraction.
 */
class Dac {
public:
    /**
     * @brief Enumerates the supported type values.
     */
    enum class Type {
        DAC8568C, ///< Selects the dac8568 c type.
        DAC8568A ///< Selects the dac8568 a type.
    };

    /**
     * @brief Channels constant used by this component.
     */
    static constexpr int Channels = CONFIG_DAC_CHANNELS; ///< Number of physical DAC output channels configured for this hardware.

    typedef uint16_t Value;

    /**
     * @brief Constructs a Dac instance.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     */
    Dac(Type type = Type::DAC8568C);

    /**
     * @brief Initializes the Dac and its runtime resources.
     */
    void init();

    /**
     * @brief Sets the value.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     * @param[in] value New value to store or apply.
     */
    void setValue(int channel, Value value) {
        _values[channel] = value;
    }

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in] channel MIDI or logical channel selected for the operation.
     */
    void write(int channel);
    /**
     * @brief Writes the supplied value/data to the destination.
     */
    void write();

private:
    /**
     * @brief Writes dac.
     *
     * @param[in] command DAC command word sent to the converter.
     * @param[in] address Byte address in the target flash/storage address space.
     * @param[in] data Input data to read, decode, copy, or process.
     * @param[in] function Function selector or callback consumed by the operation.
     */
    void writeDac(uint8_t command, uint8_t address, uint16_t data, uint8_t function);

    /**
     * @brief Resets the Dac to its initial runtime state.
     */
    void reset();
    /**
     * @brief Sets the internal ref.
     *
     * @param[in] enabled `true` to enable the behavior; `false` to disable it.
     */
    void setInternalRef(bool enabled);

    /**
     * @brief Enumerates the supported clear code values.
     */
    enum ClearCode {
        ClearZeroScale  = 0, ///< Selects the clear zero scale clear code.
        ClearMidScale   = 1, ///< Selects the clear mid scale clear code.
        ClearFullScale  = 2, ///< Selects the clear full scale clear code.
        ClearIgnore     = 3, ///< Selects the clear ignore clear code.
    };

    /**
     * @brief Sets the clear code.
     *
     * @param[in] code Status, key or command code interpreted by the owning subsystem.
     */
    void setClearCode(ClearCode code);

    Value _values[Channels]; ///< Owned fixed-size collection of values.
    /**
     * @brief Driver value representing data shift.
     */
    uint32_t _dataShift = 0; ///< Bit shift applied when packing DAC channel data into the transfer word.
};
