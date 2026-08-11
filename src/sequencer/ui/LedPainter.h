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

class LedPainter {
public:
    static void drawTrackGatesAndSelectedTrack(Leds &leds, const Engine &engine, const PlayState &playState, int selectedTrack);
    static void drawTrackGates(Leds &leds, const Engine &engine, const PlayState &playState);

    static void drawNoteSequenceGateAndCurrentStep(Leds &leds, const NoteSequence &sequence, int stepOffset, int currentStep);

    static void drawSelectedPage(Leds &leds, int page);
    static void drawSelectedSequenceSection(Leds &leds, int section);
    static void drawSelectedPattern(Leds &leds, int activePattern, int requestedPattern);
    static void drawSelectedPatterns(Leds &leds, uint16_t activePatterns, uint16_t requestedPatterns);
    static void drawSelectedQuickEditValue(Leds &leds, int index, int count);

    static void drawMutes(Leds &leds, uint8_t activeMutes, uint8_t requestedMutes);
    static void drawFills(Leds &leds, uint8_t activeFills);
    static void drawSongSlot(Leds &leds, uint16_t usedPatterns);

    static void drawMode(Leds &leds, int index, int count);
    static void drawValue(Leds &leds, int index, int count);

    static void unmaskSteps(Leds &leds);
};
