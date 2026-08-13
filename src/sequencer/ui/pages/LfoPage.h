/**
 * @file LfoPage.h
 * @brief Declares the LfoPage component used by the sequencer UI page.
 *
 * @author Axel Napolitano — Styr implementation and modernization
 * @copyright 2026 Axel Napolitano
 *
 * @par License
 * MIT; see LICENSES/MIT.txt.
 *
 * SPDX-FileCopyrightText: 2026 Axel Napolitano
 * SPDX-License-Identifier: MIT
 */
#pragma once

#include "Config.h"

#include "ui/pages/BasePage.h"
#include "model/LfoTrack.h"
#include "core/gfx/Canvas.h"
#include "ui/StepSelection.h"

/**
 * @brief Implements the lfo page user-interface page.
 */
class LfoPage : public BasePage {
public:
    /**
     * @brief Constructs a LfoPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    LfoPage(PageManager &manager, PageContext &context);

    /**
     * @brief Activates the page/controller and initializes transient UI state.
     */
    void enter() override;
    /**
     * @brief Deactivates the page/controller and releases transient UI state.
     */
    void exit() override;
    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void draw(Canvas &canvas) override;
    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    void keyPress(KeyPressEvent &event) override;
    /**
     * @brief Handles an encoder movement or push action.
     *
     * @param[in] event Event to process.
     */
    void encoder(EncoderEvent &event) override;

private:
    // Note: do not cache a reference to the LfoTrack here because the
    // selected track can change; always access via _project.selectedTrack().lfoTrack()
    // -1 means no function key selected initially
    /**
     * @brief UI value representing selected param.
     */
    int _selectedParam = -1; ///< Selected LFO parameter index; `-1` means no parameter is selected.
    /**
     * @brief Number of param entries supported by the implementation.
     */
    static constexpr int ParamCount = CONFIG_FUNCTION_KEY_COUNT; ///< Number of param entries supported by the implementation.
    /**
     * @brief Adjusts the param from a UI edit delta.
     *
     * @param[in] param LFO parameter selected for editing.
     * @param[in] delta Signed edit delta applied to the selected parameter.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     * @param[in] encoderPressed Whether encoder pressed is enabled for this operation.
     */
    void editParam(int param, int delta, bool shift, bool encoderPressed = false);
    /**
     * @brief Draws params.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void drawParams(Canvas &canvas);
    /**
     * @brief Draws waveform.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    void drawWaveform(Canvas &canvas);
    StepSelection<16> _stepSelection; ///< Step-selection mask/range manipulated by this sequence-edit page.
};
