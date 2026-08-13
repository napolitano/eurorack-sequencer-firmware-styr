/**
 * @file CurveTrackListModel.h
 * @brief Declares the CurveTrackListModel component used by the sequencer UI.
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

#include "model/CurveTrack.h"

/**
 * @brief Provides list data and editing behavior for curve track.
 */
class CurveTrackListModel : public RoutableListModel {
public:
    /**
     * @brief Sets the track.
     *
     * @param[in] track Track model operated on by this component.
     */
    void setTrack(CurveTrack &track) {
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
        case Offset:
            return Routing::Target::Offset;
        case Rotate:
            return Routing::Target::Rotate;
        case ShapeProbabilityBias:
            return Routing::Target::ShapeProbabilityBias;
        case GateProbabilityBias:
            return Routing::Target::GateProbabilityBias;
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
        MuteMode, ///< Selects the mute mode item.
        SlideTime, ///< Selects the slide time item.
        Offset, ///< Selects the offset item.
        Rotate, ///< Selects the rotate item.
        ShapeProbabilityBias, ///< Selects the shape probability bias item.
        GateProbabilityBias, ///< Selects the gate probability bias item.
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
        case PlayMode:              return TXT_LIST_LABEL_PLAY_MODE;
        case FillMode:              return TXT_LIST_LABEL_FILL_MODE;
        case MuteMode:              return TXT_LIST_LABEL_MUTE_MODE;
        case SlideTime:             return TXT_LIST_LABEL_SLIDE_TIME;
        case Offset:                return TXT_LIST_LABEL_OFFSET;
        case Rotate:                return TXT_LIST_LABEL_ROTATE;
        case ShapeProbabilityBias:  return TXT_LIST_LABEL_GATE_SHAPE_PROBABILITY_BIAS;
        case GateProbabilityBias:   return TXT_LIST_LABEL_GATE_PROBABILITY_BIAS;
        case Last:                  break;
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
        case MuteMode:
            _track->printMuteMode(str);
            break;
        case SlideTime:
            _track->printSlideTime(str);
            break;
        case Offset:
            _track->printOffset(str);
            break;
        case Rotate:
            _track->printRotate(str);
            break;
        case ShapeProbabilityBias:
            _track->printShapeProbabilityBias(str);
            break;
        case GateProbabilityBias:
            _track->printGateProbabilityBias(str);
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
        case MuteMode:
            _track->editMuteMode(value, shift);
            break;
        case SlideTime:
            _track->editSlideTime(value, shift);
            break;
        case Offset:
            _track->editOffset(value, shift);
            break;
        case Rotate:
            _track->editRotate(value, shift);
            break;
        case ShapeProbabilityBias:
            _track->editShapeProbabilityBias(value, shift);
            break;
        case GateProbabilityBias:
            _track->editGateProbabilityBias(value, shift);
            break;
        case Last:
            break;
        }
    }

    /**
     * @brief Persistent track model operated on by this component.
     */
    CurveTrack *_track; ///< Persistent track model operated on by this component.
};
