/**
 * @file ClockSetupListModel.h
 * @brief Declares the ClockSetupListModel component used by the sequencer UI.
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

#include "ListModel.h"

#include "model/ClockSetup.h"

/**
 * @brief Provides list data and editing behavior for clock setup.
 */
class ClockSetupListModel : public ListModel {
public:
    /**
     * @brief Constructs a ClockSetupListModel instance.
     *
     * @param[in] clockSetup Clock configuration model displayed or edited by the list model.
     */
    ClockSetupListModel(ClockSetup &clockSetup) :
        /**
         * @brief Returns the clock setup.
         */
        _clockSetup(clockSetup)
    {}

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        return Last;
    }

    /**
     * @brief Returns the columns.
     *
     * @return Number of columns represented by this object.
     */
    virtual int columns() const override {
        return 2;
    }

    /**
     * @brief Returns the cell at the requested row and column.
     *
     * @param[in] row Zero-based row index.
     * @param[in] column Zero-based column index.
     * @param[out] str String builder that receives the formatted representation.
     */
    virtual void cell(int row, int column, StringBuilder &str) const override {
        if (column == 0) {
            formatName(Item(row), str);
        } else if (column == 1) {
            formatValue(Item(row), str);
        }
    }

    /**
     * @brief Applies a UI edit delta to the currently addressed value.
     *
     * @param[in] row Zero-based row index.
     * @param[in] column Zero-based column index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    virtual void edit(int row, int column, int value, bool shift) override {
        if (column == 1) {
            editValue(Item(row), value, shift);
        }
    }

private:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        Mode, ///< Selects the mode item.
        ShiftMode, ///< Selects the shift mode item.
        ClockInputDivisor, ///< Selects the clock input divisor item.
        ClockInputMode, ///< Selects the clock input mode item.
        ClockOutputDivisor, ///< Selects the clock output divisor item.
        ClockOutputSwing, ///< Selects the clock output swing item.
        ClockOutputPulse, ///< Selects the clock output pulse item.
        ClockOutputMode, ///< Selects the clock output mode item.
        MidiRx, ///< Selects the midi rx item.
        MidiTx, ///< Selects the midi tx item.
        UsbRx, ///< Selects the usb rx item.
        UsbTx, ///< Selects the usb tx item.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns the display name for item.
     *
     * @param[in] item Item or list entry addressed by the operation.
     *
     * @return Pointer to the item name; `nullptr` when no value is available.
     */
    static const char *itemName(Item item) {
        switch (item) {
        case Mode:              return TXT_LIST_LABEL_MODE;
        case ShiftMode:         return TXT_LIST_LABEL_SHIFT_MODE;
        case ClockInputDivisor: return TXT_LIST_LABEL_INPUT_DIVISOR;
        case ClockInputMode:    return TXT_LIST_LABEL_INPUT_MODE;
        case ClockOutputDivisor:return TXT_LIST_LABEL_OUTPUT_DIVISOR;
        case ClockOutputSwing:  return TXT_LIST_LABEL_OUTPUT_SWING;
        case ClockOutputPulse:  return TXT_LIST_LABEL_OUTPUT_PULSE;
        case ClockOutputMode:   return TXT_LIST_LABEL_OUTPUT_MODE;
        case MidiRx:            return TXT_LIST_LABEL_MIDI_RX;
        case MidiTx:            return TXT_LIST_LABEL_MIDI_TX;
        case UsbRx:             return TXT_LIST_LABEL_USB_RX;
        case UsbTx:             return TXT_LIST_LABEL_USB_TX;
        case Last:              break;
        }
        return nullptr;
    }

    /**
     * @brief Formats the name for display.
     *
     * @param[in] item Item or list entry addressed by the operation.
     * @param[out] str String builder that receives the formatted representation.
     */
    void formatName(Item item, StringBuilder &str) const {
        str(itemName(item));
    }

    /**
     * @brief Formats the value for display.
     *
     * @param[in] item Item or list entry addressed by the operation.
     * @param[out] str String builder that receives the formatted representation.
     */
    void formatValue(Item item, StringBuilder &str) const {
        switch (item) {
        case Mode:
            _clockSetup.printMode(str);
            break;
        case ShiftMode:
            _clockSetup.printShiftMode(str);
            break;
        case ClockInputDivisor:
            _clockSetup.printClockInputDivisor(str);
            break;
        case ClockInputMode:
            _clockSetup.printClockInputMode(str);
            break;
        case ClockOutputDivisor:
            _clockSetup.printClockOutputDivisor(str);
            break;
        case ClockOutputSwing:
            _clockSetup.printClockOutputSwing(str);
            break;
        case ClockOutputPulse:
            _clockSetup.printClockOutputPulse(str);
            break;
        case ClockOutputMode:
            _clockSetup.printClockOutputMode(str);
            break;
        case MidiRx:
            _clockSetup.printMidiRx(str);
            break;
        case MidiTx:
            _clockSetup.printMidiTx(str);
            break;
        case UsbRx:
            _clockSetup.printUsbRx(str);
            break;
        case UsbTx:
            _clockSetup.printUsbTx(str);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Adjusts the value from a UI edit delta.
     *
     * @param[in] item Item or list entry addressed by the operation.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editValue(Item item, int value, bool shift) {
        switch (item) {
        case Mode:
            _clockSetup.editMode(value, shift);
            break;
        case ShiftMode:
            _clockSetup.editShiftMode(value, shift);
            break;
        case ClockInputDivisor:
            _clockSetup.editClockInputDivisor(value, shift);
            break;
        case ClockInputMode:
            _clockSetup.editClockInputMode(value, shift);
            break;
        case ClockOutputDivisor:
            _clockSetup.editClockOutputDivisor(value, shift);
            break;
        case ClockOutputSwing:
            _clockSetup.editClockOutputSwing(value, shift);
            break;
        case ClockOutputPulse:
            _clockSetup.editClockOutputPulse(value, shift);
            break;
        case ClockOutputMode:
            _clockSetup.editClockOutputMode(value, shift);
            break;
        case MidiRx:
            _clockSetup.editMidiRx(value, shift);
            break;
        case MidiTx:
            _clockSetup.editMidiTx(value, shift);
            break;
        case UsbRx:
            _clockSetup.editUsbRx(value, shift);
            break;
        case UsbTx:
            _clockSetup.editUsbTx(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Reference to clock setup owned by another component.
     */
    ClockSetup &_clockSetup; ///< Reference to clock setup owned by another component.
};
