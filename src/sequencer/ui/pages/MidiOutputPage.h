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

/**
 * @brief Implements the midi output page user-interface page.
 */
class MidiOutputPage : public ListPage {
public:
    /**
     * @brief Constructs a MidiOutputPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    MidiOutputPage(PageManager &manager, PageContext &context);

    /**
     * @brief Resets the MidiOutputPage to its initial runtime state.
     */
    void reset();

    /**
     * @brief Activates the page/controller and initializes transient UI state.
     */
    virtual void enter() override;
    /**
     * @brief Deactivates the page/controller and releases transient UI state.
     */
    virtual void exit() override;

    /**
     * @brief Draws this component into the supplied canvas or rendering target.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     */
    virtual void draw(Canvas &canvas) override;

    /**
     * @brief Handles a key-press event.
     *
     * @param[in] event Event to process.
     */
    virtual void keyPress(KeyPressEvent &event) override;
    /**
     * @brief Handles an encoder movement or push action.
     *
     * @param[in] event Event to process.
     */
    virtual void encoder(EncoderEvent &event) override;

private:
    /**
     * @brief Shows output.
     *
     * @param[in] outputIndex Zero-based output index.
     */
    void showOutput(int outputIndex);
    /**
     * @brief Selects output.
     *
     * @param[in] outputIndex Zero-based output index.
     */
    void selectOutput(int outputIndex);

    MidiOutput::Output _editOutput; ///< Working copy of the MIDI output being edited before it is committed.
    OutputListModel _outputListModel; ///< List model backing the output controls shown on this page.
    /**
     * @brief Pointer to output; `nullptr` denotes that no object/resource is assigned.
     */
    MidiOutput::Output *_output; ///< Pointer to output; `nullptr` denotes that no object/resource is assigned.
    /**
     * @brief Zero-based output index; a negative/sentinel value represents no selection where applicable.
     */
    uint8_t _outputIndex; ///< Zero-based output index; a negative/sentinel value represents no selection where applicable.
};
