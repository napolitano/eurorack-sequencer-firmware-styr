/**
 * @file ClipBoard.h
 * @brief Declares the ClipBoard component used by the sequencer model.
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

#include "Track.h"
#include "NoteSequence.h"
#include "CurveSequence.h"
#include "Project.h"
#include "UserScale.h"

#include "core/utils/Container.h"

#include <bitset>

class ClipBoard {
public:
    typedef std::bitset<CONFIG_STEP_COUNT> SelectedSteps;

    ClipBoard(Project &project);
    ~ClipBoard();

    ClipBoard(const ClipBoard &) = delete;
    ClipBoard &operator=(const ClipBoard &) = delete;

    void clear();

    void copyTrack(const Track &track);
    void copyNoteSequence(const NoteSequence &noteSequence);
    void copyNoteSequenceSteps(const NoteSequence &noteSequence, const SelectedSteps &selectedSteps);
    void copyCurveSequence(const CurveSequence &curveSequence);
    void copyCurveSequenceSteps(const CurveSequence &curveSequence, const SelectedSteps &selectedSteps);
    void copyPattern(int patternIndex);
    void copyUserScale(const UserScale &userScale);

    void pasteTrack(Track &track) const;
    void pasteNoteSequence(NoteSequence &noteSequence) const;
    void pasteNoteSequenceSteps(NoteSequence &noteSequence, const SelectedSteps &selectedSteps) const;
    void pasteCurveSequence(CurveSequence &curveSequence) const;
    void pasteCurveSequenceSteps(CurveSequence &curveSequence, const SelectedSteps &selectedSteps) const;
    void pastePattern(int patternIndex) const;
    void pasteUserScale(UserScale &userScale) const;

    bool canPasteTrack() const;
    bool canPasteNoteSequence() const;
    bool canPasteNoteSequenceSteps() const;
    bool canPasteCurveSequence() const;
    bool canPasteCurveSequenceSteps() const;
    bool canPastePattern() const;
    bool canPasteUserScale() const;

private:
    enum class Type : uint8_t {
        None,
        Track,
        NoteSequence,
        NoteSequenceSteps,
        CurveSequence,
        CurveSequenceSteps,
        Pattern,
        UserScale,
    };

    struct NoteSequenceSteps {
        NoteSequence sequence;
        SelectedSteps selected;
    };

    struct CurveSequenceSteps {
        CurveSequence sequence;
        SelectedSteps selected;
    };

    /**
     * @brief Stores one pattern entry while preserving the lifetime of the
     *        active sequence object.
     *
     * @note NoteSequence and CurveSequence are non-trivially constructed.
     *       The active union member is therefore explicitly constructed and
     *       destroyed instead of being accessed as raw storage.
     */
    class PatternSequence {
    public:
        PatternSequence();
        ~PatternSequence();

        PatternSequence(const PatternSequence &) = delete;
        PatternSequence &operator=(const PatternSequence &) = delete;

        Track::TrackMode trackMode() const { return _trackMode; }

        void store(const NoteSequence &sequence);
        void store(const CurveSequence &sequence);
        void storeEmpty(Track::TrackMode trackMode);
        void clear();

        const NoteSequence &note() const { return _data.note; }
        const CurveSequence &curve() const { return _data.curve; }

    private:
        union Data {
            Data() {}
            ~Data() {}

            NoteSequence note;
            CurveSequence curve;
        } _data;

        Track::TrackMode _trackMode = Track::TrackMode::Last;
    };

    struct Pattern {
        PatternSequence sequences[CONFIG_TRACK_COUNT];
    };

    /**
     * @brief Destroys the object currently stored in the clipboard buffer.
     *
     * @note Container provides storage only; ClipBoard owns the active type
     *       and is responsible for ending its object lifetime before reuse.
     */
    void destroyValue();

    template<typename T>
    T &createValue(Type type) {
        destroyValue();
        T *value = _container.create<T>();
        _type = type;
        return *value;
    }

    Project &_project;
    Type _type = Type::None;
    Container<Track, NoteSequence, NoteSequenceSteps, CurveSequence, CurveSequenceSteps, Pattern, UserScale> _container;
};
