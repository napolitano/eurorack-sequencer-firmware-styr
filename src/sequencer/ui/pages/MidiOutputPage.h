/**
 * @file MidiOutputPage.h
 * @brief Declares the MidiOutputPage component used by the sequencer UI page.
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

#include "ListPage.h"

#include "ui/model/OutputListModel.h"

#include "model/MidiOutput.h"

#include "engine/MidiLearn.h"

class MidiOutputPage : public ListPage {
public:
    MidiOutputPage(PageManager &manager, PageContext &context);

    void reset();

    virtual void enter() override;
    virtual void exit() override;

    virtual void draw(Canvas &canvas) override;

    virtual void keyPress(KeyPressEvent &event) override;
    virtual void encoder(EncoderEvent &event) override;

private:
    void showOutput(int outputIndex);
    void selectOutput(int outputIndex);

    MidiOutput::Output _editOutput;
    OutputListModel _outputListModel;
    MidiOutput::Output *_output;
    uint8_t _outputIndex;
};
