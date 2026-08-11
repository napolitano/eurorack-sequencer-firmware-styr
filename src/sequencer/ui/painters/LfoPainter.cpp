/**
 * @file LfoPainter.cpp
 * @brief Implements the LfoPainter component used by the sequencer UI painter.
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
#include "LfoPainter.h"
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <vector>

// Deterministic pseudo-random generator matching engine behaviour.
static inline float randForIndexPainter(int idx) {
    uint32_t x = uint32_t(idx) + 0x9e3779b1u;
    x = x * 1664525u + 1013904223u;
    x ^= x >> 16;
    const uint32_t mask = 0x7FFFFFFFu;
    float r = float(x & mask) / float(mask);
    return r * 2.f - 1.f;
}

void LfoPainter::drawWaveform(Canvas &canvas, const LfoTrack &lfoTrack, int x, int y, int w, int h, float progress, const std::vector<float> *randSteps) {
    // Visual mapping range: display within physical -5.00V..+5.00V
    const float maxRange = 5.0f;

    // Draw Min/Max (hi/low) lines first so the waveform is rendered above them
    // Compute midline used for mapping
    int midY = y + h / 2;
    int hiY = midY - int((lfoTrack.hi() / 100.f / maxRange) * (h / 2));
    int lowY = midY - int((lfoTrack.low() / 100.f / maxRange) * (h / 2));
    canvas.setColor(UI_COLOR_DIM_MORE);
    canvas.hline(x, hiY, w);
    canvas.hline(x, lowY, w);

    // Draw midline (under the waveform)
    canvas.setColor(UI_COLOR_DIM);
    canvas.hline(x, midY, w);

    // Draw waveform into a per-column buffer first. This gives us exact
    // control over which step each pixel column belongs to and allows us to
    // draw single-column vertical connectors that are neither missing nor
    // doubled in width.
    if (w <= 0) return;
    std::vector<int> plotY(w);
    std::vector<int> colStep(w, -1);

    for (int i = 0; i < w; ++i) {
        float basePhase = (w == 1) ? 0.f : float(i) / (w - 1);
        float phase = basePhase;
        // Apply 'nudge' (circular step shift) in Sync mode for selected waveforms
        if (lfoTrack.mode() == LfoTrack::Mode::Sync) {
            switch (lfoTrack.waveform()) {
                case LfoTrack::Waveform::Sine:
                case LfoTrack::Waveform::Triangle:
                case LfoTrack::Waveform::RampUp:
                case LfoTrack::Waveform::RampDown:
                case LfoTrack::Waveform::Square: {
                    int steps = std::min(64, lfoTrack.speed());
                    if (steps > 0 && lfoTrack.nudge() != 0) {
                        float scaled = basePhase * steps;
                        int idx = std::min(steps - 1, int(std::floor(scaled)));
                        float frac = scaled - std::floor(scaled);
                        int shifted = (idx + lfoTrack.nudge()) % steps;
                        if (shifted < 0) shifted += steps;
                        phase = (shifted + frac) / float(steps);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        float v = 0.f;
        int stepIdx = -1;
        switch (lfoTrack.waveform()) {
            case LfoTrack::Waveform::Sine:
                v = std::sin(phase * 2 * M_PI);
                break;
            case LfoTrack::Waveform::Triangle:
                v = 2.f * std::abs(2.f * (phase - std::floor(phase + 0.5f))) - 1.f;
                break;
            case LfoTrack::Waveform::RampUp:
                v = 2.f * phase - 1.f;
                break;
            case LfoTrack::Waveform::RampDown:
                v = 1.f - 2.f * phase;
                break;
            case LfoTrack::Waveform::Square:
                v = (phase < lfoTrack.pulseWidth() / 100.f) ? 1.f : -1.f;
                break;
            case LfoTrack::Waveform::RandomHard: {
                int steps = (lfoTrack.mode() == LfoTrack::Mode::Free) ? 64 : std::min(64, lfoTrack.speed());
                int idx = std::min(steps - 1, int(phase * steps));
                stepIdx = idx;
                if (randSteps && int(randSteps->size()) > 0) {
                    int ri = std::min(int(randSteps->size()) - 1, idx);
                    v = (*randSteps)[ri];
                } else {
                    v = randForIndexPainter(idx);
                }
                break;
            }
            case LfoTrack::Waveform::RandomSoft: {
                int steps = (lfoTrack.mode() == LfoTrack::Mode::Free) ? 64 : std::min(64, lfoTrack.speed());
                float scaled = phase * steps;
                int idx = std::min(steps - 1, int(std::floor(scaled)));
                float frac = scaled - std::floor(scaled);
                int idxNext = (idx + 1 < steps) ? (idx + 1) : idx;
                float a, b;
                if (randSteps && int(randSteps->size()) > 0) {
                    int ri = std::min(int(randSteps->size()) - 1, idx);
                    int rj = std::min(int(randSteps->size()) - 1, idxNext);
                    a = (*randSteps)[ri];
                    b = (*randSteps)[rj];
                } else {
                    a = randForIndexPainter(idx);
                    b = randForIndexPainter(idxNext);
                }
                float mu = (1.f - std::cos(frac * M_PI)) * 0.5f;
                v = a * (1.f - mu) + b * mu;
                break;
            }
            case LfoTrack::Waveform::Noise:
                v = (std::rand() % 2000 - 1000) / 1000.f;
                break;
            default:
                v = 0.f;
        }
        colStep[i] = stepIdx;
        float hi = lfoTrack.hi() / 100.f;
        float low = lfoTrack.low() / 100.f;
        float valueV = v * (hi - low) / 2.f + (hi + low) / 2.f;
        float norm = std::max(-1.f, std::min(1.f, valueV / maxRange));
        plotY[i] = midY - int(norm * (h / 2));
    }

    // Render using the populated column buffer. RandomHard requires special
    // stair-step drawing with single-column vertical connectors between
    // different steps. Other waveforms draw a continuous polyline.
    canvas.setColor(UI_COLOR_ACTIVE);
    if (lfoTrack.waveform() == LfoTrack::Waveform::RandomHard) {
        int i = 0;
        while (i < w) {
            int s = colStep[i];
            int j = i + 1;
            while (j < w && colStep[j] == s) ++j;
            int startX = x + i;
            int endX = x + j - 1;
            // horizontal segment for this run
            canvas.hline(startX, plotY[i], endX - startX + 1);
            // vertical connector at the boundary to previous run
            if (i > 0) {
                int vx = startX; // connect at the first column of this run
                int prevY = plotY[i - 1];
                int curY = plotY[i];
                int topY = std::min(prevY, curY);
                int height = std::abs(curY - prevY) + 1;
                if (vx < x) vx = x;
                if (vx > x + w - 1) vx = x + w - 1;
                canvas.vline(vx, topY, height);
            }
            i = j;
        }
    } else {
        for (int i = 1; i < w; ++i) {
            canvas.line(x + i - 1, plotY[i - 1], x + i, plotY[i]);
        }
    }

    // Draw progress marker if provided. For Free mode + Noise waveform the
    // playhead doesn't make sense (noise has no phase continuity), so skip it.
    if (progress >= 0.f && progress <= 1.f) {
        if (!(lfoTrack.mode() == LfoTrack::Mode::Free && lfoTrack.waveform() == LfoTrack::Waveform::Noise)) {
            int px = x + int(progress * (w - 1));
            canvas.setColor(UI_COLOR_ACTIVE);
            canvas.vline(px, y, h);
        }
    }
}
