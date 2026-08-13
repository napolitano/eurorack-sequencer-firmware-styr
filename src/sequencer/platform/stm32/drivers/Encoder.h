/**
 * @file Encoder.h
 * @brief Declares the Encoder component used by the STM32 sequencer driver.
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

#include "SystemConfig.h"

#include "core/utils/RingBuffer.h"
#include "core/utils/Debouncer.h"

#include <cstdint>

/**
 * @brief Provides the encoder hardware/platform abstraction.
 */
class Encoder {
public:
    /**
     * @brief Enumerates the supported event values.
     */
    enum Event {
        Left,   ///< Encoder rotation toward decreasing values.
        Right,  ///< Encoder rotation toward increasing values.
        Down,   ///< Encoder push switch transitioned to pressed.
        Up,     ///< Encoder push switch transitioned to released.
    };

    /**
     * @brief Constructs a Encoder instance.
     *
     * @param[in] reverse Whether reverse is enabled for this operation.
     */
    Encoder(bool reverse = false);

    /**
     * @brief Initializes the Encoder and its runtime resources.
     */
    void init();

    /**
     * @brief Processes the supplied input for this component.
     */
    void process();

    /**
     * @brief Returns next event.
     *
     * @param[in] event Event to process.
     *
     * @return `true` if next event; otherwise `false`.
     */
    inline bool nextEvent(Event &event) {
        if (_events.readable() < 1) {
            return false;
        }
        event = Event(_events.read());
        return true;
    }

private:
    /**
     * @brief Whether reverse is true in the current state.
     */
    bool _reverse; ///< True when encoder direction is reversed to match the installed hardware orientation.

    RingBuffer<uint8_t, 32> _events; ///< Fixed-capacity FIFO holding pending events.

    Debouncer<3> _switchDebouncer; ///< Debouncer state used to reject switch transitions that have not remained stable long enough.
    /**
     * @brief Whether switch state is true in the current state.
     */
    bool _switchState = false; ///< Last sampled logical state of the encoder push switch.
    /**
     * @brief Driver value representing encoder state.
     */
    uint8_t _encoderState = 0; ///< Last sampled quadrature state used to detect encoder transitions.
};
