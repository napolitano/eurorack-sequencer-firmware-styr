/**
 * @file TestMidi.cpp
 * @brief Implements the TestMidi component used by the integration test.
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
#include "IntegrationTest.h"

#include "drivers/Midi.h"

class TestMidi : public IntegrationTest {
public:
    void init() override {
        midi.init();
    }

    void update() override {
        switch (mode) {
        case Receive: {
            MidiMessage msg;
            if (midi.recv(&msg)) {
                MidiMessage::dump(msg);
            }
            break;
        }
        case Arpeggio: {
            DBG("step: %d", step);
            midi.send(MidiMessage::makeNoteOff(0, 36 + step % 24));
            ++step;
            midi.send(MidiMessage::makeNoteOn(0, 36 + step % 24));
            os::delay(500);
            break;
        }
        }
    }

private:
    enum Mode {
        Receive,
        Arpeggio,
    };

    Midi midi;
    Mode mode = Receive;
    int step = 0;
};

INTEGRATION_TEST(TestMidi, "Midi", true)
