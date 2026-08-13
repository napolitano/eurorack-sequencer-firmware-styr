/**
 * @file Model.h
 * @brief Declares the Model component used by the sequencer model.
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

#include "Project.h"
#include "Settings.h"
#include "ClipBoard.h"
#include "Serialize.h"

#include "os/os.h"

/**
 * @brief Stores and manipulates model model data.
 */
class Model {
public:
    //----------------------------------------
    // Types
    //----------------------------------------

    /**
     * @brief Stores and manipulates write lock model data.
     */
    class WriteLock : public os::InterruptLock {};

    /**
     * @brief Stores and manipulates config lock model data.
     */
    class ConfigLock {
    public:
        /**
         * @brief Constructs a ConfigLock instance.
         */
        ConfigLock() {
        }

        /**
         * @brief Destroys the ConfigLock instance.
         */
        ~ConfigLock() {
        }
    };

    //----------------------------------------
    // Properties
    //----------------------------------------

    /**
     * @brief Returns the project.
     *
     * @return Reference to the project.
     */
    const Project &project() const { return _project; }
          /**
           * @brief Returns the project.
           *
           * @return Reference to the project.
           */
          Project &project()       { return _project; }

    /**
     * @brief Sets the tings.
     *
     * @return Reference to the settings.
     */
    const Settings &settings() const { return _settings; }
          /**
           * @brief Sets the tings.
           *
           * @return Reference to the settings.
           */
          Settings &settings()       { return _settings; }

    /**
     * @brief Returns the clip board.
     *
     * @return Reference to the clip board.
     */
    const ClipBoard &clipBoard() const { return _clipBoard; }
          /**
           * @brief Returns the clip board.
           *
           * @return Reference to the clip board.
           */
          ClipBoard &clipBoard()       { return _clipBoard; }

    //----------------------------------------
    // Methods
    //----------------------------------------

    /**
     * @brief Constructs a Model instance.
     */
    Model();

    /**
     * @brief Initializes the Model and its runtime resources.
     */
    void init();

private:
    /**
     * @brief Active project model used by this component.
     */
    Project _project; ///< Active project model used by this component.
    Settings _settings; ///< Persistent global settings model.
    ClipBoard _clipBoard; ///< In-memory clipboard used for copy/paste operations across sequencer objects.
};
