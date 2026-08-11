/**
 * @file EngineState.h
 * @brief Declares the EngineState component used by the sequencer engine.
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

class EngineState {
public:
    bool running() const { return _running; }
    bool recording() const { return _recording; }

private:
    void setRunning(bool running) { _running = running; }
    void setRecording(bool recording) { _recording = recording; }

    bool _running = false;
    bool _recording = false;

    friend class Engine;
};
