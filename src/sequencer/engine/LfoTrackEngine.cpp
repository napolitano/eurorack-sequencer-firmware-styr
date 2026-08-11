/**
 * @file LfoTrackEngine.cpp
 * @brief Implements the LfoTrackEngine component used by the sequencer engine.
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
#include "LfoTrackEngine.h"
#include "Config.h"
#include <cmath>
#include <cstdlib>
// include other engines to query sequence divisor when linked
#include "NoteTrackEngine.h"
#include "CurveTrackEngine.h"

void LfoTrackEngine::reset() {
    _cvValue = 0.f;
    _activity = false;
    _phase = 0.f;
    // initialize RNG
    std::random_device rd;
    _rng.seed(rd());
    _randSteps.clear();
    _lastRandStepsSpeed = 0;
    _freeInitialized = false;
}

void LfoTrackEngine::generateRandomStepsIfNeeded() {
    if (_lfoTrack.mode() == LfoTrack::Mode::Sync) {
        ensureRandomSteps(false);
        return;
    }
    // Free mode: prepare a fixed 64-sample preview table, but only if not
    // already initialized. Do not regenerate on every UI draw.
    if (_freeInitialized && _randSteps.size() == 64) return;
    int steps = 64;
    _freePrevSteps.clear();
    _freeNextSteps.clear();
    _freePrevSteps.resize(steps);
    _freeNextSteps.resize(steps);
    std::uniform_real_distribution<float> dist(-1.f, 1.f);
    for (int i = 0; i < steps; ++i) {
        float v = dist(_rng);
        _freePrevSteps[i] = v;
        _freeNextSteps[i] = v;
    }
    _randSteps = _freePrevSteps; // current preview table
    _freeInitialized = true;
    _freeElapsed = 0.f;
}

void LfoTrackEngine::restart() {
    reset();
}

// Deterministic pseudo-random generator based on an integer index.
static inline float randForIndex(int idx) {
    uint32_t x = uint32_t(idx) + 0x9e3779b1u;
    x = x * 1664525u + 1013904223u;
    x ^= x >> 16;
    const uint32_t mask = 0x7FFFFFFFu;
    float r = float(x & mask) / float(mask);
    return r * 2.f - 1.f;
}

static float evalWaveform(LfoTrack::Waveform wf, float phase, const LfoTrack &t) {
    float ev = 0.f;
    switch (wf) {
        case LfoTrack::Waveform::Sine:
            ev = std::sin(phase * 2 * M_PI);
            break;
        case LfoTrack::Waveform::Triangle:
            ev = 2.f * std::abs(2.f * (phase - std::floor(phase + 0.5f))) - 1.f;
            break;
        case LfoTrack::Waveform::RampUp:
            ev = 2.f * phase - 1.f;
            break;
        case LfoTrack::Waveform::RampDown:
            ev = 1.f - 2.f * phase;
            break;
        case LfoTrack::Waveform::Square:
            ev = phase < (t.pulseWidth() / 100.f) ? 1.f : -1.f;
            break;
        case LfoTrack::Waveform::RandomHard: {
            int steps = (t.mode() == LfoTrack::Mode::Free) ? 64 : std::min(64, t.speed());
            int idx = std::min(steps - 1, int(phase * steps));
            ev = randForIndex(idx);
            break;
        }
        case LfoTrack::Waveform::RandomSoft: {
            int steps = (t.mode() == LfoTrack::Mode::Free) ? 64 : std::min(64, t.speed());
            float scaled = phase * steps;
            int idx = std::min(steps - 1, int(std::floor(scaled)));
            float frac = scaled - std::floor(scaled);
            int idxNext = (idx + 1 < steps) ? (idx + 1) : idx;
            float a = randForIndex(idx);
            float b = randForIndex(idxNext);
            // cosine interpolation for smooth transition
            float mu = (1.f - std::cos(frac * M_PI)) * 0.5f;
            ev = a * (1.f - mu) + b * mu;
            break;
        }
        case LfoTrack::Waveform::Noise:
            ev = (float(std::rand()) / RAND_MAX) * 2.f - 1.f;
            break;
        default:
            ev = 0.f;
            break;
    }

    return ev;
}

// Ensure random steps for Sync mode are generated and up-to-date. If force==true,
// regenerate unconditionally.
void LfoTrackEngine::ensureRandomSteps(bool force) {
    // Determine table length based on current mode:
    // - Sync mode: table length = min(speed, 64)
    // - Free mode: fixed table length = 64
    int steps;
    if (_lfoTrack.mode() == LfoTrack::Mode::Free) {
        steps = 64;
    } else {
        steps = std::min(64, std::max(1, _lfoTrack.speed()));
    }
    if (!force && _randSteps.size() == size_t(steps) &&
        _lastRandStepsSpeed == steps &&
        _lastRandWaveform == _lfoTrack.waveform() &&
        _lastRandHi == _lfoTrack.hi() &&
        _lastRandLow == _lfoTrack.low()) {
        return; // up-to-date
    }
    _randSteps.clear();
    _randSteps.resize(steps);
    std::uniform_real_distribution<float> dist(-1.f, 1.f);
    for (int i = 0; i < steps; ++i) _randSteps[i] = dist(_rng);
    _lastRandStepsSpeed = steps;
    _lastRandWaveform = _lfoTrack.waveform();
    _lastRandHi = _lfoTrack.hi();
    _lastRandLow = _lfoTrack.low();
}

TrackEngine::TickResult LfoTrackEngine::tick(uint32_t tick) {
    // Sync mode: compute phase from linked engine's sequence progress if available,
    // otherwise compute phase from local speed/step mapping.
    if (_lfoTrack.mode() == LfoTrack::Mode::Sync) {
        float phase = 0.f;
        if (_linkedTrackEngine) {
            // Use the linked engine's sequence progress (fraction across the sequence)
            phase = _linkedTrackEngine->sequenceProgress();
        } else {
            const int steps = _lfoTrack.speed();
            // Determine sequence divisor: prefer linked engine's sequence divisor if present
            int seqDivisor = 12; // default NoteSequence::clear() sets divisor=12
            if (_linkedTrackEngine) {
                switch (_linkedTrackEngine->trackMode()) {
                case Track::TrackMode::Note: {
                    const auto &ne = _linkedTrackEngine->as<NoteTrackEngine>();
                    seqDivisor = ne.sequence().divisor();
                    break;
                }
                case Track::TrackMode::Curve: {
                    const auto &ce = _linkedTrackEngine->as<CurveTrackEngine>();
                    seqDivisor = ce.sequence().divisor();
                    break;
                }
                default:
                    break;
                }
            }
            const int ticksPerStep = seqDivisor * (CONFIG_PPQN / CONFIG_SEQUENCE_PPQN);
            const int totalTicks = std::max(1, steps * ticksPerStep);
            phase = std::fmod(float(tick) / float(totalTicks), 1.f);
        }

        float hi = _lfoTrack.hi() / 100.f;
        float low = _lfoTrack.low() / 100.f;

        float v = 0.f;
        // Special handling for Random modes in Sync: use a generated random table.
        // Regenerate the table on each full sequence pass (wrap-around).
        if (_lfoTrack.waveform() == LfoTrack::Waveform::RandomHard || _lfoTrack.waveform() == LfoTrack::Waveform::RandomSoft) {
            bool wrapped = (phase < _phase - 1e-6f);
            ensureRandomSteps(wrapped);
            int steps = int(_randSteps.size());
            float scaled = phase * steps;
            int idx = std::min(steps - 1, int(std::floor(scaled)));
            float frac = scaled - std::floor(scaled);
            if (_lfoTrack.waveform() == LfoTrack::Waveform::RandomHard) {
                v = _randSteps[idx];
            } else {
                int idxNext = (idx + 1 < steps) ? (idx + 1) : idx;
                float a = _randSteps[idx];
                float b = _randSteps[idxNext];
                float mu = (1.f - std::cos(frac * M_PI)) * 0.5f;
                v = a * (1.f - mu) + b * mu;
            }
        } else {
            // Apply nudge (circular step shift) in Sync mode for selected waveforms
            if (_lfoTrack.nudge() != 0) {
                switch (_lfoTrack.waveform()) {
                    case LfoTrack::Waveform::Sine:
                    case LfoTrack::Waveform::Triangle:
                    case LfoTrack::Waveform::RampUp:
                    case LfoTrack::Waveform::RampDown:
                    case LfoTrack::Waveform::Square: {
                        int steps = std::min(64, _lfoTrack.speed());
                        if (steps > 0) {
                            float scaled = phase * steps;
                            int idx = std::min(steps - 1, int(std::floor(scaled)));
                            float frac = scaled - std::floor(scaled);
                            int shifted = (idx + _lfoTrack.nudge()) % steps;
                            if (shifted < 0) shifted += steps;
                            phase = (shifted + frac) / float(steps);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            float ev = evalWaveform(_lfoTrack.waveform(), phase, _lfoTrack);
            v = ev;
        }
        float value = v * (hi - low) / 2.f + (hi + low) / 2.f;
        if (_lfoTrack.clip()) {
            if (value > hi) value = hi;
            if (value < low) value = low;
        }

        _cvValue = value;
        _phase = phase;
        _activity = true;
        return TickResult::CvUpdate;
    }

    // Free mode: handled in update(dt) for continuous progress
    return TickResult::NoUpdate;
}

void LfoTrackEngine::update(float dt) {
    if (_lfoTrack.mode() == LfoTrack::Mode::Free) {
        // In Free mode use the dedicated fine-resolution frequency (centi-Hz).
        float hz = _lfoTrack.freeFreqCentihz() / 100.f;
        float prevPhase = _phase;
        _phase = std::fmod(_phase + dt * hz, 1.f);

        // DEBUG: print phase progression for diagnostics
        // printf("LfoTrackEngine::update: dt=%f prevPhase=%f newPhase=%f waveform=%d\n", dt, prevPhase, _phase, int(_lfoTrack.waveform())); fflush(stdout);

        float hi = _lfoTrack.hi() / 100.f;
        float low = _lfoTrack.low() / 100.f;

        float v = 0.f;
        if (_lfoTrack.waveform() == LfoTrack::Waveform::RandomHard || _lfoTrack.waveform() == LfoTrack::Waveform::RandomSoft) {
            // For Random modes, limit table length to 64 samples (memory constraint).
            int steps = 64;
            // Ensure table exists
            ensureRandomSteps(false);
            // Regenerate table only after a full traversal (wrap)
            bool wrapped = (_phase < prevPhase - 1e-6f);
            if (wrapped) ensureRandomSteps(true);

            // Sample current table using phase
            float scaled = _phase * steps;
            int idx = std::min(steps - 1, int(std::floor(scaled)));
            float frac = scaled - std::floor(scaled);
            if (_lfoTrack.waveform() == LfoTrack::Waveform::RandomHard) {
                v = _randSteps[idx];
            } else {
                int idxNext = (idx + 1 < steps) ? (idx + 1) : idx;
                float a = _randSteps[idx];
                float b = _randSteps[idxNext];
                float mu = (1.f - std::cos(frac * M_PI)) * 0.5f;
                v = a * (1.f - mu) + b * mu;
            }
        } else {
            v = evalWaveform(_lfoTrack.waveform(), _phase, _lfoTrack);
        }

        float value = v * (hi - low) / 2.f + (hi + low) / 2.f;
        if (_lfoTrack.clip()) {
            if (value > hi) value = hi;
            if (value < low) value = low;
        }
        _cvValue = value;
        _activity = true;
    } else {
        _activity = true; // sync mode activity is driven by tick
    }
}

bool LfoTrackEngine::activity() const {
    return _activity;
}

bool LfoTrackEngine::gateOutput(int /*index*/) const {
    return false;
}

float LfoTrackEngine::cvOutput(int /*index*/) const {
    return _cvValue;
}

