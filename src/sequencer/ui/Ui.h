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

class Ui {
public:
    Ui(Model &model, Engine &engine, Lcd &lcd, ButtonLedMatrix &blm, Encoder &encoder);

    void init();
    void update();

#ifdef PLATFORM_SIM
    // Simulator/documentation tooling needs to know when the modal startup
    // page has finished before it can send page-navigation input. Keep this
    // test hook out of firmware builds.
    bool simulatorStartupActive() const { return _pageManager.top() == &_pages.startup; }
#endif

    void showAssert(const char *filename, int line, const char *msg);

private:
    void handleKeys();
    void handleEncoder();
    void handleMidi();

    Model &_model;
    Engine &_engine;

    Lcd &_lcd;
    ButtonLedMatrix &_blm;
    Encoder &_encoder;

    struct ReceiveMidiEvent {
        MidiPort port;
        uint8_t cable;
        MidiMessage message;
    };
    RingBuffer<ReceiveMidiEvent, 16> _receiveMidiEvents;

    uint8_t _frameBufferData[CONFIG_LCD_WIDTH * CONFIG_LCD_HEIGHT];
    FrameBuffer8bit _frameBuffer;
    Canvas _canvas;
    uint32_t _lastFrameBufferUpdateTicks;

    KeyState _pageKeyState;
    KeyState _globalKeyState;
    KeyPressEventTracker _keyPressEventTracker;
    Leds _leds;

    MessageManager _messageManager;

    PageManager _pageManager;
    PageContext _pageContext;
    Pages _pages;

    ControllerManager _controllerManager;
    uint32_t _lastControllerUpdateTicks;
};
