/**
 * @file UsbH.h
 * @brief Declares the UsbH component used by the STM32 sequencer driver.
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

#include "UsbMidi.h"

#include <cstdint>

/**
 * @brief Provides the usb h hardware/platform abstraction.
 */
class UsbH {
public:
    /**
     * @brief Constructs a UsbH instance.
     *
     * @param[in] usbMidi USB-MIDI transport used by the sequencer engine.
     */
    UsbH(UsbMidi &usbMidi);

    /**
     * @brief Initializes the UsbH and its runtime resources.
     */
    void init();

    /**
     * @brief Processes the supplied input for this component.
     */
    void process();

    /**
     * @brief Enables the powered state for this component.
     */
    void powerOn();
    /**
     * @brief Disables the powered state for this component.
     */
    void powerOff();
    /**
     * @brief Returns the power fault.
     *
     * @return `true` if power fault; otherwise `false`.
     */
    bool powerFault();

private:
    /**
     * @brief Registers a connected MIDI device.
     *
     * @param[in] device Device object or device identifier addressed by the operation.
     * @param[in] vendorId USB vendor identifier (VID).
     * @param[in] productId USB product identifier (PID).
     */
    void midiConnectDevice(uint8_t device, uint16_t vendorId, uint16_t productId) {
        _midiDevices |= (1 << device);
        _usbMidi.connect(vendorId, productId);
    }

    /**
     * @brief Removes a disconnected MIDI device.
     *
     * @param[in] device Device object or device identifier addressed by the operation.
     */
    void midiDisconnectDevice(uint8_t device) {
        _midiDevices &= ~(1 << device);
        _usbMidi.disconnect();
    }

    /**
     * @brief Returns midi device connected.
     *
     * @param[in] device Device object or device identifier addressed by the operation.
     *
     * @return `true` if midi device connected; otherwise `false`.
     */
    bool midiDeviceConnected(uint8_t device) {
        return _midiDevices & (1 << device);
    }

    /**
     * @brief Queues a MIDI message for host/frontend delivery.
     *
     * @param[in] device Device object or device identifier addressed by the operation.
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     */
    void midiEnqueueMessage(uint8_t device, uint8_t cable, const MidiMessage &message) {
        _usbMidi.enqueueMessage(cable, message);
    }

    /**
     * @brief Queues raw MIDI data for host/frontend delivery.
     *
     * @param[in] device Device object or device identifier addressed by the operation.
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] data Input data to read, decode, copy, or process.
     */
    void midiEnqueueData(uint8_t device, uint8_t cable, uint8_t data) {
        _usbMidi.enqueueData(cable, data);
    }

    /**
     * @brief Returns midi dequeue message.
     *
     * @param[in] device Device object or device identifier addressed by the operation.
     * @param[in] cable USB-MIDI cable number selected for the operation.
     * @param[in] message Message to process.
     *
     * @return `true` if midi dequeue message; otherwise `false`.
     */
    bool midiDequeueMessage(uint8_t *device, uint8_t *cable, MidiMessage *message) {
        *device = 0;
        return _usbMidi.dequeueMessage(cable, message);
    }

    /**
     * @brief Reference to usb midi owned by another component.
     */
    UsbMidi &_usbMidi; ///< Reference to usb midi owned by another component.

    /**
     * @brief Driver value representing midi devices.
     */
    uint8_t _midiDevices = 0; ///< Number of currently connected USB-MIDI devices.

    friend struct MidiDriverHandler;
};
