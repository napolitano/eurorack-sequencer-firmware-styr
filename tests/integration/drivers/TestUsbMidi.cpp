/**
 * @file TestUsbMidi.cpp
 * @brief Implements the TestUsbMidi component used by the integration test.
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

#ifdef PLATFORM_STM32
#include "drivers/UsbH.h"
#endif

#include "drivers/UsbMidi.h"

#include "core/midi/MidiMessage.h"

class TestUsbMidi : public IntegrationTest {
public:
#ifdef PLATFORM_STM32
    TestUsbMidi() :
        usbh(usbMidi)
    {}
#endif

    void init() override {
        usbMidi.init();
#ifdef PLATFORM_STM32
        usbh.init();
        usbh.powerOn();
#endif
    }

    void update() override {
#ifdef PLATFORM_STM32
        usbh.process();
#endif
        uint8_t cable;
        MidiMessage msg;
        while (usbMidi.recv(&cable, &msg)) {
            MidiMessage::dump(msg);
        }

        if (sendInterval++ % 200 == 0) {
            const uint8_t pattern[] = { 0, 12, 0, 12, 3, 9, 12, 3 };
            usbMidi.send(0, MidiMessage::makeNoteOff(0, 36 + pattern[sendPosition]));
            sendPosition = (sendPosition + 1) % sizeof(pattern);
            usbMidi.send(0, MidiMessage::makeNoteOn(0, 36 + pattern[sendPosition]));
        }

        os::delay(1);
    }

private:
    UsbMidi usbMidi;
#ifdef PLATFORM_STM32
    UsbH usbh;
#endif
    uint32_t sendInterval = 0;
    uint8_t sendPosition = 0;
};

INTEGRATION_TEST(TestUsbMidi, "UsbMidi", true)
