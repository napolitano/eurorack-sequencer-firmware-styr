/**
 * @file CurveSequenceEditPage.h
 * @brief Declares the CurveSequenceEditPage component used by the sequencer UI page.
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

#include "Config.h"

#include "BasePage.h"

#include "ui/StepSelection.h"
#include "ui/model/CurveSequenceListModel.h"

#include "engine/generators/SequenceBuilder.h"

#include "core/utils/Container.h"

/**
 * @brief Implements the curve sequence edit page user-interface page.
 */
class CurveSequenceEditPage : public BasePage {
public:
    /**
     * @brief Constructs a CurveSequenceEditPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    CurveSequenceEditPage(PageManager &manager, PageContext &context);

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
     * @brief Refreshes LED state from the current model and UI state.
     *
     * @param[in] leds LED-state buffer or mask updated by the operation.
     */
    virtual void updateLeds(Leds &leds) override;

    /**
     * @brief Reports whether a key is currently held.
     *
     * @param[in] event Event to process.
     */
    virtual void keyDown(KeyEvent &event) override;
    /**
     * @brief Reports whether a key is currently released.
     *
     * @param[in] event Event to process.
     */
    virtual void keyUp(KeyEvent &event) override;
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
    typedef CurveSequence::Layer Layer;

    /**
     * @brief Number of step entries supported by the implementation.
     */
    static const int StepCount = 16; ///< Number of step entries supported by the implementation.

    /**
     * @brief Returns the step offset.
     *
     * @return Current edited step offset.
     */
    int stepOffset() const { return _section * StepCount; }

    /**
     * @brief Changes the active controller/display layer.
     *
     * @param[in] functionKey Function-key identifier or state used by the UI.
     * @param[in] shift UI modifier or coarse-adjustment value supplied by the caller.
     */
    void switchLayer(int functionKey, bool shift);
    /**
     * @brief Returns the active function key.
     *
     * @return Currently held function-key index, or the page-defined sentinel when none is active.
     */
    int activeFunctionKey();

    /**
     * @brief Updates the step used for live monitoring.
     */
    void updateMonitorStep();
    /**
     * @brief Draws detail.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] step Zero-based sequence-step index.
     * @param[in] stepIndex Zero-based step index.
     */
    void drawDetail(Canvas &canvas, const CurveSequence::Step &step, int stepIndex);

    /**
     * @brief Opens the context menu for the current selection.
     */
    void contextShow();
    /**
     * @brief Executes the selected context-menu action.
     *
     * @param[in] index Zero-based context action index.
     */
    void contextAction(int index);
    /**
     * @brief Returns context action enabled.
     *
     * @param[in] index Zero-based context action enabled index.
     *
     * @return `true` if context action enabled; otherwise `false`.
     */
    bool contextActionEnabled(int index) const;

    /**
     * @brief Initializes a sequence with required default state.
     */
    void initSequence();
    /**
     * @brief Copies sequence.
     */
    void copySequence();
    /**
     * @brief Pastes sequence.
     */
    void pasteSequence();
    /**
     * @brief Copies one sequence into another sequence slot.
     */
    void duplicateSequence();
    /**
     * @brief Generates sequence data from the configured generator parameters.
     */
    void generateSequence();

    /**
     * @brief Applies the page-specific quick-edit action.
     *
     * @param[in] index Zero-based quick edit index.
     */
    void quickEdit(int index);

    /**
     * @brief Returns the layer.
     *
     * @return Currently selected edit layer.
     */
    CurveSequence::Layer layer() const { return _project.selectedCurveSequenceLayer(); }
    /**
     * @brief Sets the layer.
     *
     * @param[in] layer Layer index or layer identifier.
     */
    void setLayer(CurveSequence::Layer layer) { _project.setSelectedCurveSequenceLayer(layer); }

    ContextMenu _contextMenu; ///< Context-menu model/state for the current page.

    /**
     * @brief UI value representing section.
     */
    int _section = 0; ///< Zero-based edit-section index currently shown on the sequence edit page.
    /**
     * @brief Whether show detail is true in the current state.
     */
    bool _showDetail; ///< True while the temporary per-step detail overlay is visible.
    /**
     * @brief UI value representing show detail ticks.
     */
    uint32_t _showDetailTicks; ///< Scheduler tick at which the temporary detail overlay should stop being shown.

    CurveSequenceListModel _listModel; ///< List model backing the list model controls shown on this page.

    StepSelection<CONFIG_STEP_COUNT> _stepSelection; ///< Step-selection mask/range manipulated by this sequence-edit page.

    Container<CurveSequenceBuilder> _builderContainer; ///< In-place variant storage that owns the active builder object without heap allocation.
};
