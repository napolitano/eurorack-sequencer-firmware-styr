/**
 * @file SystemPage.h
 * @brief Declares the SystemPage component used by the sequencer UI page.
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

#include "ui/model/CalibrationCvOutputListModel.h"
#include "ui/model/UtilitiesListModel.h"
#ifdef CONFIG_ADVANCED_SETTINGS
#include "ui/model/AdvancedSettingsListModel.h"
#endif

#include "model/Settings.h"

/**
 * @brief Implements the system page user-interface page.
 */
class SystemPage : public ListPage {
public:
    /**
     * @brief Constructs a SystemPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    SystemPage(PageManager &manager, PageContext &context);

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
    /**
     * @brief Enumerates the supported mode values.
     */
    enum class Mode : uint8_t {
        Calibration = 0, ///< Selects the calibration mode.
#ifdef CONFIG_ADVANCED_SETTINGS
        Advanced    = 2, ///< Selects the advanced mode.
#endif
        Utilities   = 3, ///< Selects the utilities mode.
        Update      = 4, ///< Selects the update mode.
    };

    /**
     * @brief Sets the mode.
     *
     * @param[in] mode Mode to select.
     */
    void setMode(Mode mode);
    /**
     * @brief Sets the output index.
     *
     * @param[in] index Zero-based output index index.
     */
    void setOutputIndex(int index);
    /**
     * @brief Refreshes the component output values.
     */
    void updateOutputs();

    /**
     * @brief Executes the selected utility-menu action.
     *
     * @param[in] item Item or list entry addressed by the operation.
     */
    void executeUtilityItem(UtilitiesListModel::Item item);

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
     * @brief Initializes settings.
     */
    void initSettings();
    /**
     * @brief Saves settings.
     */
    void saveSettings();
    /**
     * @brief Backs up current settings.
     */
    void backupSettings();
    /**
     * @brief Restores previously backed-up settings.
     */
    void restoreSettings();
    /**
     * @brief Formats the sd card for display.
     */
    void formatSdCard();

    /**
     * @brief Saves settings to flash.
     */
    void saveSettingsToFlash();
    /**
     * @brief Writes a settings backup to a file.
     */
    void backupSettingsToFile();
    /**
     * @brief Restores settings from a backup file.
     */
    void restoreSettingsFromFile();

    Mode _mode = Mode::Calibration; ///< Active mode controlling the behavior of `SystemPage`.
    /**
     * @brief Reference to settings owned by another component.
     */
    Settings &_settings; ///< Reference to settings owned by another component.

    /**
     * @brief Zero-based output index; a negative/sentinel value represents no selection where applicable.
     */
    int _outputIndex; ///< Zero-based output index; a negative/sentinel value represents no selection where applicable.
    CalibrationCvOutputListModel _cvOutputListModel; ///< List model backing the cv output controls shown on this page.
    UtilitiesListModel _utilitiesListModel; ///< List model backing the utilities controls shown on this page.
#ifdef CONFIG_ADVANCED_SETTINGS
    AdvancedSettingsListModel _advancedListModel; ///< List model backing the advanced controls shown on this page.
#endif
    /**
     * @brief UI value representing encoder down ticks.
     */
    uint32_t _encoderDownTicks; ///< UI value representing encoder down ticks.
};
