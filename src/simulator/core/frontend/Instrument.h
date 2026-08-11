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

class Instrument {
public:
    typedef std::shared_ptr<Instrument> Ptr;

    virtual ~Instrument() {}

    virtual void setGate(bool gate) = 0;
    virtual void setCv(float cv) = 0;
};

} // namespace sim
