/**
 * @file tools_manual_screenshots.cpp
 * @brief Generates deterministic LCD screenshots for the Styr user manual.
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
#include "core/Simulator.h"
#include "core/Debug.h"
#include "SequencerApp.h"
#include "ui/Key.h"
#include "engine/generators/Generator.h"
#include "model/Curve.h"
#include "model/Types.h"
#include "model/Routing.h"

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>

namespace {

class ScreenshotController {
public:
    using UiReadyProbe = std::function<bool()>;

    ScreenshotController(sim::Simulator &simulator, std::string outputDirectory, int screenshotScale, UiReadyProbe uiReadyProbe) :
        _simulator(simulator),
        _outputDirectory(std::move(outputDirectory)),
        _screenshotScale(screenshotScale),
        _uiReadyProbe(std::move(uiReadyProbe))
    {}

    ScreenshotController &wait(int ms = 100) {
        _simulator.wait(ms);
        return *this;
    }

    ScreenshotController &down(int key) {
        _simulator.setButton(key, true);
        return *this;
    }

    ScreenshotController &up(int key) {
        _simulator.setButton(key, false);
        return *this;
    }

    ScreenshotController &press(int key, int pre = 10, int post = 10) {
        down(key);
        wait(pre);
        up(key);
        wait(post);
        return *this;
    }

    ScreenshotController &waitUntilUiReady(const char *context, int timeoutMs = 10000) {
        for (int elapsed = 0; elapsed < timeoutMs; ++elapsed) {
            if (_uiReadyProbe && _uiReadyProbe()) {
                // StartupPage closes while drawing its final frame. Give the
                // underlying main page enough time to render a fresh LCD frame
                // before navigation or capture continues.
                wait(50);
                return *this;
            }
            wait(1);
        }

        std::ostringstream message;
        message << "timed out waiting for the sequencer UI after " << context;
        throw std::runtime_error(message.str());
    }

    ScreenshotController &requireUiReady(const char *context) {
        if (!_uiReadyProbe || !_uiReadyProbe()) {
            std::ostringstream message;
            message << "refusing documentation " << context
                    << " while the startup page is still active";
            throw std::runtime_error(message.str());
        }
        return *this;
    }

    ScreenshotController &selectPage(int key) {
        requireUiReady("page navigation");
        down(Key::Page);
        wait(10);
        press(key);
        up(Key::Page);
        wait(50);
        return *this;
    }

    ScreenshotController &downEncoder() {
        _simulator.setEncoder(true);
        return *this;
    }

    ScreenshotController &upEncoder() {
        _simulator.setEncoder(false);
        return *this;
    }

    ScreenshotController &pressEncoder(int pre = 10, int post = 10) {
        downEncoder();
        wait(pre);
        upEncoder();
        wait(post);
        return *this;
    }

    ScreenshotController &rotateEncoder(int count) {
        const int direction = count < 0 ? -1 : 1;
        for (int i = 0; i < std::abs(count); ++i) {
            _simulator.rotateEncoder(direction);
            wait(10);
        }
        return *this;
    }

    ScreenshotController &midi(int port, const MidiMessage &message) {
        _simulator.sendMidi(port, message);
        return *this;
    }

    ScreenshotController &openContextMenu() {
        down(Key::Shift).wait(10).down(Key::Page).wait(40);
        return *this;
    }

    ScreenshotController &closeContextMenu() {
        up(Key::Page).wait(10).up(Key::Shift).wait(40);
        return *this;
    }

    ScreenshotController &screenshot(const std::string &name) {
        requireUiReady("screenshot capture");
        wait(50);
        const std::string filename = _outputDirectory + "/" + name + ".png";
        _simulator.screenshot(filename, _screenshotScale);
        std::cout << "manual screenshot: " << filename << std::endl;
        return *this;
    }

    ScreenshotController &screenshotRegion(const std::string &name, int x, int y, int width, int height) {
        requireUiReady("screenshot capture");
        wait(50);
        const std::string filename = _outputDirectory + "/" + name + ".png";
        _simulator.screenshotRegion(filename, x, y, width, height, _screenshotScale);
        std::cout << "manual screenshot region: " << filename << std::endl;
        return *this;
    }

    ScreenshotController &screenshotHeld(int key, const std::string &name) {
        down(key).wait(30).screenshot(name).up(key).wait(30);
        return *this;
    }

    ScreenshotController &quickEdit(int stepKey, const std::string &name, int closeWait = 40) {
        down(Key::Page).wait(10).down(stepKey).wait(40).screenshot(name);
        up(stepKey).wait(10).up(Key::Page);
        if (closeWait > 0) {
            wait(closeWait);
        }
        return *this;
    }

    ScreenshotController &releaseAllControls() {
        for (int index = 0; index < sim::ButtonState::Count; ++index) {
            _simulator.setButton(index, false);
        }
        _simulator.setEncoder(false);
        return *this;
    }

    ScreenshotController &reboot(const char *section) {
        releaseAllControls();
        std::cout << "manual screenshot section: " << section << std::endl;
        _simulator.reboot();
        return waitUntilUiReady(section);
    }

private:
    sim::Simulator &_simulator;
    std::string _outputDirectory;
    int _screenshotScale;
    UiReadyProbe _uiReadyProbe;
};

void screenshotAssertHandler(const char *filename, int line, const char *) {
    std::ostringstream message;
    message << "firmware assertion while generating manual screenshots";
    if (filename) {
        message << " at " << filename << ":" << line;
    }
    throw std::runtime_error(message.str());
}

int parseScale(const char *value) {
    const int scale = std::atoi(value);
    if (scale < 1 || scale > 8) {
        throw std::runtime_error("screenshot scale must be an integer between 1 and 8");
    }
    return scale;
}

void prepareNoteExample(Project &project) {
    project.setSelectedTrackIndex(0);
    project.setTrackMode(0, Track::TrackMode::Note);
    project.setSelectedNoteSequenceLayer(NoteSequence::Layer::Gate);

    auto &sequence = project.track(0).noteTrack().sequence(0);
    sequence.setFirstStep(0);
    sequence.setLastStep(15);

    static const Types::Condition conditions[] = {
        Types::Condition::Off,
        Types::Condition::Fill,
        Types::Condition::NotFill,
        Types::Condition::Pre,
        Types::Condition::NotPre,
        Types::Condition::First,
        Types::Condition::NotFirst,
        Types::Condition::Loop2
    };

    for (int i = 0; i < 16; ++i) {
        auto &step = sequence.step(i);
        step.setGate((i % 4) != 3);
        step.setGateProbability((i * 3) % NoteSequence::GateProbability::Range);
        step.setGateOffset((i % 9) - 4);
        step.setSlide((i % 5) == 2);
        step.setRetrigger(i % NoteSequence::Retrigger::Range);
        step.setRetriggerProbability((i * 5) % NoteSequence::RetriggerProbability::Range);
        step.setLength((i * 2) % NoteSequence::Length::Range);
        step.setLengthVariationRange((i % 7) - 3);
        step.setLengthVariationProbability((i * 3) % NoteSequence::LengthVariationProbability::Range);
        step.setNote((i % 8) - 4);
        step.setNoteVariationRange((i % 9) - 4);
        step.setNoteVariationProbability((i * 5) % NoteSequence::NoteVariationProbability::Range);
        step.setCondition(conditions[i % (sizeof(conditions) / sizeof(conditions[0]))]);
    }
}

void prepareCurveExample(Project &project) {
    project.setSelectedTrackIndex(0);
    project.setTrackMode(0, Track::TrackMode::Curve);
    project.setSelectedCurveSequenceLayer(CurveSequence::Layer::Shape);

    auto &sequence = project.track(0).curveTrack().sequence(0);
    sequence.setFirstStep(0);
    sequence.setLastStep(15);

    const int shapeCount = std::max(1, int(Curve::Last));
    for (int i = 0; i < 16; ++i) {
        auto &step = sequence.step(i);
        step.setShape(i % shapeCount);
        step.setShapeVariation((i + 3) % shapeCount);
        step.setShapeVariationProbability((i * 2) % 9);
        const int minValue = 16 + (i * 9) % 112;
        const int maxValue = 144 + (i * 7) % 112;
        step.setMin(minValue);
        step.setMax(std::max(minValue, maxValue));
        step.setGate((i * 3) % CurveSequence::Gate::Range);
        step.setGateProbability((i * 5) % CurveSequence::GateProbability::Range);
    }
}

void openGeneratorSelect(ScreenshotController &controller) {
    controller.openContextMenu();
    controller.press(Key::F4); // Generate in the sequence-edit context menu.
    controller.closeContextMenu();
}

void openGenerator(ScreenshotController &controller, Generator::Mode mode, const std::string &selectionScreenshot) {
    openGeneratorSelect(controller);
    // GeneratorSelectPage keeps its selected row between visits. Clamp back to
    // the first row before selecting a deterministic generator mode.
    controller.rotateEncoder(-8).rotateEncoder(int(mode)).screenshot(selectionScreenshot);
    controller.pressEncoder().wait(100);
}

void captureGeneratorParameter(ScreenshotController &controller, int functionKey, const std::string &name, int delta = 0) {
    controller.down(functionKey).wait(20);
    if (delta != 0) {
        controller.rotateEncoder(delta);
    }
    controller.screenshot(name).up(functionKey).wait(30);
}

void commitGenerator(ScreenshotController &controller, const std::string &menuName, const std::string &resultName) {
    controller.openContextMenu().screenshot(menuName);
    controller.press(Key::F4).wait(50); // Commit
    controller.closeContextMenu().screenshot(resultName);
}

void captureNoteLayers(ScreenshotController &c, Project &project) {
    prepareNoteExample(project);
    c.selectPage(Key::Step0);

    struct LayerCapture {
        NoteSequence::Layer layer;
        const char *name;
    };

    static const LayerCapture captures[] = {
        { NoteSequence::Layer::Gate, "note-steps" },
        { NoteSequence::Layer::GateProbability, "note-gate-probability" },
        { NoteSequence::Layer::GateOffset, "note-gate-offset" },
        { NoteSequence::Layer::Slide, "note-slide" },
        { NoteSequence::Layer::Retrigger, "note-retrigger" },
        { NoteSequence::Layer::RetriggerProbability, "note-retrigger-probability" },
        { NoteSequence::Layer::Length, "note-length" },
        { NoteSequence::Layer::LengthVariationRange, "note-length-variation-range" },
        { NoteSequence::Layer::LengthVariationProbability, "note-length-variation-probability" },
        { NoteSequence::Layer::Note, "note-note" },
        { NoteSequence::Layer::NoteVariationRange, "note-note-variation-range" },
        { NoteSequence::Layer::NoteVariationProbability, "note-note-variation-probability" },
        { NoteSequence::Layer::Condition, "note-condition" },
    };

    for (const auto &capture : captures) {
        project.setSelectedNoteSequenceLayer(capture.layer);
        c.wait(30).screenshot(capture.name);
    }

    // Shift+function keys jump directly back to the corresponding primary
    // layer. Capture the modifier while it is physically held so the images
    // document the actual gesture rather than only the resulting state.
    struct ShiftCapture {
        NoteSequence::Layer startLayer;
        int functionKey;
        const char *name;
    };

    static const ShiftCapture shiftCaptures[] = {
        { NoteSequence::Layer::GateOffset, Key::F0, "note-shift-gate" },
        { NoteSequence::Layer::RetriggerProbability, Key::F1, "note-shift-retrigger" },
        { NoteSequence::Layer::LengthVariationProbability, Key::F2, "note-shift-length" },
        { NoteSequence::Layer::NoteVariationProbability, Key::F3, "note-shift-note" },
        { NoteSequence::Layer::NoteVariationProbability, Key::F4, "note-shift-condition" },
    };

    for (const auto &capture : shiftCaptures) {
        project.setSelectedNoteSequenceLayer(capture.startLayer);
        c.down(Key::Shift).wait(20).press(capture.functionKey).wait(20).screenshot(capture.name).up(Key::Shift).wait(30);
    }

    project.setSelectedNoteSequenceLayer(NoteSequence::Layer::Gate);
    c.openContextMenu().screenshot("note-context-menu").closeContextMenu();

    c.quickEdit(Key::Step8, "note-quick-first-step");
    c.quickEdit(Key::Step9, "note-quick-last-step");
    c.quickEdit(Key::Step10, "note-quick-run-mode");
    c.quickEdit(Key::Step11, "note-quick-divisor");
    c.quickEdit(Key::Step12, "note-quick-reset-measure");
    c.quickEdit(Key::Step13, "note-quick-scale");
    c.quickEdit(Key::Step14, "note-quick-root-note", 0);
}

void captureCurveLayers(ScreenshotController &c, Project &project) {
    prepareCurveExample(project);
    c.selectPage(Key::Step0);

    struct LayerCapture {
        CurveSequence::Layer layer;
        const char *name;
    };

    static const LayerCapture captures[] = {
        { CurveSequence::Layer::Shape, "curve-steps" },
        { CurveSequence::Layer::ShapeVariation, "curve-shape-variation" },
        { CurveSequence::Layer::ShapeVariationProbability, "curve-shape-variation-probability" },
        { CurveSequence::Layer::Min, "curve-min" },
        { CurveSequence::Layer::Max, "curve-max" },
        { CurveSequence::Layer::Gate, "curve-gate" },
        { CurveSequence::Layer::GateProbability, "curve-gate-probability" },
    };

    for (const auto &capture : captures) {
        project.setSelectedCurveSequenceLayer(capture.layer);
        c.wait(30).screenshot(capture.name);
    }

    struct ShiftCapture {
        CurveSequence::Layer startLayer;
        int functionKey;
        const char *name;
    };

    static const ShiftCapture shiftCaptures[] = {
        { CurveSequence::Layer::ShapeVariationProbability, Key::F0, "curve-shift-shape" },
        { CurveSequence::Layer::GateProbability, Key::F1, "curve-shift-min" },
        { CurveSequence::Layer::GateProbability, Key::F2, "curve-shift-max" },
        { CurveSequence::Layer::GateProbability, Key::F3, "curve-shift-gate" },
    };

    for (const auto &capture : shiftCaptures) {
        project.setSelectedCurveSequenceLayer(capture.startLayer);
        c.down(Key::Shift).wait(20).press(capture.functionKey).wait(20).screenshot(capture.name).up(Key::Shift).wait(30);
    }

    project.setSelectedCurveSequenceLayer(CurveSequence::Layer::Shape);
    c.openContextMenu().screenshot("curve-context-menu").closeContextMenu();

    c.quickEdit(Key::Step8, "curve-quick-first-step");
    c.quickEdit(Key::Step9, "curve-quick-last-step");
    c.quickEdit(Key::Step10, "curve-quick-run-mode");
    c.quickEdit(Key::Step11, "curve-quick-divisor");
    c.quickEdit(Key::Step12, "curve-quick-reset-measure");
    c.quickEdit(Key::Step13, "curve-quick-range", 0);
}

void prepareLfoDocumentationState(Project &project) {
    project.clear();
    project.setSelectedTrackIndex(0);
    project.setTrackMode(0, Track::TrackMode::Lfo);

    // Routing uses a second value lane for routable parameters. Documentation
    // captures must never inherit a previously edited route from an earlier
    // screen in the same generator run, otherwise the visible Min/Max values
    // can silently come from the routed lane instead of the base lane.
    static const Routing::Target lfoTargets[] = {
        Routing::Target::LfoHi,
        Routing::Target::LfoLow,
        Routing::Target::LfoSpeed,
        Routing::Target::LfoShape,
        Routing::Target::LfoMode,
        Routing::Target::LfoClip,
        Routing::Target::LfoPulseWidth,
    };
    for (auto target : lfoTargets) {
        Routing::setRouted(target, 0xff, false);
    }

    auto &lfo = project.track(0).lfoTrack();
    lfo.setWaveform(LfoTrack::Waveform::Sine);
    lfo.setMode(LfoTrack::Mode::Sync);
    lfo.setSpeed(16, false);
    lfo.setSpeed(16, true);
    lfo.setLow(-500, false);
    lfo.setLow(-500, true);
    lfo.setHi(500, false);
    lfo.setHi(500, true);
    lfo.setPulseWidth(50, false);
    lfo.setPulseWidth(50, true);
    lfo.setClip(false);
    lfo.setNudge(0);

    if (lfo.low() != -500 || lfo.hi() != 500) {
        throw std::runtime_error("LFO documentation range must resolve to -5.00V..+5.00V");
    }
}

void captureLfo(ScreenshotController &c, Project &project) {
    prepareLfoDocumentationState(project);
    auto &lfo = project.track(0).lfoTrack();

    c.selectPage(Key::Step0).screenshot("lfo-sync");

    // Shape is a real function state and each supported waveform receives a
    // separate deterministic documentation capture. Keep the full +/-5V
    // range for every shape so visual differences are immediately legible.
    c.press(Key::F0).screenshot("lfo-shape");
    struct WaveformCapture {
        LfoTrack::Waveform waveform;
        const char *name;
    };
    static const WaveformCapture waveformCaptures[] = {
        { LfoTrack::Waveform::Sine,       "lfo-shape-sine" },
        { LfoTrack::Waveform::Triangle,   "lfo-shape-triangle" },
        { LfoTrack::Waveform::RampUp,     "lfo-shape-ramp-up" },
        { LfoTrack::Waveform::RampDown,   "lfo-shape-ramp-down" },
        { LfoTrack::Waveform::Square,     "lfo-shape-square" },
        { LfoTrack::Waveform::RandomHard, "lfo-shape-random" },
        { LfoTrack::Waveform::RandomSoft, "lfo-shape-smoothed-random" },
        { LfoTrack::Waveform::Noise,      "lfo-shape-noise" },
    };
    for (const auto &capture : waveformCaptures) {
        lfo.setWaveform(capture.waveform);
        // LfoPainter intentionally uses std::rand() for Noise. Reset the seed
        // before that capture so documentation output remains reproducible.
        if (capture.waveform == LfoTrack::Waveform::Noise) {
            std::srand(0);
        }
        if (lfo.low() != -500 || lfo.hi() != 500) {
            throw std::runtime_error("LFO waveform capture lost the -5.00V..+5.00V range");
        }
        c.wait(30).screenshot(capture.name);
    }

    lfo.setWaveform(LfoTrack::Waveform::Sine);
    c.press(Key::F1).screenshot("lfo-steps");
    c.press(Key::F2).screenshot("lfo-mode");
    c.press(Key::F3).screenshot("lfo-min");
    c.press(Key::F4).screenshot("lfo-max");

    lfo.setMode(LfoTrack::Mode::Free);
    lfo.setFreeFreqCentihz(248, false);
    lfo.setFreeFreqCentihz(248, true);
    c.wait(30).screenshot("lfo-free");
    c.press(Key::F1).screenshot("lfo-frequency");

    lfo.setMode(LfoTrack::Mode::Sync);
    lfo.setWaveform(LfoTrack::Waveform::Square);
    lfo.setPulseWidth(25, false);
    lfo.setPulseWidth(25, true);
    lfo.setNudge(2);
    lfo.setClip(true);
    c.down(Key::Shift).wait(30).screenshot("lfo-shift");
    c.press(Key::F0).screenshot("lfo-pulse-width");
    c.press(Key::F1).screenshot("lfo-clip");
    c.press(Key::F4).screenshot("lfo-nudge");
    c.up(Key::Shift).wait(30);
}

void captureGenerators(ScreenshotController &c, Project &project) {
    // Init Layer is an immediate generator action: it has a selection state
    // and a resulting step-editor state, but no intermediate GeneratorPage.
    project.clear();
    prepareNoteExample(project);
    project.setSelectedNoteSequenceLayer(NoteSequence::Layer::Gate);
    c.selectPage(Key::Step0);
    openGeneratorSelect(c);
    c.rotateEncoder(-8).screenshot("generator-select-init-layer");
    c.pressEncoder().wait(100).screenshot("generator-init-layer-result");

    // Euclidean on the Gate layer.
    project.clear();
    prepareNoteExample(project);
    project.setSelectedNoteSequenceLayer(NoteSequence::Layer::Gate);
    c.selectPage(Key::Step0);
    openGenerator(c, Generator::Mode::Euclidean, "generator-select-euclidean");
    c.screenshot("generator-euclidean");
    captureGeneratorParameter(c, Key::F0, "generator-euclidean-steps", -4);
    captureGeneratorParameter(c, Key::F1, "generator-euclidean-beats", 1);
    captureGeneratorParameter(c, Key::F2, "generator-euclidean-offset", 2);
    commitGenerator(c, "generator-euclidean-commit-menu", "generator-euclidean-committed");

    // Random on the Note layer.
    project.clear();
    prepareNoteExample(project);
    project.setSelectedNoteSequenceLayer(NoteSequence::Layer::Note);
    c.selectPage(Key::Step0);
    openGenerator(c, Generator::Mode::Random, "generator-select-random");
    c.screenshot("generator-random");
    captureGeneratorParameter(c, Key::F0, "generator-random-seed", 7);
    captureGeneratorParameter(c, Key::F1, "generator-random-smooth", 3);
    captureGeneratorParameter(c, Key::F2, "generator-random-bias", 2);
    captureGeneratorParameter(c, Key::F3, "generator-random-scale", -3);
    commitGenerator(c, "generator-random-commit-menu", "generator-random-committed");

#ifdef CONFIG_ACID_BASS_GENERATOR
    // Acid Bassline is a Note-sequence generator; capture every parameter and
    // the actual Commit path, not only the generator-selection list.
    project.clear();
    prepareNoteExample(project);
    project.setSelectedNoteSequenceLayer(NoteSequence::Layer::Note);
    c.selectPage(Key::Step0);
    openGenerator(c, Generator::Mode::AcidBassline, "generator-select-acid-bassline");
    c.screenshot("acid-bassline-generator");
    captureGeneratorParameter(c, Key::F0, "acid-bassline-seed", 5);
    captureGeneratorParameter(c, Key::F1, "acid-bassline-root", 4);
    captureGeneratorParameter(c, Key::F2, "acid-bassline-length", -4);
    captureGeneratorParameter(c, Key::F3, "acid-bassline-density", 8);
    captureGeneratorParameter(c, Key::F4, "acid-bassline-legato", 10);
    commitGenerator(c, "acid-bassline-commit-menu", "acid-bassline-committed");
#endif
}

} // namespace

int main(int argc, char **argv) {
    try {
        std::cout << std::unitbuf;
        std::cerr << std::unitbuf;
        dbg_set_assert_handler(&screenshotAssertHandler);
        const std::string outputDirectory = argc > 1 ? argv[1] : "docs/manual/assets";
        const int screenshotScale = argc > 2 ? parseScale(argv[2]) : 3;
        const std::string requestedSection = argc > 3 ? argv[3] : "all";

        static const char *validSections[] = {
            "all", "global", "note", "curve", "midi-cv", "lfo", "generators", "song", "system"
        };
        bool sectionValid = false;
        for (const char *section : validSections) {
            if (requestedSection == section) {
                sectionValid = true;
                break;
            }
        }
        if (!sectionValid) {
            throw std::runtime_error("unknown manual screenshot section: " + requestedSection);
        }

        const auto wantsSection = [&requestedSection] (const char *section) {
            return requestedSection == "all" || requestedSection == section;
        };

        std::unique_ptr<SequencerApp> sequencer;
        sim::Simulator simulator({
            [&sequencer] () {
                sequencer.reset(new SequencerApp());
            },
            [&sequencer] () {
                sequencer.reset();
            },
            [&sequencer] () {
                sequencer->update();
            }
        });

        ScreenshotController c(
            simulator,
            outputDirectory,
            screenshotScale,
            [&sequencer] () {
                return sequencer && !sequencer->ui.simulatorStartupActive();
            }
        );
        c.waitUntilUiReady("initial boot");

        Project *project = &sequencer->model.project();

        if (wantsSection("global")) {
            std::cout << "manual screenshot section: global" << std::endl;

            // Global and project pages.
            c.down(Key::Tempo).wait(100).screenshot("tempo").up(Key::Tempo).wait(100);
            c.down(Key::Tempo).wait(100).down(Key::Right).wait(800).screenshot("tempo-nudge").up(Key::Right).up(Key::Tempo).wait(100);
            auto &clockSetup = project->clockSetup();
            clockSetup.setMode(ClockSetup::Mode::Slave);
            clockSetup.setShiftMode(ClockSetup::ShiftMode::Pause);
            clockSetup.setClockInputDivisor(12);
            clockSetup.setClockInputMode(ClockSetup::ClockInputMode::StartStop);
            clockSetup.setClockOutputDivisor(24);
            clockSetup.setClockOutputSwing(true);
            clockSetup.setClockOutputPulse(3);
            clockSetup.setClockOutputMode(ClockSetup::ClockOutputMode::Run);
            clockSetup.setMidiRx(true);
            clockSetup.setMidiTx(true);
            clockSetup.setUsbRx(false);
            clockSetup.setUsbTx(false);
            c.selectPage(Key::Tempo).screenshot("clock");
            c.selectPage(Key::Left).screenshot("overview");
            c.screenshotRegion("header", 0, 0, CONFIG_LCD_WIDTH, 10);
            c.selectPage(Key::Pattern).screenshot("pattern");
            c.selectPage(Key::Performer).screenshot("performance");
            c.selectPage(Key::Track0).screenshot("project");

            // Layout modes and examples.
            c.selectPage(Key::Track1).screenshot("layout-mode");
            c.screenshotRegion("footer", 0, CONFIG_LCD_HEIGHT - 10, CONFIG_LCD_WIDTH, 10);
            c.press(Key::F1).screenshot("layout-link");
            c.press(Key::F2).screenshot("layout-gate");
            c.press(Key::F3).screenshot("layout-cv");
            c.press(Key::F0).pressEncoder().rotateEncoder(1).screenshot("layout-mode-confirm");

            project->setTrackMode(0, Track::TrackMode::MidiCv);
            auto &midiCvTrack = project->track(0).midiCvTrack();
            midiCvTrack.setVoiceConfig(MidiCvTrack::VoiceConfig::PitchVelocity);
            midiCvTrack.setVoices(2);
            project->setCvOutputTrack(0, 0);
            project->setCvOutputTrack(1, 0);
            project->setCvOutputTrack(2, 0);
            project->setCvOutputTrack(3, 0);
            c.press(Key::F3).screenshot("layout-cv-example");
            project->clear();

            // Routing, MIDI, scales and monitoring.
            c.selectPage(Key::Track2).screenshot("routing");
            c.pressEncoder().rotateEncoder(10).screenshot("routing-edit");
            c.selectPage(Key::Track3).screenshot("midi-output");
            c.selectPage(Key::Track4).screenshot("user-scale");
            c.selectPage(Key::Step7).screenshot("monitor-cv-in");
            c.press(Key::F1).screenshot("monitor-cv-out");
            c.press(Key::F2).midi(0, MidiMessage::makeNoteOn(0, 60)).screenshot("monitor-midi");
            c.press(Key::F3).screenshot("monitor-stats");
        }

        const auto beginIsolatedSection = [&] (const char *section) {
            // In the normal CMake documentation target every complex section
            // runs in its own process. The fallback "all" mode retains the
            // historical in-process reboot path for developer convenience.
            if (requestedSection == "all") {
                c.reboot(section);
            } else {
                c.releaseAllControls();
                std::cout << "manual screenshot section: " << section << std::endl;
                c.requireUiReady("section start");
            }
            project = &sequencer->model.project();
        };

        if (wantsSection("note")) {
            beginIsolatedSection("note");
            prepareNoteExample(*project);
            c.selectPage(Key::Step2).screenshot("note-track");
            c.selectPage(Key::Step1).screenshot("note-sequence");
            captureNoteLayers(c, *project);
        }

        if (wantsSection("curve")) {
            beginIsolatedSection("curve");
            prepareCurveExample(*project);
            c.selectPage(Key::Step2).screenshot("curve-track");
            c.selectPage(Key::Step1).screenshot("curve-sequence");
            captureCurveLayers(c, *project);
        }

        if (wantsSection("midi-cv")) {
            beginIsolatedSection("midi-cv");
            project->setTrackMode(0, Track::TrackMode::MidiCv);
            auto &midiCvTrack = project->track(0).midiCvTrack();
            midiCvTrack.setVoices(4);
            midiCvTrack.setVoiceConfig(MidiCvTrack::VoiceConfig::PitchVelocityPressure);
            midiCvTrack.setNotePriority(MidiCvTrack::NotePriority::LowestNote);
            midiCvTrack.setLowNote(36);
            midiCvTrack.setHighNote(84);
            midiCvTrack.setPitchBendRange(12);
            midiCvTrack.setRetrigger(true);
            midiCvTrack.setSlideTime(35);
            midiCvTrack.setTranspose(-5);
            auto &arpeggiator = midiCvTrack.arpeggiator();
            arpeggiator.setEnabled(true);
            arpeggiator.setHold(true);
            arpeggiator.setMode(Arpeggiator::Mode::UpDown);
            arpeggiator.setDivisor(12);
            arpeggiator.setGateLength(50);
            arpeggiator.setOctaves(2);
            c.wait(30).selectPage(Key::Step2).screenshot("midi-cv-track");
        }

        if (wantsSection("lfo")) {
            beginIsolatedSection("lfo");
            captureLfo(c, *project);
        }

        if (wantsSection("generators")) {
            beginIsolatedSection("generators");
            captureGenerators(c, *project);
        }

        if (wantsSection("song")) {
            beginIsolatedSection("song");
            project->clear();
            c.selectPage(Key::Step3).screenshot("song");
            c.down(Key::F0).wait(100)
                .press(Key::Step0).press(Key::Step0).press(Key::Step0).press(Key::Step1)
                .press(Key::Step0).press(Key::Step0).press(Key::Step0).press(Key::Step2)
                .up(Key::F0).wait(100).press(Key::Play).press(Key::F4).screenshot("song-chain-example");
            c.rotateEncoder(-5).press(Key::F4).wait(1500).screenshot("song-playback");
            c.press(Key::F4).press(Key::Play).wait(100);
        }

        if (wantsSection("system")) {
            beginIsolatedSection("system");
            c.selectPage(Key::Track7).screenshot("system-confirm");
            c.press(Key::F4).screenshot("system-calibration");
            c.pressEncoder().screenshot("system-calibration-edit").pressEncoder();
            c.press(Key::F2).screenshot("system-advanced");
            c.press(Key::F3).screenshot("system-utilities");
            c.press(Key::F4).screenshot("system-update");
        }

        std::cout << "Generated manual screenshot section '" << requestedSection
                  << "' at " << screenshotScale << "x nearest-neighbour scale." << std::endl;
        return 0;
    } catch (const std::exception &error) {
        std::cerr << "manual screenshot generation failed: " << error.what() << std::endl;
        return 1;
    }
}
