/**
 * @file ClockSetup.h
 * @brief Declares the ClockSetup component used by the sequencer model.
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

#include "core/math/Math.h"
#include "core/utils/StringBuilder.h"

#include <cstdint>

/**
 * @brief Stores and edits clock setup configuration.
 */
class ClockSetup {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        Auto = 0, ///< Selects the auto mode.
        Master, ///< Selects the master mode.
        Slave, ///< Selects the slave mode.
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
        case Mode::Auto:    return TXT_MODEL_CLOCK_MODE_AUTO;
        case Mode::Master:  return TXT_MODEL_CLOCK_MODE_MASTER;
        case Mode::Slave:   return TXT_MODEL_CLOCK_MODE_SLAVE;
        case Mode::Last:    break;
        }
        return nullptr;
    }

    /**
     * @brief Enumerates the supported shift mode values.
     */
    enum class ShiftMode : uint8_t {
        Restart, ///< Selects the restart shift mode.
        Pause, ///< Selects the pause shift mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Shifts mode name.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the shift mode name; `nullptr` when no value is available.
     */
    static const char *shiftModeName(ShiftMode mode) {
        switch (mode) {
        case ShiftMode::Restart:    return TXT_MODEL_CLOCK_SHIFT_MODE_RESTART;
        case ShiftMode::Pause:      return TXT_MODEL_CLOCK_SHIFT_MODE_PAUSE;
        case ShiftMode::Last:       break;
        }
        return nullptr;
    }

    /**
     * @brief Enumerates the supported clock input mode values.
     */
    enum class ClockInputMode : uint8_t {
        Reset = 0, ///< Selects the reset clock input mode.
        Run, ///< Selects the run clock input mode.
        StartStop, ///< Selects the start stop clock input mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for clock input mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the clock input mode name; `nullptr` when no value is available.
     */
    static const char *clockInputModeName(ClockInputMode mode) {
        switch (mode) {
        case ClockInputMode::Reset:     return TXT_MODEL_CLOCK_MODE_RESET;
        case ClockInputMode::Run:       return TXT_MODEL_CLOCK_MODE_RUN;
        case ClockInputMode::StartStop: return TXT_MODEL_CLOCK_MODE_START_STOP;
        case ClockInputMode::Last:      break;
        }
        return nullptr;
    }

    /**
     * @brief Enumerates the supported clock output mode values.
     */
    enum class ClockOutputMode : uint8_t {
        Reset = 0, ///< Selects the reset clock output mode.
        Run, ///< Selects the run clock output mode.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for clock output mode.
     *
     * @param[in] mode Mode to select.
     *
     * @return Pointer to the clock output mode name; `nullptr` when no value is available.
     */
    static const char *clockOutputModeName(ClockOutputMode mode) {
        switch (mode) {
        case ClockOutputMode::Reset:    return TXT_MODEL_CLOCK_MODE_RESET;
        case ClockOutputMode::Run:      return TXT_MODEL_CLOCK_MODE_RUN;
        case ClockOutputMode::Last:     break;
        }
        return nullptr;
    }

    //----------------------------------------
    // Properties
    //----------------------------------------

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
        mode = ModelUtils::clampedEnum(mode);
        if (mode != _mode) {
            _mode = mode;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMode(int value, int shift) {
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

    // shiftMode

    /**
     * @brief Shifts mode.
     *
     * @return Configured shift mode.
     */
    ShiftMode shiftMode() const { return _shiftMode; }
    /**
     * @brief Sets the shift mode.
     *
     * @param[in] shiftMode Shift mode consumed by `setShiftMode()`.
     */
    void setShiftMode(ShiftMode shiftMode) {
        _shiftMode = ModelUtils::clampedEnum(shiftMode);
    }

    /**
     * @brief Adjusts the shift mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editShiftMode(int value, int shift) {
        setShiftMode(ModelUtils::adjustedEnum(shiftMode(), value));
    }

    /**
     * @brief Formats the shift mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printShiftMode(StringBuilder &str) const {
        str(shiftModeName(shiftMode()));
    }

    // clockInputDivisor

    /**
     * @brief Returns the clock input divisor.
     *
     * @return Configured external-clock input divisor.
     */
    int clockInputDivisor() const { return _clockInputDivisor; }
    /**
     * @brief Sets the clock input divisor.
     *
     * @param[in] clockInputDivisor New external clock divisor in sequencer timing units to store or apply.
     */
    void setClockInputDivisor(int clockInputDivisor) {
        clockInputDivisor = clamp(clockInputDivisor, 1, 192);
        if (clockInputDivisor != _clockInputDivisor) {
            _clockInputDivisor = clockInputDivisor;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the clock input divisor from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editClockInputDivisor(int value, int shift) {
        setClockInputDivisor(ModelUtils::adjustedByDivisor(clockInputDivisor(), value, shift));
    }

    /**
     * @brief Formats the clock input divisor into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printClockInputDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, clockInputDivisor());
    }

    // clockInputMode

    /**
     * @brief Returns the clock input mode.
     *
     * @return Configured clock input mode.
     */
    ClockInputMode clockInputMode() const { return _clockInputMode; }
    /**
     * @brief Sets the clock input mode.
     *
     * @param[in] mode Mode to select.
     */
    void setClockInputMode(ClockInputMode mode) {
        mode = ModelUtils::clampedEnum(mode);
        if (mode != _clockInputMode) {
            _clockInputMode = mode;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the clock input mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editClockInputMode(int value, int shift) {
        setClockInputMode(ModelUtils::adjustedEnum(clockInputMode(), value));
    }

    /**
     * @brief Formats the clock input mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printClockInputMode(StringBuilder &str) const {
        str(clockInputModeName(clockInputMode()));
    }

    // clockOutputDivisor

    /**
     * @brief Returns the clock output divisor.
     *
     * @return Configured clock-output divisor.
     */
    int clockOutputDivisor() const { return _clockOutputDivisor; }
    /**
     * @brief Sets the clock output divisor.
     *
     * @param[in] clockOutputDivisor New clock-output divisor in sequencer timing units to store or apply.
     */
    void setClockOutputDivisor(int clockOutputDivisor) {
        clockOutputDivisor = clamp(clockOutputDivisor, 2, 192);
        if (clockOutputDivisor != _clockOutputDivisor) {
            _clockOutputDivisor = clockOutputDivisor;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the clock output divisor from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editClockOutputDivisor(int value, int shift) {
        setClockOutputDivisor(ModelUtils::adjustedByDivisor(clockOutputDivisor(), value, shift));
    }

    /**
     * @brief Formats the clock output divisor into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printClockOutputDivisor(StringBuilder &str) const {
        ModelUtils::printDivisor(str, clockOutputDivisor());
    }

    // clockOutputSwing

    /**
     * @brief Returns the clock output swing.
     *
     * @return `true` if clock output swing; otherwise `false`.
     */
    bool clockOutputSwing() const { return _clockOutputSwing; }
    /**
     * @brief Sets the clock output swing.
     *
     * @param[in] clockOutputSwing `true` to enable/set clock output swing; `false` to disable/clear it.
     */
    void setClockOutputSwing(bool clockOutputSwing) {
        if (clockOutputSwing != _clockOutputSwing) {
            _clockOutputSwing = clockOutputSwing;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the clock output swing from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editClockOutputSwing(int value, int shift) {
        setClockOutputSwing(value > 0);
    }

    /**
     * @brief Formats the clock output swing into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printClockOutputSwing(StringBuilder &str) const {
        ModelUtils::printYesNo(str, _clockOutputSwing);
    }

    // clockOutputPulse

    /**
     * @brief Returns the clock output pulse.
     *
     * @return Configured clock-output pulse width.
     */
    int clockOutputPulse() const { return _clockOutputPulse; }
    /**
     * @brief Sets the clock output pulse.
     *
     * @param[in] clockOutputPulse New clock-output pulse width in milliseconds to store or apply.
     */
    void setClockOutputPulse(int clockOutputPulse) {
        clockOutputPulse = clamp(clockOutputPulse, 1, 20);
        if (clockOutputPulse != _clockOutputPulse) {
            _clockOutputPulse = clockOutputPulse;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the clock output pulse from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editClockOutputPulse(int value, int shift) {
        setClockOutputPulse(clockOutputPulse() + value);
    }

    /**
     * @brief Formats the clock output pulse into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printClockOutputPulse(StringBuilder &str) const {
        str(TXT_MODEL_CLOCK_PULSE, clockOutputPulse());
    }

    // clockOutputMode

    /**
     * @brief Returns the clock output mode.
     *
     * @return Configured clock output mode.
     */
    ClockOutputMode clockOutputMode() const { return _clockOutputMode; }
    /**
     * @brief Sets the clock output mode.
     *
     * @param[in] mode Mode to select.
     */
    void setClockOutputMode(ClockOutputMode mode) {
        mode = ModelUtils::clampedEnum(mode);
        if (mode != _clockOutputMode) {
            _clockOutputMode = mode;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the clock output mode from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editClockOutputMode(int value, int shift) {
        setClockOutputMode(ModelUtils::adjustedEnum(clockOutputMode(), value));
    }

    /**
     * @brief Formats the clock output mode into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printClockOutputMode(StringBuilder &str) const {
        str(clockOutputModeName(clockOutputMode()));
    }

    // midiRx

    /**
     * @brief Returns the midi rx.
     *
     * @return `true` if midi rx; otherwise `false`.
     */
    bool midiRx() const { return _midiRx; }
    /**
     * @brief Sets the midi rx.
     *
     * @param[in] midiRx `true` to enable/set midi rx; `false` to disable/clear it.
     */
    void setMidiRx(bool midiRx) {
        if (midiRx != _midiRx) {
            _midiRx = midiRx;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the midi rx from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMidiRx(int value, bool shift) {
        setMidiRx(value > 0);
    }

    /**
     * @brief Formats the midi rx into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMidiRx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, midiRx());
    }

    // midiTx

    /**
     * @brief Returns the midi tx.
     *
     * @return `true` if midi tx; otherwise `false`.
     */
    bool midiTx() const { return _midiTx; }
    /**
     * @brief Sets the midi tx.
     *
     * @param[in] midiTx `true` to enable/set midi tx; `false` to disable/clear it.
     */
    void setMidiTx(bool midiTx) {
        if (midiTx != _midiTx) {
            _midiTx = midiTx;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the midi tx from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editMidiTx(int value, bool shift) {
        setMidiTx(value > 0);
    }

    /**
     * @brief Formats the midi tx into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printMidiTx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, midiTx());
    }

    // usbRx

    /**
     * @brief Returns the usb rx.
     *
     * @return `true` if usb rx; otherwise `false`.
     */
    bool usbRx() const { return _usbRx; }
    /**
     * @brief Sets the usb rx.
     *
     * @param[in] usbRx `true` to enable/set usb rx; `false` to disable/clear it.
     */
    void setUsbRx(bool usbRx) {
        if (usbRx != _usbRx) {
            _usbRx = usbRx;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the usb rx from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editUsbRx(int value, bool shift) {
        setUsbRx(value > 0);
    }

    /**
     * @brief Formats the usb rx into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printUsbRx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, usbRx());
    }

    // usbTx

    /**
     * @brief Returns the usb tx.
     *
     * @return `true` if usb tx; otherwise `false`.
     */
    bool usbTx() const { return _usbTx; }
    /**
     * @brief Sets the usb tx.
     *
     * @param[in] usbTx `true` to enable/set usb tx; `false` to disable/clear it.
     */
    void setUsbTx(bool usbTx) {
        if (usbTx != _usbTx) {
            _usbTx = usbTx;
            _dirty = true;
        }
    }

    /**
     * @brief Adjusts the usb tx from a UI edit delta.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editUsbTx(int value, bool shift) {
        setUsbTx(value > 0);
    }

    /**
     * @brief Formats the usb tx into the supplied string builder/output.
     *
     * @param[out] str String builder that receives the formatted representation.
     */
    void printUsbTx(StringBuilder &str) const {
        ModelUtils::printYesNo(str, usbTx());
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

    /**
     * @brief Reports whether dirty.
     *
     * @return `true` if dirty; otherwise `false`.
     */
    bool isDirty() const { return _dirty; }
    /**
     * @brief Clears the model dirty flag after state has been accepted/persisted.
     */
    void clearDirty() { _dirty = false; }

private:
    Mode _mode; ///< Clock operating mode selecting internal master, external slave, or automatic behavior.
    ShiftMode _shiftMode; ///< Shift-key clock-edit behavior selected by the user.
    uint8_t _clockInputDivisor; ///< External clock input divisor, constrained to 1..192 sequencer clock units.
    ClockInputMode _clockInputMode; ///< Electrical/transport interpretation mode for the external clock input.
    uint8_t _clockOutputDivisor; ///< Clock-output divisor, constrained to 2..192 sequencer clock units.
    /**
     * @brief Whether clock output swing is true in the current state.
     */
    bool _clockOutputSwing; ///< True when swing timing is enabled for the corresponding clock output.
    uint8_t _clockOutputPulse; ///< Configured clock-output pulse width in milliseconds, constrained to 1..20 ms.
    ClockOutputMode _clockOutputMode; ///< Clock-output behavior selected for the hardware clock output.
    /**
     * @brief Whether midi rx is true in the current state.
     */
    bool _midiRx; ///< True when DIN MIDI clock/input reception is enabled.
    /**
     * @brief Whether midi tx is true in the current state.
     */
    bool _midiTx; ///< True when DIN MIDI clock/output transmission is enabled.
    /**
     * @brief Whether usb rx is true in the current state.
     */
    bool _usbRx; ///< True when USB-MIDI clock/input reception is enabled.
    /**
     * @brief Whether usb tx is true in the current state.
     */
    bool _usbTx; ///< True when USB-MIDI clock/output transmission is enabled.
    /**
     * @brief Whether persistent model state has changed since it was last accepted/saved.
     */
    bool _dirty; ///< Whether persistent model state has changed since it was last accepted/saved.
};
