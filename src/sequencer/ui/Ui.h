/**
 * @file Ui.h
 * @brief Declares the Ui component used by the sequencer UI.
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
#include "MessageManager.h"
#include "Page.h"
#include "PageManager.h"
#include "Key.h"
#include "KeyPressEventTracker.h"
#include "Leds.h"
#include "ControllerManager.h"

#include "pages/Pages.h"

#include "drivers/ButtonLedMatrix.h"
#include "drivers/Encoder.h"
#include "drivers/Lcd.h"

#include "core/gfx/FrameBuffer.h"
#include "core/gfx/Canvas.h"
#include "core/utils/RingBuffer.h"
#include "core/midi/MidiMessage.h"

#include "engine/Engine.h"

#include "model/Model.h"

class Key;

/**
 * @brief Coordinates sequencer input handling, page navigation, rendering, LEDs, and external controllers.
 */
class Ui {
public:
    /**
     * @brief Constructs a Ui instance.
     *
     * @param[in] model Sequencer model that supplies persistent configuration and project state.
     * @param[in] engine Sequencer engine that owns or drives this component.
     * @param[in] lcd Lcd used by the operation.
     * @param[in] blm Blm used by the operation.
     * @param[in] encoder Encoder used by the operation.
     */
    Ui(Model &model, Engine &engine, Lcd &lcd, ButtonLedMatrix &blm, Encoder &encoder);

    /**
     * @brief Initializes the Ui and its runtime resources.
     */
    void init();
    /**
     * @brief Updates the Ui for the current service cycle.
     */
    void update();

#ifdef PLATFORM_SIM
    // Simulator/documentation tooling needs to know when the modal startup
    // page has finished before it can send page-navigation input. Keep this
    // test hook out of firmware builds.
    /**
     * @brief Returns the simulator startup active.
     *
     * @return `true` if simulator startup active; otherwise `false`.
     */
    bool simulatorStartupActive() const { return _pageManager.top() == &_pages.startup; }
#endif

    /**
     * @brief Shows assert.
     *
     * @param[in] filename Filename identifying the file or path used by the operation.
     * @param[in] line Line used by the operation.
     * @param[in] msg Raw MIDI message byte to process.
     */
    void showAssert(const char *filename, int line, const char *msg);

private:
    /**
     * @brief Handles keys.
     */
    void handleKeys();
    /**
     * @brief Handles encoder.
     */
    void handleEncoder();
    /**
     * @brief Handles midi.
     */
    void handleMidi();

    Model &_model; ///< Persistent sequencer model used by this component.
    Engine &_engine; ///< Reference to engine owned by another component.

    Lcd &_lcd; ///< Reference to lcd owned by another component.
    ButtonLedMatrix &_blm; ///< Reference to blm owned by another component.
    Encoder &_encoder; ///< Reference to encoder owned by another component.

    /**
     * @brief Stores one MIDI receive event queued for processing by the UI.
     */
    struct ReceiveMidiEvent {
        MidiPort port; ///< UI subsystem state/service for port.
        uint8_t cable; ///< UI value representing cable.
        MidiMessage message; ///< UI subsystem state/service for message.
    };
    RingBuffer<ReceiveMidiEvent, 16> _receiveMidiEvents; ///< Queue of pending MIDI receive events consumed by the UI thread.

    uint8_t _frameBufferData[CONFIG_LCD_WIDTH * CONFIG_LCD_HEIGHT]; ///< UI value representing frame buffer data.
    FrameBuffer8bit _frameBuffer; ///< Pixel buffer backing the hardware/simulator display.
    Canvas _canvas; ///< Drawing canvas bound to the UI framebuffer.
    uint32_t _lastFrameBufferUpdateTicks; ///< Most recently observed frame buffer update ticks.

    KeyState _pageKeyState; ///< UI subsystem state/service for page key state.
    KeyState _globalKeyState; ///< UI subsystem state/service for global key state.
    KeyPressEventTracker _keyPressEventTracker; ///< UI subsystem state/service for key press event tracker.
    Leds _leds; ///< UI subsystem state/service for leds.

    MessageManager _messageManager; ///< UI subsystem state/service for message manager.

    PageManager _pageManager; ///< UI subsystem state/service for page manager.
    PageContext _pageContext; ///< UI subsystem state/service for page context.
    Pages _pages; ///< UI subsystem state/service for pages.

    ControllerManager _controllerManager; ///< UI subsystem state/service for controller manager.
    uint32_t _lastControllerUpdateTicks; ///< Most recently observed controller update ticks.
};
