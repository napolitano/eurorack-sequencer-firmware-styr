/**
 * @file PageKeyMap.h
 * @brief Declares the PageKeyMap component used by the sequencer UI.
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

#include "Key.h"

//  Project     Layout      Routing     MidiOutput  UserScale   -       -       System
//  SequenceEdt Sequence    Track       Song        -           -       -       Monitor

namespace PageKeyMap {

    /**
     * @brief Enumerates the supported page values.
     */
    enum Page {
        Clock           = Key::Tempo, ///< Selects the clock page.
        Pattern         = Key::Pattern, ///< Selects the pattern page.
        Performer       = Key::Performer, ///< Selects the performer page.
        Overview        = Key::Left, ///< Selects the overview page.

        Project         = Key::Track0, ///< Selects the project page.
        Layout          = Key::Track1, ///< Selects the layout page.
        Routing         = Key::Track2, ///< Selects the routing page.
        MidiOutput      = Key::Track3, ///< Selects the midi output page.
        UserScale       = Key::Track4, ///< Selects the user scale page.

        SequenceEdit    = Key::Step0, ///< Selects the sequence edit page.
        Sequence        = Key::Step1, ///< Selects the sequence page.
        Track           = Key::Step2, ///< Selects the track page.
        Song            = Key::Step3, ///< Selects the song page.

        System          = Key::Track7, ///< Selects the system page.
        Monitor         = Key::Step7, ///< Selects the monitor page.
    };

    /**
     * @brief Reports whether a key code is assigned to direct page navigation.
     *
     * @param[in] code Input key code to classify.
     *
     * @return `true` when the documented condition is satisfied; otherwise `false`.
     */
    static bool isPageKey(int code) {
        switch (code) {
        case Clock:
        case Pattern:
        case Performer:
        case Overview:

        case Project:
        case Layout:
        case Routing:
        case MidiOutput:
        case UserScale:

        case SequenceEdit:
        case Sequence:
        case Track:
        case Song:

        case System:
        case Monitor:
            return true;
        default:
            break;
        }

        return false;
    }

} // namespace PageKeyMap
