/**
 * @file LfoTrackListModel.h
 * @brief Declares the LfoTrackListModel component used by the sequencer UI.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "Config.h"

#include "RoutableListModel.h"

#include "model/LfoTrack.h"

/**
 * @brief Provides list data and editing behavior for lfo track.
 */
class LfoTrackListModel : public RoutableListModel {
public:
    /**
     * @brief Sets the track.
     *
     * @param[in] track Track model operated on by this component.
     */
    void setTrack(LfoTrack &track) {
        _track = &track;
    }

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

    /**
     * @brief Returns routing target.
     *
     * @param[in] row Zero-based row index.
     *
     * @return Resolved routing target identifier.
     */
    virtual Routing::Target routingTarget(int row) const override {
        switch (Item(row)) {
        case Waveform: return Routing::Target::LfoShape;
        case Hi:       return Routing::Target::LfoHi;
        case Low:      return Routing::Target::LfoLow;
        case Speed:    return Routing::Target::LfoSpeed;
        case Mode:     return Routing::Target::LfoMode;
        case Clip:     return Routing::Target::LfoClip;
        case PulseWidth: return Routing::Target::LfoPulseWidth;
        default: return Routing::Target::None;
        }
    }

private:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        Waveform, ///< Selects the waveform item.
        Hi, ///< Selects the hi item.
        Low, ///< Selects the low item.
        Mode, ///< Selects the mode item.
        Speed, ///< Selects the speed item.
        Clip, ///< Selects the clip item.
        PulseWidth, ///< Selects the pulse width item.
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
        case Waveform:   return "Shape";
        case Hi:         return "Hi";
        case Low:        return "Low";
        case Mode:       return "Mode";
        case Speed:      return "Speed";
        case Clip:       return "Clip";
        case PulseWidth: return "PW";
        case Last:       break;
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
        if (!_track) { str("-"); return; }
        switch (item) {
        case Waveform:   _track->printWaveform(str); break;
        case Hi:         _track->printHi(str); break;
        case Low:        _track->printLow(str); break;
        case Mode:       _track->printMode(str); break;
        case Speed:      _track->printSpeed(str); break;
        case Clip:       _track->printClip(str); break;
        case PulseWidth: _track->printPulseWidth(str); break;
        case Last: break;
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
        if (!_track) return;
        switch (item) {
        case Waveform:   _track->editWaveform(value, shift); break;
        case Hi:         _track->editHi(value, shift); break;
        case Low:        _track->editLow(value, shift); break;
        case Mode:       _track->editMode(value, shift); break;
        case Speed:      _track->editSpeed(value, shift); break;
        case Clip:       _track->editClip(value, shift); break;
        case PulseWidth: _track->editPulseWidth(value, shift); break;
        case Last: break;
        }
    }

    /**
     * @brief Persistent track model operated on by this component.
     */
    LfoTrack *_track = nullptr; ///< Persistent track model operated on by this component.
};

