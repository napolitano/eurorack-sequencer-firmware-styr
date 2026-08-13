/**
 * @file MonitorPage.h
 * @brief Declares the MonitorPage component used by the sequencer UI page.
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

#include "BasePage.h"

#include "engine/MidiPort.h"

#include "core/midi/MidiMessage.h"

/**
 * @brief Implements the monitor page user-interface page.
 */
class MonitorPage : public BasePage {
public:
    /**
     * @brief Constructs a MonitorPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    MonitorPage(PageManager &manager, PageContext &context);

    /**
     * @brief Activates the page/controller and initializes transient UI state.
     */
    virtual void enter() override;
    /**
     * @brief Deactivates the page/controller and releases transient UI state.
     */
    virtual void exit() override;

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    virtual void draw(Canvas &canvas) override;
    /**
     * @brief Refreshes LED state from the current model and UI state.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    virtual void updateLeds(Leds &leds) override;

    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;
    /**
     * @brief Handles an encoder movement or push action.
     *
     * @param[in] event Event to process.
     */
    virtual void encoder(EncoderEvent &event) override;
    /**
     * @brief Returns or processes the MIDI interface associated with this component.
     *
     * @param[in] event Event to process.
     */
    virtual void midi(MidiEvent &event) override;

private:
    /**
     * @brief Draws cv in.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void drawCvIn(Canvas &canvas);
    /**
     * @brief Draws cv out.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void drawCvOut(Canvas &canvas);
    /**
     * @brief Draws midi.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void drawMidi(Canvas &canvas);
    /**
     * @brief Draws stats.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void drawStats(Canvas &canvas);

    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        CvIn, ///< Selects the cv in mode.
        CvOut, ///< Selects the cv out mode.
        Midi, ///< Selects the midi mode.
        Stats, ///< Selects the stats mode.
    };

    Mode _mode = Mode::CvIn; ///< Active mode controlling the behavior of `MonitorPage`.
    /**
     * @brief Most recently observed midi message.
     */
    MidiMessage _lastMidiMessage; ///< Most recent MIDI message retained for display in the monitor page.
    /**
     * @brief Most recently observed midi message port.
     */
    MidiPort _lastMidiMessagePort; ///< Port on which `_lastMidiMessage` was received.
    /**
     * @brief Most recently observed midi message ticks.
     */
    uint32_t _lastMidiMessageTicks = -1; ///< System tick when `_lastMidiMessage` arrived; `UINT32_MAX` denotes that no message has been observed yet.
};
