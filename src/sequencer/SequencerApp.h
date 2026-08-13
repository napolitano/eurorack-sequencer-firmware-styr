/**
 * @file SequencerApp.h
 * @brief Declares the SequencerApp component used by the sequencer application.
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
#include "Config.h"

#include "drivers/Adc.h"
#include "drivers/ButtonLedMatrix.h"
#include "drivers/ClockTimer.h"
#include "drivers/Dac.h"
#include "drivers/Dio.h"
#include "drivers/Encoder.h"
#include "drivers/GateOutput.h"
#include "drivers/Lcd.h"
#include "drivers/Midi.h"
#include "drivers/SdCard.h"
#include "drivers/UsbMidi.h"

#include "core/fs/Volume.h"

#include "model/Model.h"
#include "model/FileManager.h"
#include "engine/Engine.h"
#include "ui/Ui.h"

#include "os/os.h"

/**
 * @brief Aggregates the platform drivers, filesystem services, model, engine, and UI that form the sequencer application.
 */
struct SequencerApp {
    // drivers
    ClockTimer clockTimer; ///< Hardware/platform service used for clock timer I/O.
    ButtonLedMatrix blm; ///< Hardware/platform service used for blm I/O.
    Lcd lcd; ///< Hardware/platform service used for lcd I/O.
    Adc adc; ///< Hardware/platform service used for adc I/O.
    Dac dac; ///< Hardware/platform service used for dac I/O.
    Dio dio; ///< Hardware/platform service used for dio I/O.
    Encoder encoder; ///< Hardware/platform service used for encoder I/O.
    GateOutput gateOutput; ///< Current digital gate output state.
    Midi midi; ///< Hardware/platform service used for midi I/O.
    UsbMidi usbMidi; ///< Hardware/platform service used for usb midi I/O.
    SdCard sdCard; ///< Hardware/platform service used for sd card I/O.

    // filesystem
    fs::Volume volume; ///< Mounted filesystem volume used for project and update storage.
    os::PeriodicTask<1024> fsTask; ///< Periodic task servicing fs  operations.

    uint8_t midiMessagePayloadPool[32]; ///< Runtime value representing midi message payload pool.

    // application
    Model model; ///< Persistent sequencer model used by this component.
    Engine engine; ///< Sequencer engine instance driven by the application.
    Ui ui; ///< Sequencer user-interface instance driven by the application.

    /**
     * @brief Constructs a SequencerApp instance.
     */
    SequencerApp() :
        volume(sdCard),
        fsTask("file", CONFIG_FILE_TASK_PRIORITY, os::time::ms(10), [] () {
            FileManager::processTask();
        }),
        /**
         * @brief Executes engine.
         *
         * @note Includes an unnamed `model` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `clockTimer` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `adc` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `dac` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `dio` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `gateOutput` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `midi` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `usbMidi` input parameter as declared by the inherited/interface signature.
         */
        engine(model, clockTimer, adc, dac, dio, gateOutput, midi, usbMidi),
        ui(model, engine, lcd, blm, encoder)
    {
        MidiMessage::setPayloadPool(midiMessagePayloadPool, sizeof(midiMessagePayloadPool));

        FileManager::init();

        model.init();
        engine.init();
        ui.init();
    }

    /**
     * @brief Updates the SequencerApp for the current service cycle.
     */
    void update() {
        engine.update();
        ui.update();
    }
};
