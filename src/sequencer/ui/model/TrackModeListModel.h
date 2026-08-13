/**
 * @file TrackModeListModel.h
 * @brief Declares the TrackModeListModel component used by the sequencer UI.
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

#include "model/Project.h"

#include <array>

/**
 * @brief Provides list data and editing behavior for track mode.
 */
class TrackModeListModel : public ListModel {
public:
    /**
     * @brief Constructs a TrackModeListModel instance.
     *
     * @param[in] project Project model read or modified by the operation.
     */
    TrackModeListModel(Project &project) {
        fromProject(project);
    }

    /**
     * @brief Returns same as project.
     *
     * @param[in] project Project model read or modified by the operation.
     *
     * @return `true` if same as project; otherwise `false`.
     */
    bool sameAsProject(Project &project) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            if (_trackModes[i] != project.track(i).trackMode()) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Loads this UI/model state from the project model.
     *
     * @param[in] project Project model read or modified by the operation.
     */
    void fromProject(Project &project) {
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            _trackModes[i] = project.track(i).trackMode();
        }
    }

    /**
     * @brief Writes this UI/model state back to the project model.
     *
     * @param[in] project Project model read or modified by the operation.
     */
    void toProject(Project &project) {
        // Cache the new track modes because calling setTrackMode will actually
        // trigger a reload of the track setup page and reinitialize the
        // TrackModeListModel leading to only the first track with a new track
        // mode to be updated.
        Track::TrackMode newTrackModes[CONFIG_TRACK_COUNT];
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            newTrackModes[i] = _trackModes[i];
        }
        for (int i = 0; i < CONFIG_TRACK_COUNT; ++i) {
            if (newTrackModes[i] != project.track(i).trackMode()) {
                project.setTrackMode(i, newTrackModes[i]);
            }
        }
    }

    /**
     * @brief Returns the rows.
     *
     * @return Number of rows represented by this object.
     */
    virtual int rows() const override {
        return CONFIG_TRACK_COUNT;
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
            str(TXT_LIST_LABEL_TRACK, row + 1);
        } else if (column == 1) {
            str(Track::trackModeName(_trackModes[row]));
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
            _trackModes[row] = ModelUtils::adjustedEnum(_trackModes[row], value);
        }
    }

private:
    /**
     * @brief Fixed-capacity storage for track modes.
     */
    std::array<Track::TrackMode, CONFIG_TRACK_COUNT> _trackModes; ///< Editable track-mode snapshot indexed by sequencer track.
};
