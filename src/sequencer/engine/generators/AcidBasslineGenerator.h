/**
 * @file AcidBasslineGenerator.h
 * @brief Declares the AcidBasslineGenerator component used by the sequencer generator.
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

#include "Generator.h"
#include "model/NoteSequence.h"

#include "core/math/Math.h"

#include <array>
#include <cstdint>

/**
 * @brief Generates scale-aware acid basslines with density, accents, slides and timing variation.
 */
class AcidBasslineGenerator : public Generator {
public:
    /**
     * @brief Enumerates the supported param values.
     */
    enum class Param {
        Seed, ///< Selects the seed param.
        RootNote, ///< Selects the root note param.
        PatternLength, ///< Selects the pattern length param.
        Density, ///< Selects the density param.
        LegatoMix, ///< Selects the legato mix param.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Stores the configurable parameters consumed by the enclosing algorithm.
     */
    struct Params {
        /**
         * @brief Runtime value representing seed.
         */
        uint16_t seed = 0; ///< Random-number generator seed/state.
        /**
         * @brief Runtime value representing root note.
         */
        uint8_t rootNote = 0; ///< Root note state used by `AcidBasslineGenerator` to preserve its current configuration or runtime progress.
        /**
         * @brief Runtime value representing pattern length.
         */
        uint8_t patternLength = 16; ///< Generated/configured pattern length in steps.
        /**
         * @brief Runtime value representing density.
         */
        uint8_t density = 62; ///< Configured/generated note/gate density.
        /**
         * @brief Runtime value representing legato mix.
         */
        uint8_t legatoMix = 35; ///< Legato weighting/mix used by the generator.
    };

    /**
     * @brief Stores timing-variation parameters used to humanize generated steps.
     */
    struct TimingFeel {
        /**
         * @brief Runtime value representing anchor.
         */
        int anchor[4]; ///< Anchor note/step used by the generator heuristic.
        /**
         * @brief Runtime value representing even.
         */
        int even[4]; ///< Score/count associated with even positions.
        /**
         * @brief Runtime value representing odd.
         */
        int odd[4]; ///< Score/count associated with odd positions.
        /**
         * @brief Whether this CV event enables or continues slide interpolation.
         */
        int slide[4]; ///< Whether this CV event enables or continues slide interpolation.
    };

    /**
     * @brief K tight feel constant used by this component.
     */
    static const TimingFeel kTightFeel; ///< Acid Bassline timing-feel preset with the tight/default timing coefficients.

    /**
     * @brief Constructs a AcidBasslineGenerator instance.
     *
     * @param[in] builder String/data builder that receives the generated representation.
     * @param[in] params Parameter set controlling the generated pattern or game object.
     */
    AcidBasslineGenerator(SequenceBuilder &builder, Params &params);

    /**
     * @brief Returns the mode.
     *
     * @return Configured mode.
     */
    Mode mode() const override { return Mode::AcidBassline; }

    /**
     * @brief Returns the param count.
     *
     * @return Number of param entries represented by the object.
     */
    int paramCount() const override;
    /**
     * @brief Returns the display name for param.
     *
     * @param[in] index Zero-based param name index.
     *
     * @return Pointer to the param name; `nullptr` when no value is available.
     */
    const char *paramName(int index) const override;
    /**
     * @brief Adjusts the param from a UI edit delta.
     *
     * @param[in] index Zero-based param index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void editParam(int index, int value, bool shift) override;
    /**
     * @brief Formats the param into the supplied string builder/output.
     *
     * @param[in] index Zero-based param index.
     * @param[out] str String builder that receives the formatted representation.
     */
    void printParam(int index, StringBuilder &str) const override;

    /**
     * @brief Initializes the AcidBasslineGenerator and its runtime resources.
     */
    void init() override;
    /**
     * @brief Updates the AcidBasslineGenerator for the current service cycle.
     */
    void update() override;

    /**
     * @brief Returns the seed.
     *
     * @return Current pseudo-random generator seed.
     */
    int seed() const { return _params.seed; }
    /**
     * @brief Sets the seed.
     *
     * @param[in] seed Seed used to initialize the pseudo-random number generator.
     */
    void setSeed(int seed) { _params.seed = clamp(seed, 0, 65535); }

    /**
     * @brief Returns the root note.
     *
     * @return Configured root-note selection.
     */
    int rootNote() const { return _params.rootNote; }
    /**
     * @brief Sets the root note.
     *
     * @param[in] rootNote New chromatic root-note index to store or apply; values are clamped to `0`..`11`.
     */
    void setRootNote(int rootNote) { _params.rootNote = clamp(rootNote, 0, 11); }

    /**
     * @brief Returns the pattern length.
     *
     * @return Configured generated-pattern length in sequence steps.
     */
    int patternLength() const { return _params.patternLength; }
    /**
     * @brief Sets the pattern length.
     *
     * @param[in] patternLength Generated pattern length in steps.
     */
    void setPatternLength(int patternLength) { _params.patternLength = clamp(patternLength, 1, CONFIG_STEP_COUNT); }

    /**
     * @brief Returns the density.
     *
     * @return Configured note/event density.
     */
    int density() const { return _params.density; }
    /**
     * @brief Sets the density.
     *
     * @param[in] density New generator density in the 0..100 range to store or apply; values are clamped to `0`..`100`.
     */
    void setDensity(int density) { _params.density = clamp(density, 0, 100); }

    /**
     * @brief Returns the legato mix.
     *
     * @return Configured proportion of generated legato transitions.
     */
    int legatoMix() const { return _params.legatoMix; }
    /**
     * @brief Sets the legato mix.
     *
     * @param[in] legatoMix New legato weighting in the 0..100 range to store or apply; values are clamped to `0`..`100`.
     */
    void setLegatoMix(int legatoMix) { _params.legatoMix = clamp(legatoMix, 0, 100); }

    /**
     * @brief Returns the pattern.
     *
     * @return Reference to the pattern.
     */
    const GeneratorPattern &pattern() const { return _pattern; }

private:
    /**
     * @brief Stores per-step generation decisions before they are committed to the target sequence.
     */
    struct StepBlueprint {
        /**
         * @brief Runtime value representing gate score.
         */
        int gateScore = 0; ///< Heuristic score for creating a gate at the current step.
        /**
         * @brief Runtime value representing cluster score.
         */
        int clusterScore = 0; ///< Heuristic score for clustered-note behavior.
        /**
         * @brief Runtime value representing repeat score.
         */
        int repeatScore = 0; ///< Heuristic score for note repetition.
        /**
         * @brief Runtime value representing approach score.
         */
        int approachScore = 0; ///< Heuristic score for approach-note behavior.
        /**
         * @brief Runtime value representing legato score.
         */
        int legatoScore = 0; ///< Heuristic score for legato behavior.
        /**
         * @brief Runtime value representing degree pick.
         */
        int degreePick = 0; ///< Random/weighted pick used to choose a scale degree.
        /**
         * @brief Runtime value representing octave pick.
         */
        int octavePick = 0; ///< Random/weighted pick used to choose an octave.
        /**
         * @brief Runtime value representing motif pick.
         */
        int motifPick = 0; ///< Random/weighted pick used to choose a motif.

        /**
         * @brief Whether base gate is true in the current state.
         */
        bool baseGate = false; ///< Unmodified gate decision produced before Acid Bassline variation is applied.
        /**
         * @brief Whether base legato is true in the current state.
         */
        bool baseLegato = false; ///< Unmodified legato decision produced before Acid Bassline variation is applied.
        /**
         * @brief Whether base approach is true in the current state.
         */
        bool baseApproach = false; ///< Unmodified approach-note decision produced before Acid Bassline variation is applied.
        /**
         * @brief Runtime value representing base degree.
         */
        int baseDegree = 0; ///< Selected/base scale degree for the generated phrase.
        /**
         * @brief Runtime value representing base octave.
         */
        int baseOctave = 0; ///< Selected/base octave for the generated phrase.
    };

    /**
     * @brief Stores step state used while the component is running.
     */
    struct StepState {
        /**
         * @brief Whether gate is true in the current state.
         */
        bool gate = false; ///< True when the generated/current step requests an active gate.
        /**
         * @brief Whether legato is true in the current state.
         */
        bool legato = false; ///< True when the generated Acid Bassline step should connect to the next note without a gate break.
        /**
         * @brief Whether this CV event enables or continues slide interpolation.
         */
        bool slide = false; ///< Whether this CV event enables or continues slide interpolation.
        /**
         * @brief Whether approach is true in the current state.
         */
        bool approach = false; ///< True when the generated Acid Bassline step is an approach note into the following pitch.

        /**
         * @brief Runtime value representing raw note.
         */
        int rawNote = 0; ///< Unquantized/unprocessed note value.
        /**
         * @brief Runtime value representing note.
         */
        int note = 0; ///< Note value represented by this state/step.
        /**
         * @brief Number of valid encoded bytes/elements.
         */
        int length = 0; ///< Number of valid encoded bytes/elements.
        /**
         * @brief Runtime value representing gate probability.
         */
        int gateProbability = 0; ///< Probability of emitting the gate.
        /**
         * @brief Offset applied to gate in this type's timing/coordinate domain.
         */
        int gateOffset = 0; ///< Offset applied to gate in this type's timing/coordinate domain.
    };

    /**
     * @brief Params, in milliseconds.
     */
    Params &_params; ///< Mutable generator parameter set supplied by the owning generator page/model.
    GeneratorPattern _pattern; ///< Generated pattern data retained between generator parameter edits and commit.
    /**
     * @brief Fixed-capacity storage for blueprint.
     */
    std::array<StepBlueprint, CONFIG_STEP_COUNT> _blueprint; ///< Per-step generation blueprint produced before notes, slides, accents, and gates are committed.
    /**
     * @brief Fixed-capacity storage for steps.
     */
    std::array<StepState, CONFIG_STEP_COUNT> _steps; ///< Per-step working state used while constructing the generated acid pattern.

    /**
     * @brief Clears state.
     */
    void clearState();
    /**
     * @brief Builds an intermediate generator phrase/sequence blueprint.
     */
    void buildBlueprint();
    /**
     * @brief Converts the generated blueprint into concrete sequence steps.
     *
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    void realizePhrase(int length);

    /**
     * @brief Applies to note sequence.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] currentLayer Currently active UI/controller layer.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    void applyToNoteSequence(NoteSequence &sequence, NoteSequence::Layer currentLayer, int length);
    /**
     * @brief Renders a preview representation of the generated sequence.
     *
     * @param[in] currentLayer Currently active UI/controller layer.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    void renderPreview(NoteSequence::Layer currentLayer, int length);

    /**
     * @brief Reports whether anchor step.
     *
     * @param[in] step Zero-based sequence-step index.
     *
     * @return `true` if anchor step; otherwise `false`.
     */
    bool isAnchorStep(int step) const;
    /**
     * @brief Reports whether should repeat previous.
     *
     * @param[in] score Score value used by the generator/game heuristic.
     * @param[in] step Zero-based sequence-step index.
     *
     * @return `true` if should repeat previous; otherwise `false`.
     */
    bool shouldRepeatPrevious(int score, int step) const;
    /**
     * @brief Reports whether should use approach.
     *
     * @param[in] score Score value used by the generator/game heuristic.
     * @param[in] step Zero-based sequence-step index.
     *
     * @return `true` if should use approach; otherwise `false`.
     */
    bool shouldUseApproach(int score, int step) const;

    /**
     * @brief Returns choose base degree.
     *
     * @param[in] pick Selected/random pick index or value.
     *
     * @return Selected base scale degree.
     */
    int chooseBaseDegree(int pick) const;
    /**
     * @brief Returns choose approach degree.
     *
     * @param[in] pick Selected/random pick index or value.
     * @param[in] targetDegree Target scale degree selected by the generator.
     *
     * @return Selected approach scale degree.
     */
    int chooseApproachDegree(int pick, int targetDegree) const;
    /**
     * @brief Returns choose octave offset.
     *
     * @param[in] pick Selected/random pick index or value.
     *
     * @return Selected octave offset.
     */
    int chooseOctaveOffset(int pick) const;

    /**
     * @brief Returns the density threshold.
     *
     * @return Threshold used to convert density into a generated step decision.
     */
    int densityThreshold() const;
    /**
     * @brief Returns the legato threshold.
     *
     * @return Threshold used to convert legato mix into a generated legato decision.
     */
    int legatoThreshold() const;

    /**
     * @brief Computes gate offset.
     *
     * @param[in] step Zero-based sequence-step index.
     *
     * @return Computed gate/microtiming offset in sequence timing units.
     */
    int computeGateOffset(int step) const;

    /**
     * @brief Returns snap note to scale.
     *
     * @param[in] note Note value supplied to the operation.
     * @param[in] scale Scale definition used for quantization or display.
     *
     * @return Input note snapped to the selected scale.
     */
    int snapNoteToScale(int note, const Scale &scale) const;
    /**
     * @brief Applies scale.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] length Number of bytes or elements covered by the operation.
     */
    void applyScale(NoteSequence &sequence, int length);

    /**
     * @brief Returns render step value.
     *
     * @param[in] layer Layer index or layer identifier.
     * @param[in] stepIndex Zero-based step index.
     * @param[in] step Zero-based sequence-step index.
     *
     * @return Rendered/encoded step value.
     */
    uint8_t renderStepValue(NoteSequence::Layer layer, int stepIndex, const StepState &step) const;
    /**
     * @brief Returns encode layer value.
     *
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] minValue Minimum value permitted by the calculation or generated range.
     * @param[in] maxValue Maximum value permitted by the calculation or generated range.
     *
     * @return Controller-layer value encoded for transmission.
     */
    static uint8_t encodeLayerValue(int value, int minValue, int maxValue);

    /**
     * @brief Reports whether the AcidBasslineGenerator has h32.
     *
     * @param[in] x Horizontal coordinate or scalar x component.
     *
     * @return 32-bit hash of the supplied value/data.
     */
    static uint32_t hash32(uint32_t x);
};
