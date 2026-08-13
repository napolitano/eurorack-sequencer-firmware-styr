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

/**
 * @brief Stores and manipulates curve model data.
 */
class Curve {
public:
    typedef float (*Function)(float x);

    /**
     * @brief Enumerates the supported type values.
     */
    enum Type {
        Low, ///< Selects the low type.
        High, ///< Selects the high type.
        StepUp, ///< Selects the step up type.
        StepDown, ///< Selects the step down type.
        RampUp, ///< Selects the ramp up type.
        RampDown, ///< Selects the ramp down type.
        ExpUp, ///< Selects the exp up type.
        ExpDown, ///< Selects the exp down type.
        LogUp, ///< Selects the log up type.
        LogDown, ///< Selects the log down type.
        SmoothUp, ///< Selects the smooth up type.
        SmoothDown, ///< Selects the smooth down type.
        Triangle, ///< Selects the triangle type.
        Bell, ///< Selects the bell type.
        ExpDown2x, ///< Selects the exp down2x type.
        ExpDown3x, ///< Selects the exp down3x type.
        ExpDown4x, ///< Selects the exp down4x type.
        Last ///< Sentinel marking the end of the valid enumeration range.
    };

    /**
     * @brief Returns function.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     *
     * @return Result of function().
     */
    static Function function(Type type);

    /**
     * @brief Returns eval.
     *
     * @param[in] type Type discriminator selecting the variant or behavior handled by this API.
     * @param[in] x Horizontal coordinate or scalar x component.
     *
     * @return Evaluated function/curve value.
     */
    static float eval(Type type, float x);
};

