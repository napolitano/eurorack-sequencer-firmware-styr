/**
 * @file Arpeggiator.h
 * @brief Declares the Arpeggiator component used by the sequencer model.
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

#include "Config.h"
#include "Serialize.h"
#include "ModelUtils.h"

#include "core/utils/StringBuilder.h"

#include <cstdint>

/**
 * @brief Stores and manipulates arpeggiator model data.
 */
class Arpeggiator {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        PlayOrder, ///< Selects the play order mode.
        Up, ///< Selects the up mode.
        Down, ///< Selects the down mode.
        UpDown, ///< Selects the up down mode.
        DownUp, ///< Selects the down up mode.
        UpAndDown, ///< Selects the up and down mode.
        DownAndUp, ///< Selects the down and up mode.
        Converge, ///< Selects the converge mode.
        Diverge, ///< Selects the diverge mode.
        Random, ///< Selects the random mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the mode name; `nullptr` when no value is available.
     */
    static const char *modeName(Mode mode) {
        switch (mode) {
        case Mode::PlayOrder:   return TXT_MODEL_MODE_PLAY_ORDER;
        case Mode::Up:          return TXT_MODEL_MODE_UP;
        case Mode::Down:        return TXT_MODEL_MODE_DOWN;
        case Mode::UpDown:      return TXT_MODEL_MODE_UP_DOWN;
        case Mode::DownUp:      return TXT_MODEL_MODE_DOWN_UP;
        case Mode::UpAndDown:   return TXT_MODEL_MODE_UP_AND_DOWN;
        case Mode::DownAndUp:   return TXT_MODEL_MODE_DOWN_AND_UP;
        case Mode::Converge:    return TXT_MODEL_MODE_CONVERGE;
        case Mode::Diverge:     return TXT_MODEL_MODE_DIVERGE;
        case Mode::Random:      return TXT_MODEL_MODE_RANDOM;
        case Mode::Last:        break;
        }
        return nullptr;
    }

    /**
     * @brief Returns mode serialize.
     *
     * @param[in] mode Mode to select.
     *
     * @return Result of modeSerialize().
     */
    static uint8_t modeSerialize(Mode mode) {
        switch (mode) {
        case Mode::PlayOrder:   return 0;
        case Mode::Up:          return 1;
        case Mode::Down:        return 2;
        case Mode::UpDown:      return 3;
        case Mode::DownUp:      return 4;
        case Mode::UpAndDown:   return 5;
        case Mode::DownAndUp:   return 6;
        case Mode::Converge:    return 7;
        case Mode::Diverge:     return 8;
        case Mode::Random:      return 9;
        case Mode::Last:        break;
        }
        return 0;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

    // enabled

    /**
     * @brief Enables d.
     *
     * @return `true` if enabled; otherwise `false`.
     */
    bool enabled() const { return _enabled; }
    /**
     * @brief Sets the enabled.
     *
     * @param[in] enabled `true` to enable the behavior; `false` to disable it.
     */
    void setEnabled(bool enabled) {
        _enabled = enabled;
    }

    /**
     * @brief Adjusts the enabled from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editEnabled(int value, bool shift) {
        setEnabled(value > 0);
    }

    /**
     * @brief Formats the enabled into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printEnabled(StringBuilder &str) const {
        ModelUtils::printYesNo(str, enabled());
    }

    // hold

    /**
     * @brief Returns the hold.
     *
     * @return `true` if hold; otherwise `false`.
     */
    bool hold() const { return _hold; }
    /**
     * @brief Sets the hold.
     *
     * @param[in] hold `true` to enable/set hold; `false` to disable/clear it.
     */
    void setHold(bool hold) {
        _hold = hold;
    }

    /**
     * @brief Adjusts the hold from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editHold(int value, bool shift) {
        setHold(value > 0);
    }

    /**
     * @brief Formats the hold into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printHold(StringBuilder &str) const {
        ModelUtils::printYesNo(str, hold());
    }

    // mode

    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    Mode mode() const { return _mode; }
    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode) {
        _mode = ModelUtils::clampedEnum(mode);
    }

    /**
     * @brief Adjusts the mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMode(int value, bool shift) {
        setMode(ModelUtils::adjustedEnum(mode(), value));
    }

    /**
     * @brief Formats the mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMode(StringBuilder &str) const {
        str(modeName(mode()));
    }

    // divisor

    /**
     * @brief Returns the divisor.
     *
     * @return Configured clock/sequence divisor.
     */
    int divisor() const { return _divisor; }
    /**
     * @brief Sets the divisor.
     *
     * @param[in] divisor Clock or sequence divisor in the sequencer engine tick domain.
     */
    void setDivisor(int divisor) {
        _divisor = ModelUtils::clampDivisor(divisor);
    }

    /**
     * @brief Adjusts the divisor from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editDivisor(int value, bool shift) {
        setDivisor(ModelUtils::adjustedByDivisor(divisor(), value, shift));
    }

    /**
     * @brief Formats the divisor into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, divisor());
    }

    // gateLength

    /**
     * @brief Returns the gate length.
     *
     * @return Configured gate length.
     */
    int gateLength() const { return _gateLength; }
    /**
     * @brief Sets the gate length.
     *
     * @param[in] gateLength Gate length in the model-defined step-length range.
     */
    void setGateLength(int gateLength) {
        _gateLength = clamp(gateLength, 1, 100);
    }

    /**
     * @brief Adjusts the gate length from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editGateLength(int value, bool shift) {
        setGateLength(gateLength() + value * (shift ? 10 : 1));
    }

    /**
     * @brief Formats the gate length into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printGateLength(StringBuilder &str) const {
        str(TXT_MODEL_GATE_LENGTH, gateLength());
    }

    // octaves

    /**
     * @brief Returns the octaves.
     *
     * @return Configured arpeggiator octave span.
     */
    int octaves() const { return _octaves; }
    /**
     * @brief Sets the octaves.
     *
     * @param[in] octaves Number of octaves traversed by the arpeggiator.
     */
    void setOctaves(int octaves) {
        _octaves = clamp(octaves, -10, 10);
    }

    /**
     * @brief Adjusts the octaves from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editOctaves(int value, bool shift) {
        setOctaves(octaves() + value);
    }

    /**
     * @brief Formats the octaves into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printOctaves(StringBuilder &str) const {
        int value = octaves();
        if (value > 5) {
            str(TXT_MODEL_PRINT_OCTAVE_UP_DOWN, value - 5);
        } else if (value > 0) {
            str(TXT_MODEL_PRINT_OCTAVE_UP, value);
        } else if (value == 0) {
            str(TXT_MODEL_PRINT_OCTAVE_OFF);
        } else if (value >= -5) {
            str(TXT_MODEL_PRINT_OCTAVE_DOWN, -value);
        } else if (value >= -10) {
            str(TXT_MODEL_PRINT_OCTAVE_DOWN_UP, -(value + 5));
        }
    }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();

    /**
     * @brief Writes the supplied value/data to the destination.
     *
     * @param[in,out] writer Writer that receives serialized output data.
     */
    void write(VersionedSerializedWriter &writer) const;
    /**
     * @brief Reads a value/data from the source.
     *
     * @param[in] reader Reader that supplies serialized input data.
     */
    void read(VersionedSerializedReader &reader);

private:
    /**
     * @brief Whether  is enabled.
     */
    bool _enabled; ///< Whether this option/source is enabled.
    /**
     * @brief Whether hold is true in the current state.
     */
    bool _hold; ///< True when arpeggiator hold mode keeps notes active after their input keys are released.
    Mode _mode; ///< Active mode controlling the behavior of `Arpeggiator`.
    uint16_t _divisor; ///< Configured timing divisor in the owning clock/sequence domain.
    uint8_t _gateLength; ///< Configured gate length in the model-defined range.
    int8_t _octaves; ///< Configured arpeggiator octave span.
};
