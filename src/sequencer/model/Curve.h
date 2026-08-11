/**
 * @file Curve.h
 * @brief Declares the Curve component used by the sequencer model.
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

class Curve {
public:
    typedef float (*Function)(float x);

    enum Type {
        Low,
        High,
        StepUp,
        StepDown,
        RampUp,
        RampDown,
        ExpUp,
        ExpDown,
        LogUp,
        LogDown,
        SmoothUp,
        SmoothDown,
        Triangle,
        Bell,
        ExpDown2x,
        ExpDown3x,
        ExpDown4x,
        Last
    };

    static Function function(Type type);

    static float eval(Type type, float x);
};

