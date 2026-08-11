/**
 * @file TargetTraceRecorder.h
 * @brief Declares the TargetTraceRecorder component used by the simulator core.
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

#include "TargetStateTracker.h"
#include "TargetTrace.h"

namespace sim {

class TargetTraceRecorder : public TargetStateTracker, public TargetTickHandler {
public:
    TargetTraceRecorder(TargetTrace &targetTrace);

    TargetTrace &targetTrace() { return _targetTrace; }

    // TargetTickHandler
    virtual void setTick(uint32_t tick) override;

    // TargetInputHandler
    virtual void writeButton(int index, bool pressed) override;
    virtual void writeEncoder(EncoderEvent event) override;
    virtual void writeAdc(int channel, uint16_t value) override;
    virtual void writeDigitalInput(int pin, bool value) override;
    virtual void writeMidiInput(MidiEvent event) override;

    // TargetOutputHandler
    virtual void writeLed(int index, bool red, bool green) override;
    virtual void writeGateOutput(int channel, bool value) override;
    virtual void writeDac(int channel, uint16_t value) override;
    virtual void writeDigitalOutput(int pin, bool value) override;
    virtual void writeLcd(const FrameBuffer &frameBuffer) override;
    virtual void writeMidiOutput(MidiEvent event) override;

private:
    TargetState _targetState;
    uint32_t _tick = 0;
    TargetTrace &_targetTrace;
};

} // namespace sim
