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

    enum Page {
        Clock           = Key::Tempo,
        Pattern         = Key::Pattern,
        Performer       = Key::Performer,
        Overview        = Key::Left,

        Project         = Key::Track0,
        Layout          = Key::Track1,
        Routing         = Key::Track2,
        MidiOutput      = Key::Track3,
        UserScale       = Key::Track4,

        SequenceEdit    = Key::Step0,
        Sequence        = Key::Step1,
        Track           = Key::Step2,
        Song            = Key::Step3,

        System          = Key::Track7,
        Monitor         = Key::Step7,
    };

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
