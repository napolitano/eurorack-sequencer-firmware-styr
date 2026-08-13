/**
 * @file LaunchpadController.h
 * @brief Declares the LaunchpadController component used by the sequencer controller.
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

#include "LaunchpadDevice.h"
#include "LaunchpadMk2Device.h"
#include "LaunchpadMk3Device.h"
#include "LaunchpadProDevice.h"
#include "LaunchpadProMk3Device.h"

#include "ui/Controller.h"

#include "core/utils/Container.h"

/**
 * @brief Implements launchpad controller operations and maintains the associated runtime state.
 */
class LaunchpadController : public Controller {
public:
    /**
     * @brief Constructs a LaunchpadController instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] info Controller/file metadata consumed by the operation.
     */
    LaunchpadController(ControllerManager &manager, Model &model, Engine &engine, const ControllerInfo &info);
    /**
     * @brief Destroys the LaunchpadController instance.
     */
    virtual ~LaunchpadController();

    /**
     * @brief Updates the LaunchpadController for the current service cycle.
     */
    virtual void update() override;

    /**
     * @brief Receives a MIDI message from the platform transport.
     *
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     */
    virtual void recvMidi(uint8_t cable, const MidiMessage &message) override;

private:
    using Color = LaunchpadDevice::Color;

    /**
     * @brief Returns color.
     *
     * @param[in] red Whether red is enabled for this operation.
     * @param[in] green Whether green is enabled for this operation.
     * @param[in] brightness Display/LED brightness level.
     *
     * @return Current drawing color.
     */
    inline Color color(bool red, bool green, int brightness = 3) const {
        return Color(red ? brightness : 0, green ? brightness : 0);
    }

    /**
     * @brief Returns the color off.
     *
     * @return Launchpad color value representing an unlit LED.
     */
    inline Color colorOff() const { return Color(0, 0); }
    /**
     * @brief Returns color red.
     *
     * @param[in] brightness Display/LED brightness level.
     *
     * @return Red color component.
     */
    inline Color colorRed(int brightness = 3) const { return Color(brightness, 0); }
    /**
     * @brief Returns color green.
     *
     * @param[in] brightness Display/LED brightness level.
     *
     * @return Green color component.
     */
    inline Color colorGreen(int brightness = 3) const { return Color(0, brightness); }
    /**
     * @brief Returns color yellow.
     *
     * @param[in] brightness Display/LED brightness level.
     *
     * @return Yellow/mixed color value.
     */
    inline Color colorYellow(int brightness = 3) const { return Color(brightness, brightness); }

    /**
     * @brief Stores the fields required to represent button.
     */
    struct Button {
        /**
         * @brief UI value representing row.
         */
        int row = -1; ///< Launchpad grid row; `-1` means the coordinate has not been resolved.
        /**
         * @brief UI value representing col.
         */
        int col = -1; ///< Launchpad grid column; `-1` means the coordinate has not been resolved.

        /**
         * @brief Constructs a Button instance.
         */
        Button() = default;
        /**
         * @brief Constructs a Button instance.
         *
         * @param[in] row Zero-based row index.
         * @param[in] col Zero-based column index.
         */
        Button(int row, int col) : row(row), col(col) {}

        /**
         * @brief Compares this value with another instance for value equality.
         *
         * @param[in] other Same-type value to compare with this instance.
         *
         * @return `true` when the represented state is equal; otherwise `false`.
         */
        bool operator==(const Button &other) const {
            return row == other.row && col == other.col;
        }

        /**
         * @brief Compares this value with another instance for value inequality.
         *
         * @param[in] other Same-type value to compare with this instance.
         *
         * @return `true` when the represented state differs; otherwise `false`.
         */
        bool operator!=(const Button &other) const {
            return !(*this == other);
        }

        template<typename T>
        /**
         * @brief Returns the is.
         *
         * @return `true` if is; otherwise `false`.
         */
        bool is() const {
            return row == T::row && col == T::col;
        }

        /**
         * @brief Reports whether grid.
         *
         * @return `true` if grid; otherwise `false`.
         */
        bool isGrid() const { return row < 8; }
        /**
         * @brief Reports whether function.
         *
         * @return `true` if function; otherwise `false`.
         */
        bool isFunction() const { return row == LaunchpadDevice::FunctionRow; }
        /**
         * @brief Reports whether scene.
         *
         * @return `true` if scene; otherwise `false`.
         */
        bool isScene() const { return row == LaunchpadDevice::SceneRow; }

        /**
         * @brief Returns the grid index.
         *
         * @return Zero-based grid index.
         */
        int gridIndex() const { return row * 8 + col; }
        /**
         * @brief Returns the function.
         *
         * @return Zero-based function-key index represented by the key/state.
         */
        int function() const { return isFunction() ? col : -1; }
        /**
         * @brief Returns the scene.
         *
         * @return Zero-based scene index, or `-1` when the button is not a scene button.
         */
        int scene() const { return isScene() ? col : -1; }
    };

    /**
     * @brief Enumerates the supported button action values.
     */
    enum class ButtonAction {
        Down, ///< Selects the down button action.
        Up, ///< Selects the up button action.
        Press, ///< Selects the press button action.
        DoublePress ///< Selects the double press button action.
    };

    /**
     * @brief Stores the fields required to represent navigation.
     */
    struct Navigation {
        /**
         * @brief UI value representing col.
         */
        int8_t col; ///< Launchpad grid column; `-1` means the coordinate has not been resolved.
        /**
         * @brief UI value representing row.
         */
        int8_t row; ///< Launchpad grid row; `-1` means the coordinate has not been resolved.
        /**
         * @brief UI value representing left.
         */
        int8_t left; ///< Left edge of the mapped Launchpad region, in grid coordinates.
        /**
         * @brief UI value representing right.
         */
        int8_t right; ///< Right edge of the mapped Launchpad region, in grid coordinates.
        /**
         * @brief UI value representing bottom.
         */
        int8_t bottom; ///< Bottom edge of the mapped Launchpad region, in grid coordinates.
        /**
         * @brief UI value representing top.
         */
        int8_t top; ///< Top edge of the mapped Launchpad region, in grid coordinates.
    };

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        Sequence, ///< Selects the sequence mode.
        Pattern, ///< Selects the pattern mode.
        Performer, ///< Selects the performer mode.
    };

    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode);

    // Global handlers
    /**
     * @brief Draws controller/global UI state that is independent of a specific page.
     */
    void globalDraw();
    /**
     * @brief Returns global button.
     *
     * @param[in] button Button identifier or button state addressed by the operation.
     * @param[in] action Action identifier to execute.
     *
     * @return `true` if global button; otherwise `false`.
     */
    bool globalButton(const Button &button, ButtonAction action);

    // Sequence mode
    /**
     * @brief Enters Launchpad sequence-edit mode and initializes its controller state.
     */
    void sequenceEnter();
    /**
     * @brief Leaves Launchpad sequence-edit mode and clears mode-specific controller state.
     */
    void sequenceExit();
    /**
     * @brief Renders the complete sequence-edit view on the Launchpad.
     */
    void sequenceDraw();
    /**
     * @brief Handles a Launchpad button event while sequence-edit mode is active.
     *
     * @param[in] button Button identifier or button state addressed by the operation.
     * @param[in] action Action identifier to execute.
     */
    void sequenceButton(const Button &button, ButtonAction action);

    /**
     * @brief Recomputes sequence-grid navigation for the current track, layer, and visible step range.
     */
    void sequenceUpdateNavigation();

    /**
     * @brief Selects the sequence layer addressed by the Launchpad grid position.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void sequenceSetLayer(int row, int col);
    /**
     * @brief Sets the first active step of the edited sequence.
     *
     * @param[in] step Zero-based sequence-step index.
     */
    void sequenceSetFirstStep(int step);
    /**
     * @brief Sets the last active step of the edited sequence.
     *
     * @param[in] step Zero-based sequence-step index.
     */
    void sequenceSetLastStep(int step);
    /**
     * @brief Selects the run mode for the edited sequence.
     *
     * @param[in] mode Mode to select.
     */
    void sequenceSetRunMode(int mode);
    /**
     * @brief Toggles the sequence step addressed by a Launchpad grid position.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void sequenceToggleStep(int row, int col);
    /**
     * @brief Toggles the note gate for the step addressed by a Launchpad grid position.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void sequenceToggleNoteStep(int row, int col);
    /**
     * @brief Applies the current sequence-edit operation to the addressed step.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void sequenceEditStep(int row, int col);
    /**
     * @brief Applies the current note-layer edit to the addressed note step.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void sequenceEditNoteStep(int row, int col);
    /**
     * @brief Applies the current curve-layer edit to the addressed curve step.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void sequenceEditCurveStep(int row, int col);

    /**
     * @brief Renders the active sequence-layer selector on the Launchpad.
     */
    void sequenceDrawLayer();
    /**
     * @brief Renders the editable first/last-step range on the Launchpad.
     *
     * @param[in] highlight Highlight state or intensity used for rendering.
     */
    void sequenceDrawStepRange(int highlight);
    /**
     * @brief Renders the selected sequence run mode on the Launchpad.
     */
    void sequenceDrawRunMode();
    /**
     * @brief Renders sequence steps for the currently selected track.
     */
    void sequenceDrawSequence();
    /**
     * @brief Renders note-sequence steps and note-layer state on the Launchpad.
     */
    void sequenceDrawNoteSequence();
    /**
     * @brief Renders curve-sequence steps and curve-layer state on the Launchpad.
     */
    void sequenceDrawCurveSequence();

    // Pattern mode
    /**
     * @brief Activates pattern-control mode.
     */
    void patternEnter();
    /**
     * @brief Leaves pattern-control mode.
     */
    void patternExit();
    /**
     * @brief Renders pattern-control state on the controller.
     */
    void patternDraw();
    /**
     * @brief Handles a pattern-mode button event.
     *
     * @param[in] button Button identifier or button state addressed by the operation.
     * @param[in] action Action identifier to execute.
     */
    void patternButton(const Button &button, ButtonAction action);

    // Performer mode
    /**
     * @brief Activates performer-control mode.
     */
    void performerEnter();
    /**
     * @brief Leaves performer-control mode.
     */
    void performerExit();
    /**
     * @brief Renders performer-control state on the controller.
     */
    void performerDraw();
    /**
     * @brief Handles a performer-mode button event.
     *
     * @param[in] button Button identifier or button state addressed by the operation.
     * @param[in] action Action identifier to execute.
     */
    void performerButton(const Button &button, ButtonAction action);

    // Navigation
    /**
     * @brief Renders navigation controls.
     *
     * @param[in] navigation Navigation state used by the UI/controller.
     */
    void navigationDraw(const Navigation &navigation);
    /**
     * @brief Handles a navigation-button press.
     *
     * @param[in] navigation Navigation state used by the UI/controller.
     * @param[in] button Button identifier or button state addressed by the operation.
     */
    void navigationButtonDown(Navigation &navigation, const Button &button);

    // Drawing
    /**
     * @brief Draws tracks gate and selected.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] selectedTrack Selected track or selected-track index.
     */
    void drawTracksGateAndSelected(const Engine &engine, int selectedTrack);
    /**
     * @brief Draws tracks gate and mute.
     *
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] playState Project playback state read or modified by the operation.
     */
    void drawTracksGateAndMute(const Engine &engine, const PlayState &playState);

    template<typename Enum>
    /**
     * @brief Draws enum.
     *
     * @param[in] e Event/value supplied to the callback or operation.
     */
    void drawEnum(Enum e) { drawRange(0, int(Enum::Last) - 1, int(e)); }
    /**
     * @brief Draws range.
     *
     * @param[in] first First value/index in the inclusive range.
     * @param[in] last Last value/index in the inclusive range.
     * @param[in] selected Selection state or selection set applied by the operation.
     */
    void drawRange(int first, int last, int selected);

    /**
     * @brief Returns step color.
     *
     * @param[in] active Whether the addressed state should be active.
     * @param[in] current Whether current is enabled for this operation.
     *
     * @return Color associated with the requested sequence step.
     */
    Color stepColor(bool active, bool current) const;
    /**
     * @brief Draws note sequence bits.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] layer Layer index or layer identifier.
     * @param[in] currentStep Zero-based index of the current sequence step.
     */
    void drawNoteSequenceBits(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    /**
     * @brief Draws note sequence bars.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] layer Layer index or layer identifier.
     * @param[in] currentStep Zero-based index of the current sequence step.
     */
    void drawNoteSequenceBars(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    /**
     * @brief Draws note sequence dots.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] layer Layer index or layer identifier.
     * @param[in] currentStep Zero-based index of the current sequence step.
     */
    void drawNoteSequenceDots(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    /**
     * @brief Draws note sequence notes.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] layer Layer index or layer identifier.
     * @param[in] currentStep Zero-based index of the current sequence step.
     */
    void drawNoteSequenceNotes(const NoteSequence &sequence, NoteSequence::Layer layer, int currentStep);
    /**
     * @brief Draws curve sequence bars.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] layer Layer index or layer identifier.
     * @param[in] currentStep Zero-based index of the current sequence step.
     */
    void drawCurveSequenceBars(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep);
    /**
     * @brief Draws curve sequence dots.
     *
     * @param[in] sequence Sequence to inspect or modify.
     * @param[in] layer Layer index or layer identifier.
     * @param[in] currentStep Zero-based index of the current sequence step.
     */
    void drawCurveSequenceDots(const CurveSequence &sequence, CurveSequence::Layer layer, int currentStep);
    /**
     * @brief Draws bar.
     *
     * @param[in] row Zero-based row index.
     * @param[in] value Value to apply, store, compare, or encode as defined by the operation.
     * @param[in] active Whether the addressed state should be active.
     * @param[in] current Whether current is enabled for this operation.
     */
    void drawBar(int row, int value, bool active, bool current);

    // Led handling
    /**
     * @brief Sets the grid led.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setGridLed(int row, int col, Color color);
    /**
     * @brief Sets the grid led.
     *
     * @param[in] index Zero-based grid led index.
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setGridLed(int index, Color color);
    /**
     * @brief Sets the function led.
     *
     * @param[in] col Zero-based column index.
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setFunctionLed(int col, Color color);
    /**
     * @brief Sets the scene led.
     *
     * @param[in] col Zero-based column index.
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setSceneLed(int col, Color color);

    template<typename T>
    /**
     * @brief Sets the button led.
     *
     * @param[in] color Color value/index to draw, store, or emit.
     */
    void setButtonLed(Color color) {
        _device->setLed(T::row, T::col, color);
    }

    template<typename T>
    /**
     * @brief Mirrors/forwards the supplied button state to the mapped control.
     */
    void mirrorButton() {
        setButtonLed<T>(buttonState(T::row, T::col) ? color(true, true) : color(false, false));
    }

    // Button handling
    /**
     * @brief Dispatches a controller button event to the active mode/page.
     *
     * @param[in] button Button identifier or button state addressed by the operation.
     * @param[in] action Action identifier to execute.
     */
    void dispatchButtonEvent(const Button& button, ButtonAction action);
    /**
     * @brief Handles a button press.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void buttonDown(int row, int col);
    /**
     * @brief Handles a button release.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     */
    void buttonUp(int row, int col);
    /**
     * @brief Returns button state.
     *
     * @param[in] row Zero-based row index.
     * @param[in] col Zero-based column index.
     *
     * @return `true` if button state; otherwise `false`.
     */
    bool buttonState(int row, int col) const;

    template<typename T>
    /**
     * @brief Returns the button state.
     *
     * @return `true` if button state; otherwise `false`.
     */
    bool buttonState() const {
        return buttonState(T::row, T::col);
    }

    /**
     * @brief Most recently observed button.
     */
    struct {
        Button lastButton;
        uint32_t lastTicks = 0;
        uint8_t count = 1;
    } _buttonTracker;

    /**
     * @brief Active project model used by this component.
     */
    Project &_project; ///< Active project model used by this component.
    Container<LaunchpadDevice, LaunchpadMk2Device, LaunchpadMk3Device, LaunchpadProDevice, LaunchpadProMk3Device> _deviceContainer; ///< In-place variant storage that owns the active device object without heap allocation.
    /**
     * @brief Pointer to device; `nullptr` denotes that no object/resource is assigned.
     */
    LaunchpadDevice *_device; ///< Pointer to device; `nullptr` denotes that no object/resource is assigned.
    Mode _mode = Mode::Sequence; ///< Active mode controlling the behavior of `LaunchpadController`.

    /**
     * @brief Groups navigation state and the function-key state used by the corresponding Launchpad layer.
     */
    struct {
        Navigation navigation = { 0, 0, 0, 7, 0, 0 };
    } _sequence;

    /**
     * @brief Groups navigation state and the function-key state used by the corresponding Launchpad layer.
     */
    struct {
        Navigation navigation = { 0, 0, 0, 0, -1, 0 };
    } _pattern;
};
