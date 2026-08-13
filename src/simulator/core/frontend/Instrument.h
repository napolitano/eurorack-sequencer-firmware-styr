/**
 * @file Instrument.h
 * @brief Declares the Instrument component used by the simulator frontend.
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

#include <memory>

namespace sim {

/**
 * @brief Provides instrument behavior for the desktop simulator.
 */
class Instrument {
public:
    typedef std::shared_ptr<Instrument> Ptr;

    /**
     * @brief Destroys the Instrument instance.
     */
    virtual ~Instrument() {}

    /**
     * @brief Sets the gate.
     *
     * @param[in] gate `true` to enable/set gate; `false` to disable/clear it.
     */
    virtual void setGate(bool gate) = 0;
    /**
     * @brief Sets the cv.
     *
     * @param[in] cv Control-voltage value in the engine/model voltage domain.
     */
    virtual void setCv(float cv) = 0;
};

} // namespace sim
