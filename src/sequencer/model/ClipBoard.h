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

/**
 * @brief Stores and manipulates clip board model data.
 */
class ClipBoard {
public:
    typedef std::bitset<CONFIG_STEP_COUNT> SelectedSteps;

    /**
     * @brief Constructs a ClipBoard instance.
     *
     * @param[in] project Project used by the operation.
     */
    ClipBoard(Project &project);
    /**
     * @brief Destroys the ClipBoard instance.
     */
    ~ClipBoard();

    /**
     * @brief Constructs a ClipBoard instance.
     *
     * @note Includes an unnamed `const ClipBoard &` input parameter as declared by the inherited/interface signature.
     */
    ClipBoard(const ClipBoard &) = delete;

    /**
     * @brief Disables copy assignment because clipboard storage owns active typed payload state.
     *
     * @return This operation is deleted and cannot be called.
     */
    ClipBoard &operator=(const ClipBoard &) = delete;

    /**
     * @brief Clears stored runtime/container state.
     */
    void clear();

    /**
     * @brief Copies track.
     *
     * @param[in] track Track model operated on by this component.
     */
    void copyTrack(const Track &track);
    /**
     * @brief Copies note sequence.
     *
     * @param[in] noteSequence Note sequence used by the operation.
     */
    void copyNoteSequence(const NoteSequence &noteSequence);
    /**
     * @brief Copies note sequence steps.
     *
     * @param[in] noteSequence Note sequence used by the operation.
     * @param[in] selectedSteps Selected steps used by the operation.
     */
    void copyNoteSequenceSteps(const NoteSequence &noteSequence, const SelectedSteps &selectedSteps);
    /**
     * @brief Copies curve sequence.
     *
     * @param[in] curveSequence Curve sequence used by the operation.
     */
    void copyCurveSequence(const CurveSequence &curveSequence);
    /**
     * @brief Copies curve sequence steps.
     *
     * @param[in] curveSequence Curve sequence used by the operation.
     * @param[in] selectedSteps Selected steps used by the operation.
     */
    void copyCurveSequenceSteps(const CurveSequence &curveSequence, const SelectedSteps &selectedSteps);
    /**
     * @brief Copies pattern.
     *
     * @param[in] patternIndex Zero-based pattern index.
     */
    void copyPattern(int patternIndex);
    /**
     * @brief Copies user scale.
     *
     * @param[in] userScale User scale used by the operation.
     */
    void copyUserScale(const UserScale &userScale);

    /**
     * @brief Pastes track.
     *
     * @param[in] track Track model operated on by this component.
     */
    void pasteTrack(Track &track) const;
    /**
     * @brief Pastes note sequence.
     *
     * @param[in] noteSequence Note sequence used by the operation.
     */
    void pasteNoteSequence(NoteSequence &noteSequence) const;
    /**
     * @brief Pastes note sequence steps.
     *
     * @param[in] noteSequence Note sequence used by the operation.
     * @param[in] selectedSteps Selected steps used by the operation.
     */
    void pasteNoteSequenceSteps(NoteSequence &noteSequence, const SelectedSteps &selectedSteps) const;
    /**
     * @brief Pastes curve sequence.
     *
     * @param[in] curveSequence Curve sequence used by the operation.
     */
    void pasteCurveSequence(CurveSequence &curveSequence) const;
    /**
     * @brief Pastes curve sequence steps.
     *
     * @param[in] curveSequence Curve sequence used by the operation.
     * @param[in] selectedSteps Selected steps used by the operation.
     */
    void pasteCurveSequenceSteps(CurveSequence &curveSequence, const SelectedSteps &selectedSteps) const;
    /**
     * @brief Pastes pattern.
     *
     * @param[in] patternIndex Zero-based pattern index.
     */
    void pastePattern(int patternIndex) const;
    /**
     * @brief Pastes user scale.
     *
     * @param[in] userScale User scale used by the operation.
     */
    void pasteUserScale(UserScale &userScale) const;

    /**
     * @brief Reports whether the ClipBoard can paste track.
     *
     * @return `true` if paste track; otherwise `false`.
     */
    bool canPasteTrack() const;
    /**
     * @brief Reports whether the ClipBoard can paste note sequence.
     *
     * @return `true` if paste note sequence; otherwise `false`.
     */
    bool canPasteNoteSequence() const;
    /**
     * @brief Reports whether the ClipBoard can paste note sequence steps.
     *
     * @return `true` if paste note sequence steps; otherwise `false`.
     */
    bool canPasteNoteSequenceSteps() const;
    /**
     * @brief Reports whether the ClipBoard can paste curve sequence.
     *
     * @return `true` if paste curve sequence; otherwise `false`.
     */
    bool canPasteCurveSequence() const;
    /**
     * @brief Reports whether the ClipBoard can paste curve sequence steps.
     *
     * @return `true` if paste curve sequence steps; otherwise `false`.
     */
    bool canPasteCurveSequenceSteps() const;
    /**
     * @brief Reports whether the ClipBoard can paste pattern.
     *
     * @return `true` if paste pattern; otherwise `false`.
     */
    bool canPastePattern() const;
    /**
     * @brief Reports whether the ClipBoard can paste user scale.
     *
     * @return `true` if paste user scale; otherwise `false`.
     */
    bool canPasteUserScale() const;

private:
    /**
     * @brief Enumerates the supported type values.
     */
    enum class Type : uint8_t {
        None, ///< Represents no selection or an invalid value.
        Track, ///< Selects the track type.
        NoteSequence, ///< Selects the note sequence type.
        NoteSequenceSteps, ///< Selects the note sequence steps type.
        CurveSequence, ///< Selects the curve sequence type.
        CurveSequenceSteps, ///< Selects the curve sequence steps type.
        Pattern, ///< Selects the pattern type.
        UserScale, ///< Selects the user scale type.
    };

    /**
     * @brief Stores and manipulates note sequence steps model data.
     */
    struct NoteSequenceSteps {
        NoteSequence sequence; ///< Currently active sequence.
        SelectedSteps selected; ///< Selected state owned by this component.
    };

    /**
     * @brief Stores and manipulates curve sequence steps model data.
     */
    struct CurveSequenceSteps {
        CurveSequence sequence; ///< Currently active sequence.
        SelectedSteps selected; ///< Selected state owned by this component.
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
        /**
         * @brief Constructs a PatternSequence instance.
         */
        PatternSequence();
        /**
         * @brief Destroys the PatternSequence instance.
         */
        ~PatternSequence();

        /**
         * @brief Constructs a PatternSequence instance.
         *
         * @note Includes an unnamed `const PatternSequence &` input parameter as declared by the inherited/interface signature.
         */
        PatternSequence(const PatternSequence &) = delete;

        /**
         * @brief Disables copy assignment because the active sequence union member has explicit lifetime management.
         *
         * @return This operation is deleted and cannot be called.
         */
        PatternSequence &operator=(const PatternSequence &) = delete;

        /**
         * @brief Returns the track mode implemented by this engine.
         *
         * @return Current track mode.
         */
        Track::TrackMode trackMode() const { return _trackMode; }

        /**
         * @brief Stores the supplied value in the PatternSequence's backing state.
         *
         * @param[in] sequence Sequence to inspect or modify.
         */
        void store(const NoteSequence &sequence);
        /**
         * @brief Stores the supplied value in the PatternSequence's backing state.
         *
         * @param[in] sequence Sequence to inspect or modify.
         */
        void store(const CurveSequence &sequence);
        /**
         * @brief Stores empty.
         *
         * @param[in] trackMode Track mode used by the operation.
         */
        void storeEmpty(Track::TrackMode trackMode);
        /**
         * @brief Clears stored runtime/container state.
         */
        void clear();

        /**
         * @brief Returns the note.
         *
         * @return Reference to the note.
         */
        const NoteSequence &note() const { return _data.note; }
        /**
         * @brief Returns the curve.
         *
         * @return Reference to the curve.
         */
        const CurveSequence &curve() const { return _data.curve; }

    private:
        /**
         * @brief Stores and manipulates data model data.
         */
        union Data {
            /**
             * @brief Constructs a Data instance.
             */
            Data() {}
            /**
             * @brief Destroys the Data instance.
             */
            ~Data() {}

            NoteSequence note; ///< Note state owned by this component.
            CurveSequence curve; ///< Curve state owned by this component.
        } _data; ///< Data state owned by this component.

        Track::TrackMode _trackMode = Track::TrackMode::Last; ///< Track mode associated with the copied clipboard payload.
    };

    /**
     * @brief Stores and manipulates pattern model data.
     */
    struct Pattern {
        PatternSequence sequences[CONFIG_TRACK_COUNT]; ///< Sequence data stored for sequences.
    };

    /**
     * @brief Destroys the object currently stored in the clipboard buffer.
     *
     * @note Container provides storage only; ClipBoard owns the active type
     *       and is responsible for ending its object lifetime before reuse.
     */
    void destroyValue();

    template<typename T>
    /**
     * @brief Computes the create value result.
     *
     * @param[in] type Type used by the operation.
     *
     * @return Reference to the create value.
     */
    T &createValue(Type type) {
        destroyValue();
        T *value = _container.create<T>();
        _type = type;
        return *value;
    }

    Project &_project; ///< Active project model used by this component.
    Type _type = Type::None; ///< Type state owned by this component.
    Container<Track, NoteSequence, NoteSequenceSteps, CurveSequence, CurveSequenceSteps, Pattern, UserScale> _container; ///< Type-erased clipboard storage for the currently copied sequencer object.
};
