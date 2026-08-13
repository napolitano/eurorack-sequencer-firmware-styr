/**
 * @file LedPainter.h
 * @brief Declares the LedPainter component used by the sequencer UI.
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

#include <cstdint>

class Leds;
class Engine;
class PlayState;
class NoteSequence;

/**
 * @brief Renders led state to the display canvas.
 */
class LedPainter {
public:
    /**
     * @brief Draws track gates and selected track.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] playState Project playback state read or modified by the operation.
     * @param[in] selectedTrack Selected track or selected-track index.
     */
    static void drawTrackGatesAndSelectedTrack(Leds &leds, const Engine &engine, const PlayState &playState, int selectedTrack);
    /**
     * @brief Draws track gates.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] playState Project playback state read or modified by the operation.
     */
    static void drawTrackGates(Leds &leds, const Engine &engine, const PlayState &playState);

    /**
     * @brief Draws note sequence gate and current step.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] stepOffset Signed step offset applied to sequence positions.
     * @param[in] currentStep Zero-based index of the current sequence step.
     */
    static void drawNoteSequenceGateAndCurrentStep(Leds &leds, const NoteSequence &sequence, int stepOffset, int currentStep);

    /**
     * @brief Draws selected page.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] page Page instance or page identifier affected by the operation.
     */
    static void drawSelectedPage(Leds &leds, int page);
    /**
     * @brief Draws selected sequence section.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] section Section index or section identifier addressed by the operation.
     */
    static void drawSelectedSequenceSection(Leds &leds, int section);
    /**
     * @brief Draws selected pattern.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] activePattern Pattern currently active for the addressed track.
     * @param[in] requestedPattern Pattern requested for the next eligible pattern change.
     */
    static void drawSelectedPattern(Leds &leds, int activePattern, int requestedPattern);
    /**
     * @brief Draws selected patterns.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] activePatterns Per-track collection of currently active patterns.
     * @param[in] requestedPatterns Per-track collection of pending pattern requests.
     */
    static void drawSelectedPatterns(Leds &leds, uint16_t activePatterns, uint16_t requestedPatterns);
    /**
     * @brief Draws selected quick edit value.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] index Zero-based draw selected quick edit index.
     * @param[in] count Number of items covered by the operation.
     */
    static void drawSelectedQuickEditValue(Leds &leds, int index, int count);

    /**
     * @brief Draws mutes.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] activeMutes Per-track bit set describing currently active mute states.
     * @param[in] requestedMutes Per-track bit set describing pending mute-state changes.
     */
    static void drawMutes(Leds &leds, uint8_t activeMutes, uint8_t requestedMutes);
    /**
     * @brief Draws fills.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] activeFills Per-track bit set describing currently active fill states.
     */
    static void drawFills(Leds &leds, uint8_t activeFills);
    /**
     * @brief Draws song slot.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] usedPatterns Bit set or collection identifying patterns referenced by the song/project.
     */
    static void drawSongSlot(Leds &leds, uint16_t usedPatterns);

    /**
     * @brief Draws mode.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] index Zero-based draw mode index.
     * @param[in] count Number of items covered by the operation.
     */
    static void drawMode(Leds &leds, int index, int count);
    /**
     * @brief Draws value.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     * @param[in] index Zero-based draw index.
     * @param[in] count Number of items covered by the operation.
     */
    static void drawValue(Leds &leds, int index, int count);

    /**
     * @brief Removes the mask from the selected sequence steps.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    static void unmaskSteps(Leds &leds);
};
