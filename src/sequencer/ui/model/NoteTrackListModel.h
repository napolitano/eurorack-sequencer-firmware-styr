/**
 * @file NoteTrackListModel.h
 * @brief Declares the NoteTrackListModel component used by the sequencer UI.
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

#include "RoutableListModel.h"

#include "model/NoteTrack.h"

/**
 * @brief Provides list data and editing behavior for note track.
 */
class NoteTrackListModel : public RoutableListModel {
public:
    /**
     * @brief Sets the track.
     *
     * @param[in] track Track model operated on by this component.
     */
    void setTrack(NoteTrack &track) {
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
        case SlideTime:
            return Routing::Target::SlideTime;
        case Octave:
            return Routing::Target::Octave;
        case Transpose:
            return Routing::Target::Transpose;
        case Rotate:
            return Routing::Target::Rotate;
        case GateProbabilityBias:
            return Routing::Target::GateProbabilityBias;
        case RetriggerProbabilityBias:
            return Routing::Target::RetriggerProbabilityBias;
        case LengthBias:
            return Routing::Target::LengthBias;
        case NoteProbabilityBias:
            return Routing::Target::NoteProbabilityBias;
        default:
            return Routing::Target::None;
        }
    }

private:
    /**
     * @brief Enumerates the supported item values.
     */
    enum Item {
        PlayMode, ///< Selects the play mode item.
        FillMode, ///< Selects the fill mode item.
        FillMuted, ///< Selects the fill muted item.
        CvUpdateMode, ///< Selects the cv update mode item.
        SlideTime, ///< Selects the slide time item.
        Octave, ///< Selects the octave item.
        Transpose, ///< Selects the transpose item.
        Rotate, ///< Selects the rotate item.
        GateProbabilityBias, ///< Selects the gate probability bias item.
        RetriggerProbabilityBias, ///< Selects the retrigger probability bias item.
        LengthBias, ///< Selects the length bias item.
        NoteProbabilityBias, ///< Selects the note probability bias item.
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
        case PlayMode:  return TXT_LIST_LABEL_PLAY_MODE;
        case FillMode:  return TXT_LIST_LABEL_FILL_MODE;
        case FillMuted: return TXT_LIST_LABEL_FILL_MUTED;
        case CvUpdateMode:  return TXT_LIST_LABEL_CV_UPDATE_MODE;
        case SlideTime: return TXT_LIST_LABEL_SLIDE_TIME;
        case Octave:    return TXT_LIST_LABEL_OCTAVE;
        case Transpose: return TXT_LIST_LABEL_TRANSPOSE;
        case Rotate:    return TXT_LIST_LABEL_ROTATE;
        case GateProbabilityBias: return TXT_LIST_LABEL_GATE_PROBABILITY_BIAS;
        case RetriggerProbabilityBias: return TXT_LIST_LABEL_GATE_RETRIGGER_PROBABILITY_BIAS;
        case LengthBias: return TXT_LIST_LABEL_GATE_LENGTH_BIAS;
        case NoteProbabilityBias: return TXT_LIST_LABEL_GATE_NOTE_PROBABILITY_BIAS;
        case Last:      break;
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
        case PlayMode:
            _track->printPlayMode(str);
            break;
        case FillMode:
            _track->printFillMode(str);
            break;
        case FillMuted:
            _track->printFillMuted(str);
            break;
        case CvUpdateMode:
            _track->printCvUpdateMode(str);
            break;
        case SlideTime:
            _track->printSlideTime(str);
            break;
        case Octave:
            _track->printOctave(str);
            break;
        case Transpose:
            _track->printTranspose(str);
            break;
        case Rotate:
            _track->printRotate(str);
            break;
        case GateProbabilityBias:
            _track->printGateProbabilityBias(str);
            break;
        case RetriggerProbabilityBias:
            _track->printRetriggerProbabilityBias(str);
            break;
        case LengthBias:
            _track->printLengthBias(str);
            break;
        case NoteProbabilityBias:
            _track->printNoteProbabilityBias(str);
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
        case PlayMode:
            _track->editPlayMode(value, shift);
            break;
        case FillMode:
            _track->editFillMode(value, shift);
            break;
        case FillMuted:
            _track->editFillMuted(value, shift);
            break;
        case CvUpdateMode:
            _track->editCvUpdateMode(value, shift);
            break;
        case SlideTime:
            _track->editSlideTime(value, shift);
            break;
        case Octave:
            _track->editOctave(value, shift);
            break;
        case Transpose:
            _track->editTranspose(value, shift);
            break;
        case Rotate:
            _track->editRotate(value, shift);
            break;
        case GateProbabilityBias:
            _track->editGateProbabilityBias(value, shift);
            break;
        case RetriggerProbabilityBias:
            _track->editRetriggerProbabilityBias(value, shift);
            break;
        case LengthBias:
            _track->editLengthBias(value, shift);
            break;
        case NoteProbabilityBias:
            _track->editNoteProbabilityBias(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Persistent track model operated on by this component.
     */
    NoteTrack *_track; ///< Persistent track model operated on by this component.
};
