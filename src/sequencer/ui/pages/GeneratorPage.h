/**
 * @file GeneratorPage.h
 * @brief Declares the GeneratorPage component used by the sequencer UI page.
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

#include "BasePage.h"

class Generator;
class EuclideanGenerator;
class RandomGenerator;
class AcidBasslineGenerator;

/**
 * @brief Implements the generator page user-interface page.
 */
class GeneratorPage : public BasePage {
public:
    /**
     * @brief Constructs a GeneratorPage instance.
     *
     * @param[in] manager Manager coordinating the associated UI or resource lifecycle.
     * @param[in] context Context object containing state required by the operation.
     */
    GeneratorPage(PageManager &manager, PageContext &context);

    using BasePage::show;
    /**
     * @brief Makes the page, dialog, or message visible.
     *
     * @param[in] generator Generator instance or generator type used to create sequence data.
     */
    void show(Generator *generator);

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
     * @brief Reports whether modal.
     *
     * @return `true` if modal; otherwise `false`.
     */
    virtual bool isModal() const override { return true; }

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
     * @brief Initializes the GeneratorPage and its runtime resources.
     */
    void init();
    /**
     * @brief Restores the previously saved or snapshotted state.
     */
    void revert();
    /**
     * @brief Commits the edited value/state.
     */
    void commit();

private:
    /**
     * @brief Draws euclidean generator.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] generator Generator instance or generator type used to create sequence data.
     */
    void drawEuclideanGenerator(Canvas &canvas, const EuclideanGenerator &generator) const;
    /**
     * @brief Draws random generator.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] generator Generator instance or generator type used to create sequence data.
     */
    void drawRandomGenerator(Canvas &canvas, const RandomGenerator &generator) const;
    /**
     * @brief Draws acid bassline generator.
     *
     * @param[in,out] canvas Canvas that receives drawing operations.
     * @param[in] generator Generator instance or generator type used to create sequence data.
     */
    void drawAcidBasslineGenerator(Canvas &canvas, const AcidBasslineGenerator &generator) const;

    /**
     * @brief Pointer to generator; `nullptr` denotes that no object/resource is assigned.
     */
    Generator *_generator; ///< Pointer to generator; `nullptr` denotes that no object/resource is assigned.

    std::pair<uint8_t, uint8_t> _valueRange; ///< Inclusive generator parameter range currently selected for editing.
};
