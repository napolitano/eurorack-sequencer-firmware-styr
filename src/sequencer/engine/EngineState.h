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

/**
 * @brief Stores engine state used while the component is running.
 */
class EngineState {
public:
    /**
     * @brief Returns the running.
     *
     * @return `true` if running; otherwise `false`.
     */
    bool running() const { return _running; }
    /**
     * @brief Returns the recording.
     *
     * @return `true` if recording; otherwise `false`.
     */
    bool recording() const { return _recording; }

private:
    /**
     * @brief Sets the running.
     *
     * @param[in] running `true` to enable/set running; `false` to disable/clear it.
     */
    void setRunning(bool running) { _running = running; }
    /**
     * @brief Sets the recording.
     *
     * @param[in] recording `true` to enable/set recording; `false` to disable/clear it.
     */
    void setRecording(bool recording) { _recording = recording; }

    /**
     * @brief Whether running is true in the current state.
     */
    bool _running = false; ///< True while the engine/tool main loop is running.
    /**
     * @brief Whether recording is true in the current state.
     */
    bool _recording = false; ///< True while the sequencer engine is recording incoming performance data.

    friend class Engine;
};
