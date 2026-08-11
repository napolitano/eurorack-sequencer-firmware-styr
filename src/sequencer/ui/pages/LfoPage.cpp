/**
 * @file LfoPage.cpp
 * @brief Implements the LfoPage component used by the sequencer UI page.
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
#include "LfoPage.h"
#include "ui/painters/WindowPainter.h"
#include "ui/Key.h"
#include "ui/painters/LfoPainter.h"
#include "ui/StepSelection.h"
#include "core/utils/StringBuilder.h"
#include "../../Texts.h"
#include <algorithm>
#include <cmath>
#include "engine/NoteTrackEngine.h"
#include "engine/CurveTrackEngine.h"
#include "engine/LfoTrackEngine.h"

LfoPage::LfoPage(PageManager &manager, PageContext &context)
    : BasePage(manager, context) {
    _stepSelection.setStepCompare([this](int a, int b) {
        // Placeholder: always returns true, as LfoTrack has no per-step values yet
        // Replace with actual per-step parameter comparison if/when available
        return true;
    });
}

void LfoPage::enter() {}
void LfoPage::exit() {}

void LfoPage::draw(Canvas &canvas) {
    WindowPainter::clear(canvas);
    // Show the selected waveform as the active function in the header
    auto &lfo = _project.selectedTrack().lfoTrack();
    const char *wfName = LfoTrack::waveformName(lfo.waveform());
    if (wfName && wfName[0]) {
        WindowPainter::drawActiveFunction(canvas, wfName);
    }
    WindowPainter::drawHeader(canvas, _model, _engine, TXT_MODE_LFO);
    drawWaveform(canvas);
    drawParams(canvas);
    // The hardware has five function keys. The normal layer therefore exposes
    // Shape, Steps/Freq, Mode, Min and Max. Clip lives on Shift+F1 together
    // with the other LFO-only secondary controls (Pulse Width and Nudge).
    const bool shift = pageKeyState()[Key::Shift];
    const char *labels[ParamCount];
    if (shift) {
        labels[0] = (lfo.waveform() == LfoTrack::Waveform::Square)
            ? TXT_FUNCTION_LFO_PULSE_W
            : "~" TXT_FUNCTION_LFO_PULSE_W;
        labels[1] = TXT_FUNCTION_LFO_CLIP;
        labels[2] = "";
        labels[3] = "";

        const bool nudgeAllowed = (lfo.mode() == LfoTrack::Mode::Sync) && (
            lfo.waveform() == LfoTrack::Waveform::Sine ||
            lfo.waveform() == LfoTrack::Waveform::Triangle ||
            lfo.waveform() == LfoTrack::Waveform::RampUp ||
            lfo.waveform() == LfoTrack::Waveform::RampDown ||
            lfo.waveform() == LfoTrack::Waveform::Square);
        labels[4] = nudgeAllowed ? TXT_FUNCTION_LFO_NUDGE : "~" TXT_FUNCTION_LFO_NUDGE;
    } else {
        labels[0] = TXT_FUNCTION_LFO_SHAPE;
        labels[1] = (lfo.mode() == LfoTrack::Mode::Sync) ? TXT_FUNCTION_LFO_STEPS : TXT_FUNCTION_LFO_FREQ;
        labels[2] = TXT_FUNCTION_LFO_MODE;
        labels[3] = TXT_FUNCTION_LFO_MIN;
        labels[4] = TXT_FUNCTION_LFO_MAX;
    }
    WindowPainter::drawFooter(canvas, labels, pageKeyState(), _selectedParam);
}

void LfoPage::keyPress(KeyPressEvent &event) {
    const Key &key = event.key();
    if (key.code() >= Key::F0 && key.code() < Key::F0 + ParamCount) {
        int param = key.code() - Key::F0;
        auto &lfo = _project.selectedTrack().lfoTrack();
        bool shift = pageKeyState()[Key::Shift];

        // Handle Shift-layer special keys and double-click actions
        if (shift) {
            // Shift layer: Pulse Width (F0), Clip (F1) and Nudge (F4).
            if (param == 0) {
                // PULSE-W selectable only when waveform is Square
                if (lfo.waveform() == LfoTrack::Waveform::Square) {
                    _selectedParam = param;
                    // double-click cycle PW: 0,25,50,75,100
                    if (event.count() > 1) {
                        int cur = lfo.pulseWidth();
                        // allow 0..100 now
                        int step = 25;
                        int curPct = (cur / 25) * 25; // floor to 25
                        int next = curPct + step;
                        if (next > 100) next = 0;
                        lfo.setPulseWidth(next);
                    }
                } else {
                    // keep no selection if disabled
                    _selectedParam = -1;
                }
            } else if (param == 1) {
                // Clip is a secondary LFO control because the hardware has
                // only five function keys. A double press toggles it.
                _selectedParam = param;
                if (event.count() > 1) {
                    lfo.setClip(!lfo.clip());
                }
            } else if (param == 4) {
                _selectedParam = param;
                if (event.count() > 1) {
                    lfo.setNudge(0);
                }
            } else {
                // blank/hidden keys: do not select
                _selectedParam = -1;
            }
        } else {
            // Non-shift: normal mapping, support double-click actions
            _selectedParam = param;
            if (event.count() > 1) {
                switch (param) {
                    case 1: {
                        // Steps (Sync) or Freq (Free) double-click
                        if (lfo.mode() == LfoTrack::Mode::Sync) {
                            int cur = lfo.speed();
                            int newS = 16;
                            if (cur >= 16 && (cur % 16) == 0) {
                                newS = cur + 16;
                                if (newS > 64) newS = 16;
                            } else if (cur < 16) {
                                newS = 16;
                            } else {
                                newS = ((cur / 16) + 1) * 16;
                                if (newS > 64) newS = 16;
                            }
                            lfo.setSpeed(newS);
                        } else {
                            // Free mode: round freq to nearest 0.5 Hz (50 centiHz)
                            int centi = lfo.freeFreqCentihz();
                            int newCenti = (int)std::round(centi / 50.0f) * 50;
                            lfo.setFreeFreqCentihz(newCenti);
                        }
                        break;
                    }
                    case 3: {
                        // Min double-click: cycle 0,25,50,75,100% of the 0..-5V range
                        // i.e. values: 0V, -1.25V, -2.50V, -3.75V, -5.00V
                        float curV = lfo.low() / 100.f;
                        int curPct = int(std::round(std::abs(curV) / 5.0f * 100.f));
                        int step = (curPct / 25 + 1) % 5;
                        int newPct = step * 25;
                        float newV = - (newPct / 100.f) * 5.0f; // always negative or zero
                        int newLowCentiv = int(std::round(newV * 100.f));
                        // Ensure new low does not exceed current hi (low <= hi)
                        if (newLowCentiv > lfo.hi()) newLowCentiv = lfo.hi();
                        lfo.setLow(newLowCentiv);
                        break;
                    }
                    case 4: {
                        // Max double-click: cycle 0,25,50,75,100%
                        float curV = lfo.hi() / 100.f;
                        int sign = curV >= 0.f ? 1 : -1;
                        int curPct = int(std::round(std::abs(curV) / 5.0f * 100.f));
                        int step = (curPct / 25 + 1) % 5;
                        int newPct = step * 25;
                        float newV = sign * (newPct / 100.f * 5.0f);
                        lfo.setHi(int(std::round(newV * 100.f)));
                        break;
                    }
                    default:
                        break;
                }
            }
        }
    } else if (key.code() == Key::Left) {
        bool shift = pageKeyState()[Key::Shift];
        if (!shift) {
            _selectedParam = std::max(0, _selectedParam - 1);
        } else {
            // find previous selectable in shift layer (skip disabled/blank slots)
            int prev = _selectedParam;
            int found = -1;
            for (int i = 0; i < ParamCount; ++i) {
                prev = prev - 1;
                if (prev < 0) break;
                if (prev == 0) {
                    if (_project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Square) { found = prev; break; }
                } else if (prev == 1) {
                    found = prev;
                    break;
                } else if (prev == 4) {
                    bool nudgeAllowedLocal = (_project.selectedTrack().lfoTrack().mode() == LfoTrack::Mode::Sync) && (
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Sine ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Triangle ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::RampUp ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::RampDown ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Square);
                    if (nudgeAllowedLocal) { found = prev; break; }
                }
            }
            _selectedParam = found;
        }
    } else if (key.code() == Key::Right) {
        bool shift = pageKeyState()[Key::Shift];
        if (!shift) {
            _selectedParam = std::min(ParamCount - 1, _selectedParam + 1);
        } else {
            // find next selectable in shift layer
            int next = _selectedParam;
            int found = -1;
            for (int i = 0; i < ParamCount; ++i) {
                next = next + 1;
                if (next >= ParamCount) break;
                if (next == 0) {
                    if (_project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Square) { found = next; break; }
                } else if (next == 1) {
                    found = next;
                    break;
                } else if (next == 4) {
                    bool nudgeAllowedLocal = (_project.selectedTrack().lfoTrack().mode() == LfoTrack::Mode::Sync) && (
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Sine ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Triangle ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::RampUp ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::RampDown ||
                        _project.selectedTrack().lfoTrack().waveform() == LfoTrack::Waveform::Square);
                    if (nudgeAllowedLocal) { found = next; break; }
                }
            }
            _selectedParam = found;
        }
    }
}

void LfoPage::encoder(EncoderEvent &event) {
    // Use encoder to change parameter values. Positive value -> increase.
    if (event.value() == 0) return;
    bool shift = pageKeyState()[Key::Shift];
    int delta = event.value();
    if (_selectedParam < 0) return; // nothing selected
    editParam(_selectedParam, delta, shift, event.pressed());
    event.consume();
}

void LfoPage::editParam(int param, int delta, bool shift, bool encoderPressed) {
    auto &lfo = _project.selectedTrack().lfoTrack();
    if (shift) {
        // Shift layer: Pulse Width at F0, Clip at F1 and Nudge at F4
        if (param == 0) {
            // PULSE-W: only editable when waveform is Square
            if (lfo.waveform() == LfoTrack::Waveform::Square) {
                // When encoder is pressed, step size = 5%
                int mul = encoderPressed ? 5 : 1;
                // Pass 'false' for the model 'shift' flag so the page-level Shift
                // key (which enables this layer) does not additionally scale by 10x.
                lfo.editPulseWidth(delta * mul, false);
            }
        } else if (param == 1) {
            lfo.editClip(delta, false);
        } else if (param == 4) {
            // Step width: 1 normally, 4 when encoder is pressed.
            int mul = encoderPressed ? 4 : 1;
            // Do not pass the page Shift flag through to the model helper; use
            // 'false' so editNudge() does not apply its internal ×10 multiplier.
            lfo.editNudge(delta * mul, false);
        }
        return;
    }

    // Normal (non-shift) layer mapping: Shape, Speed, Mode, Min, Max
    switch (param) {
        case 0:
            lfo.editWaveform(delta, shift);
            break;
        case 1: {
            // Speed: in Sync this is steps; in Free this is frequency (centiHz)
            int mul = 1;
            if (encoderPressed) {
                mul = (lfo.mode() == LfoTrack::Mode::Sync) ? 4 : 10; // 4 steps or 0.1 Hz (10 centiHz)
            }
            lfo.editSpeed(delta * mul, shift);
            break;
        }
        case 2:
            lfo.editMode(delta, shift);
            break;
        case 3: {
            // Min: encoder pressed -> 0.1V steps (10 centivolt units)
            int mul = 1;
            if (shift) mul *= 10;
            if (encoderPressed) mul *= 10;
            int stepUnits = mul * delta;
            int newLow = lfo.low() + stepUnits;
            newLow = std::max(newLow, -500);
            newLow = std::min(newLow, lfo.hi());
            lfo.setLow(newLow);
            break;
        }
        case 4: {
            // Max: encoder pressed -> 0.1V steps (10 centivolt units)
            int mul = 1;
            if (shift) mul *= 10;
            if (encoderPressed) mul *= 10;
            int stepUnits = mul * delta;
            int newHi = lfo.hi() + stepUnits;
            newHi = std::min(newHi, 500);
            newHi = std::max(newHi, lfo.low());
            lfo.setHi(newHi);
            break;
        }
        default:
            break;
    }
}

void LfoPage::drawParams(Canvas &canvas) {
    int x = 80, y = 16, dy = 10;
    FixedStringBuilder<32> str;
    auto &lfo = _project.selectedTrack().lfoTrack();
    for (int i = 0; i < ParamCount; ++i) {
        str.reset();
        switch (i) {
            case 0:
                // omit waveform name textual duplicate of graphic
                str("");
                break;
            case 1: {
                // Hi in volts with two decimals
                break;
            }
            case 2: {
				// Low in volts with two decimals
                break;
            }
            case 3:
                // Mode displayed in main area; omit duplicate here
                str("");
                break;
            case 4: {
                // Speed/frequency shown in main area; omit duplicate here
                str("");
                break;
            }
        }
        canvas.setFont(Font::Tiny);
        canvas.setColor(i == _selectedParam ? UI_COLOR_ACTIVE : UI_COLOR_NORMAL);
        canvas.drawText(x, y + i * dy, (const char*)str);
    }
}

void LfoPage::drawWaveform(Canvas &canvas) {
    int x = 8, y = 15, w = 64, h = 35; // use full available height (45px)
    auto &lfo = _project.selectedTrack().lfoTrack();
    float progress = -1.f;
    // if currently selected engine is LFO, get its sequence progress and ensure
    // the engine has generated random steps so the preview can show them even
    // when playback is stopped.
    auto &selEngine = _engine.selectedTrackEngine();
    if (selEngine.trackMode() == Track::TrackMode::Lfo) {
        progress = selEngine.sequenceProgress();
    }
    // If the selected engine is an LFO, prefer its generated random table so
    // the preview matches the actual CV output in Sync mode. Ask the engine to
    // generate the table if needed so preview works without starting playback.
    const std::vector<float> *randStepsPtr = nullptr;
    if (selEngine.trackMode() == Track::TrackMode::Lfo) {
        auto &le = selEngine.as<LfoTrackEngine>();
        le.generateRandomStepsIfNeeded();
        if (!le.randomSteps().empty()) randStepsPtr = &le.randomSteps();
    }
    LfoPainter::drawWaveform(canvas, lfo, x, y, w, h, progress, randStepsPtr);

    // Draw Max/Min values to the right of the waveform
    int rx = x + w + 4;
    int rw = 56;
    canvas.setFont(Font::Tiny);
    // Max at top
    FixedStringBuilder<32> strMax;
    {
        float hiV = lfo.hi() / 100.f;
        int pct;
        if (hiV >= 0.f) {
            // percent relative to 0..+5V
            pct = int(std::round((hiV / 5.0f) * 100.f));
        } else {
            // percent relative to 0..-5V
            pct = int(std::round((hiV / -5.0f) * 100.f));
        }
        pct = std::max(0, std::min(100, pct));
        strMax(TXT_LFO_MAX_FORMAT, hiV, pct);
    }
    canvas.setColor(UI_COLOR_NORMAL);
    canvas.drawTextAligned(rx, y, rw, h, HorizontalAlign::Left, VerticalAlign::Top, (const char*)strMax);
    // Min at bottom
    FixedStringBuilder<32> strMin;
    {
        float lowV = lfo.low() / 100.f;
        int pct;
        if (lowV >= 0.f) {
            pct = int(std::round((lowV / 5.0f) * 100.f));
        } else {
            pct = int(std::round((lowV / -5.0f) * 100.f));
        }
        pct = std::max(0, std::min(100, pct));
        strMin(TXT_LFO_MIN_FORMAT, lowV, pct);
    }
    canvas.drawTextAligned(rx, y, rw, h, HorizontalAlign::Left, VerticalAlign::Bottom, (const char*)strMin);

    // If waveform is Square, show pulse width (PW:) vertically centered to the right of the preview
    if (lfo.waveform() == LfoTrack::Waveform::Square) {
        FixedStringBuilder<32> strPW;
        strPW(TXT_LFO_PW_FORMAT, lfo.pulseWidth());
        canvas.setFont(Font::Tiny);
        canvas.setColor(UI_COLOR_NORMAL);
        canvas.drawTextAligned(rx, y, rw, h, HorizontalAlign::Left, VerticalAlign::Center, (const char*)strPW);
    }

    // Secondary values are shown while their Shift function is selected so
    // the control remains self-describing in both the device UI and the manual.
    if (pageKeyState()[Key::Shift]) {
        FixedStringBuilder<32> strSecondary;
        if (_selectedParam == 1) {
            strSecondary("Clip: %s", lfo.clip() ? "On" : "Off");
        } else if (_selectedParam == 4) {
            strSecondary("Nudge: %d", lfo.nudge());
        }
        const char *secondary = strSecondary;
        if (secondary[0] != '\0') {
            canvas.setFont(Font::Tiny);
            canvas.setColor(UI_COLOR_NORMAL);
            canvas.drawTextCentered(180, 43, 64, 8, secondary);
        }
    }

    // Draw big Sync / Frequency indicator starting around x=180 center
    int bigX = 180;
    if (lfo.mode() == LfoTrack::Mode::Sync) {
        canvas.setFont(Font::Small);
        canvas.setColor(UI_COLOR_ACTIVE);
        canvas.drawTextCentered(bigX, 18, 64, 24, TXT_MENU_SYNC);

        // show number of steps below in small font
        int steps = lfo.speed();
        // if linked to a sequence engine, prefer that sequence's length
        const TrackEngine *linked = selEngine.linkedTrackEngine();
        if (linked) {
            switch (linked->trackMode()) {
            case Track::TrackMode::Note: {
                const auto &ne = linked->as<NoteTrackEngine>();
                const auto &seq = ne.sequence();
                steps = seq.lastStep() - seq.firstStep() + 1;
                break;
            }
            case Track::TrackMode::Curve: {
                const auto &ce = linked->as<CurveTrackEngine>();
                const auto &seq = ce.sequence();
                steps = seq.lastStep() - seq.firstStep() + 1;
                break;
            }
            default:
                break;
            }
        }
        canvas.setFont(Font::Tiny);
        FixedStringBuilder<32> strSteps;
        strSteps(TXT_LFO_STEPS_FORMAT, steps);
        canvas.setColor(UI_COLOR_NORMAL);
        canvas.drawTextCentered(bigX, 33, 64, 12, (const char*)strSteps);
    } else {
        // Free mode: show frequency in Hz (use fine-resolution frequence stored in centi-Hz)
        canvas.setFont(Font::Small);
        float hz = lfo.freeFreqCentihz() / 100.f;
        FixedStringBuilder<32> strHz;
        strHz(TXT_LFO_FREQ_FORMAT, hz);
        canvas.setColor(UI_COLOR_ACTIVE);
        canvas.drawTextCentered(bigX, 22, 64, 24, (const char*)strHz);
    }
}
