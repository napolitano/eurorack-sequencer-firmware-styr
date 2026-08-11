/**
 * @file DrumSampler.h
 * @brief Declares the DrumSampler component used by the simulator frontend.
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

#include "../Audio.h"
#include "../Instrument.h"

namespace sim {

class DrumSampler : public Instrument {
public:
    DrumSampler(Audio &audio, const std::string &filename);

    virtual void setGate(bool gate) override;
    virtual void setCv(float cv) override;

private:
    void trigger();

    Audio &_audio;
    Sample _sample;
    bool _gate = false;
};

} // namespace sim
