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

class LfoPage : public BasePage {
public:
    LfoPage(PageManager &manager, PageContext &context);

    void enter() override;
    void exit() override;
    void draw(Canvas &canvas) override;
    void keyPress(KeyPressEvent &event) override;
    void encoder(EncoderEvent &event) override;

private:
    // Note: do not cache a reference to the LfoTrack here because the
    // selected track can change; always access via _project.selectedTrack().lfoTrack()
    // -1 means no function key selected initially
    int _selectedParam = -1;
    static constexpr int ParamCount = CONFIG_FUNCTION_KEY_COUNT;
    void editParam(int param, int delta, bool shift, bool encoderPressed = false);
    void drawParams(Canvas &canvas);
    void drawWaveform(Canvas &canvas);
    StepSelection<16> _stepSelection;
};
