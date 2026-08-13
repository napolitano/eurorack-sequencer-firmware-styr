/**
 * @file Key.h
 * @brief Declares the Key component used by the sequencer UI.
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

#include "SystemConfig.h"

#include "MatrixMap.h"

#include <bitset>

typedef std::bitset<CONFIG_BLM_ROWS * CONFIG_BLM_COLS_BUTTON + 1> KeyState;

/**
 * @brief Represents a physical front-panel key and maps it to track/step/function roles.
 */
class Key {
public:
    // key codes
    /**
     * @brief Enumerates the supported code values.
     */
    enum Code {
        None = -1, ///< Represents no selection or an invalid value.
        Track0 = MatrixMap::fromTrack(0), ///< Selects the track0 code.
        Track1 = MatrixMap::fromTrack(1), ///< Selects the track1 code.
        Track2 = MatrixMap::fromTrack(2), ///< Selects the track2 code.
        Track3 = MatrixMap::fromTrack(3), ///< Selects the track3 code.
        Track4 = MatrixMap::fromTrack(4), ///< Selects the track4 code.
        Track5 = MatrixMap::fromTrack(5), ///< Selects the track5 code.
        Track6 = MatrixMap::fromTrack(6), ///< Selects the track6 code.
        Track7 = MatrixMap::fromTrack(7), ///< Selects the track7 code.
        Step0 = MatrixMap::fromStep(0), ///< Selects the step0 code.
        Step1 = MatrixMap::fromStep(1), ///< Selects the step1 code.
        Step2 = MatrixMap::fromStep(2), ///< Selects the step2 code.
        Step3 = MatrixMap::fromStep(3), ///< Selects the step3 code.
        Step4 = MatrixMap::fromStep(4), ///< Selects the step4 code.
        Step5 = MatrixMap::fromStep(5), ///< Selects the step5 code.
        Step6 = MatrixMap::fromStep(6), ///< Selects the step6 code.
        Step7 = MatrixMap::fromStep(7), ///< Selects the step7 code.
        Step8 = MatrixMap::fromStep(8), ///< Selects the step8 code.
        Step9 = MatrixMap::fromStep(9), ///< Selects the step9 code.
        Step10 = MatrixMap::fromStep(10), ///< Selects the step10 code.
        Step11 = MatrixMap::fromStep(11), ///< Selects the step11 code.
        Step12 = MatrixMap::fromStep(12), ///< Selects the step12 code.
        Step13 = MatrixMap::fromStep(13), ///< Selects the step13 code.
        Step14 = MatrixMap::fromStep(14), ///< Selects the step14 code.
        Step15 = MatrixMap::fromStep(15), ///< Selects the step15 code.
        Global0 = 24, ///< Selects the global0 code.
        Global1 = 25, ///< Selects the global1 code.
        Global2 = 26, ///< Selects the global2 code.
        Global3 = 27, ///< Selects the global3 code.
        Global4 = 28, ///< Selects the global4 code.
        Global5 = 29, ///< Selects the global5 code.
        Global6 = 30, ///< Selects the global6 code.
        Global7 = 31, ///< Selects the global7 code.
        Play = Global0, ///< Selects the play code.
        Tempo = Global1, ///< Selects the tempo code.
        Pattern = Global2, ///< Selects the pattern code.
        Performer = Global3, ///< Selects the performer code.
        Left = Global4, ///< Selects the left code.
        Right = Global5, ///< Selects the right code.
        Shift = Global6, ///< Selects the shift code.
        Page = Global7, ///< Selects the page code.
        F0 = MatrixMap::fromFunction(0), ///< Selects the f0 code.
        F1 = MatrixMap::fromFunction(1), ///< Selects the f1 code.
        F2 = MatrixMap::fromFunction(2), ///< Selects the f2 code.
        F3 = MatrixMap::fromFunction(3), ///< Selects the f3 code.
        F4 = MatrixMap::fromFunction(4), ///< Selects the f4 code.
        Encoder = 40, ///< Selects the encoder code.
    };

    /**
     * @brief Constructs a Key instance.
     *
     * @param[in] code Status, key or command code interpreted by the owning subsystem.
     * @param[in] state New state or logical level to apply.
     */
    Key(int code, const KeyState &state) : _code(code), _state(state) {}

    /**
     * @brief Returns the code.
     *
     * @return Physical/logical key code.
     */
    int code() const { return _code; }

    /**
     * @brief Returns the state.
     *
     * @return Reference to the state.
     */
    const KeyState &state() const { return _state; }
    /**
     * @brief Returns state.
     *
     * @param[in] code Status, key or command code interpreted by the owning subsystem.
     *
     * @return `true` if state; otherwise `false`.
     */
    bool state(int code) const { return _state[code]; }

    /**
     * @brief Shifts modifier.
     *
     * @return `true` if shift modifier; otherwise `false`.
     */
    bool shiftModifier() const { return _state[Shift]; }
    /**
     * @brief Returns the page modifier.
     *
     * @return `true` if page modifier; otherwise `false`.
     */
    bool pageModifier() const { return _state[Page]; }

    /**
     * @brief Returns is.
     *
     * @param[in] code Status, key or command code interpreted by the owning subsystem.
     *
     * @return `true` if is; otherwise `false`.
     */
    bool is(int code) const { return _code == code; }

    // individual buttons

    /**
     * @brief Reports whether play.
     *
     * @return `true` if play; otherwise `false`.
     */
    bool isPlay() const { return _code == Play; }
    /**
     * @brief Reports whether tempo.
     *
     * @return `true` if tempo; otherwise `false`.
     */
    bool isTempo() const { return _code == Tempo; }
    /**
     * @brief Reports whether pattern.
     *
     * @return `true` if pattern; otherwise `false`.
     */
    bool isPattern() const { return _code == Pattern; }
    /**
     * @brief Reports whether performer.
     *
     * @return `true` if performer; otherwise `false`.
     */
    bool isPerformer() const { return _code == Performer; }
    /**
     * @brief Reports whether left.
     *
     * @return `true` if left; otherwise `false`.
     */
    bool isLeft() const { return _code == Left; }
    /**
     * @brief Reports whether right.
     *
     * @return `true` if right; otherwise `false`.
     */
    bool isRight() const { return _code == Right; }
    /**
     * @brief Reports whether page.
     *
     * @return `true` if page; otherwise `false`.
     */
    bool isPage() const { return _code == Page; };
    /**
     * @brief Reports whether shift.
     *
     * @return `true` if shift; otherwise `false`.
     */
    bool isShift() const { return _code == Shift; }

    // button groups

    /**
     * @brief Reports whether track.
     *
     * @return `true` if track; otherwise `false`.
     */
    bool isTrack() const { return MatrixMap::isTrack(_code); }
    /**
     * @brief Returns the track.
     *
     * @return Track index addressed by this key.
     */
    int track() const { return MatrixMap::toTrack(_code); }

    /**
     * @brief Reports whether step.
     *
     * @return `true` if step; otherwise `false`.
     */
    bool isStep() const { return MatrixMap::isStep(_code); }
    /**
     * @brief Advances the Key by one logical step.
     *
     * @return Zero-based sequence-step index represented by the key/state.
     */
    int step() const { return MatrixMap::toStep(_code); }

    /**
     * @brief Reports whether function.
     *
     * @return `true` if function; otherwise `false`.
     */
    bool isFunction() const { return MatrixMap::isFunction(_code); }
    /**
     * @brief Returns the function.
     *
     * @return Zero-based function-key index represented by the key/state.
     */
    int function() const { return MatrixMap::toFunction(_code); }

    // combined buttons

    /**
     * @brief Reports whether track select.
     *
     * @return `true` if track select; otherwise `false`.
     */
    bool isTrackSelect() const { return !pageModifier() && isTrack(); }
    /**
     * @brief Returns the track select.
     *
     * @return Track-selection index addressed by this key.
     */
    int trackSelect() const { return track(); }

    /**
     * @brief Reports whether page select.
     *
     * @return `true` if page select; otherwise `false`.
     */
    bool isPageSelect() const { return pageModifier() && (isTrack() || (isStep() && step() < 8)); }
    /**
     * @brief Returns the page select.
     *
     * @return Page-selection index addressed by this key.
     */
    int pageSelect() const {
        if (isTrack()) {
            return track();
        } else if (isStep()) {
            return step() + 8;
        }
        return -1;
    }

    /**
     * @brief Reports whether quick edit.
     *
     * @return `true` if quick edit; otherwise `false`.
     */
    bool isQuickEdit() const { return pageModifier() && isStep() && step() >= 8; }
    /**
     * @brief Returns the quick edit.
     *
     * @return Quick-edit action addressed by this key.
     */
    int quickEdit() const { return step() - 8; }

    /**
     * @brief Reports whether context menu.
     *
     * @return `true` if context menu; otherwise `false`.
     */
    bool isContextMenu() const { return (is(Page) && shiftModifier()) || (is(Shift) && pageModifier()); }

    /**
     * @brief Reports whether encoder.
     *
     * @return `true` if encoder; otherwise `false`.
     */
    bool isEncoder() const { return is(Encoder); }

    /**
     * @brief Reports whether global.
     *
     * @return `true` if global; otherwise `false`.
     */
    bool isGlobal() const { return isPlay() || isTempo(); }

private:
    /**
     * @brief UI value representing code.
     */
    int _code; ///< Numeric key code identifying the physical or logical UI key.
    /**
     * @brief Reference to state owned by another component.
     */
    const KeyState &_state; ///< Reference to state owned by another component.
};
