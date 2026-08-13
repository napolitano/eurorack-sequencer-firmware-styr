/**
 * @file Pages.h
 * @brief Declares the Pages component used by the sequencer UI page.
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

#include "BusyPage.h"
#include "ClockSetupPage.h"
#include "ConfirmationPage.h"
#include "ContextMenuPage.h"
#include "CurveSequenceEditPage.h"
#include "CurveSequencePage.h"
#include "FileSelectPage.h"
#include "GeneratorPage.h"
#include "GeneratorSelectPage.h"
#include "LayoutPage.h"
#include "LfoPage.h"
#include "MidiOutputPage.h"
#include "MonitorPage.h"
#include "NoteSequenceEditPage.h"
#include "NoteSequencePage.h"
#include "OverviewPage.h"
#include "PatternPage.h"
#include "PerformerPage.h"
#include "ProjectPage.h"
#include "QuickEditPage.h"
#include "RoutingPage.h"
#include "SongPage.h"
#include "StartupPage.h"
#include "SystemPage.h"
#include "TempoPage.h"
#include "TextInputPage.h"
#include "TopPage.h"
#include "TrackPage.h"
#include "UserScalePage.h"

#ifdef CONFIG_ENABLE_INTRO
#include "IntroPage.h"
#endif
#ifdef CONFIG_ENABLE_ASTEROIDS
#include "AsteroidsPage.h"
#endif

/**
 * @brief Owns the concrete UI page instances used by the sequencer page manager.
 */
struct Pages {
    TopPage top; ///< Page instance for the top screen.
    // main pages
    ProjectPage project; ///< Active project model used by this component.
    LayoutPage layout; ///< Page instance for the layout screen.
    TrackPage track; ///< Persistent track model operated on by this component.
    NoteSequencePage noteSequence; ///< Page instance for the note sequence screen.
    CurveSequencePage curveSequence; ///< Page instance for the curve sequence screen.
    NoteSequenceEditPage noteSequenceEdit; ///< Page instance for the note sequence edit screen.
    CurveSequenceEditPage curveSequenceEdit; ///< Page instance for the curve sequence edit screen.
    PatternPage pattern; ///< Page instance for the pattern screen.
    PerformerPage performer; ///< Page instance for the performer screen.
    SongPage song; ///< Page instance for the song screen.
    RoutingPage routing; ///< Page instance for the routing screen.
    MidiOutputPage midiOutput; ///< Page instance for the midi output screen.
    UserScalePage userScale; ///< Page instance for the user scale screen.
    OverviewPage overview; ///< Page instance for the overview screen.
    SystemPage system; ///< Page instance for the system screen.
    // modal pages
    TempoPage tempo; ///< Page instance for the tempo screen.
    ClockSetupPage clockSetup; ///< Page instance for the clock setup screen.
    MonitorPage monitor; ///< Page instance for the monitor screen.
    FileSelectPage fileSelect; ///< Page instance for the file select screen.
    ContextMenuPage contextMenu; ///< Page instance for the context menu screen.
    QuickEditPage quickEdit; ///< Page instance for the quick edit screen.
    // generator pages
    GeneratorPage generator; ///< Page instance for the generator screen.
    GeneratorSelectPage generatorSelect; ///< Page instance for the generator select screen.
    // helper pages
    TextInputPage textInput; ///< Page instance for the text input screen.
    ConfirmationPage confirmation; ///< Page instance for the confirmation screen.
    BusyPage busy; ///< Page instance for the busy screen.
    // misc
    StartupPage startup; ///< Page instance for the startup screen.
	LfoPage lfo; ///< Page instance for the lfo screen.
#ifdef CONFIG_ENABLE_INTRO
    IntroPage intro; ///< Page instance for the intro screen.
#endif
#ifdef CONFIG_ENABLE_ASTEROIDS
    AsteroidsPage asteroids; ///< Page instance for the asteroids screen.
#endif

    /**
     * @brief Constructs a Pages instance.
     *
     * @param[in] manager Manager used by the operation.
     * @param[in] context Context used by the operation.
     */
    Pages(PageManager &manager, PageContext &context) :
        top(manager, context),
        // main pages
        project(manager, context),
        layout(manager, context),
        track(manager, context),
        noteSequence(manager, context),
        curveSequence(manager, context),
        noteSequenceEdit(manager, context),
        curveSequenceEdit(manager, context),
        pattern(manager, context),
        performer(manager, context),
        song(manager, context),
        routing(manager, context),
        midiOutput(manager, context),
        userScale(manager, context),
        overview(manager, context),
        system(manager, context),
        // modal pages
        tempo(manager, context),
        clockSetup(manager, context),
        monitor(manager, context),
        fileSelect(manager, context),
        contextMenu(manager, context),
        quickEdit(manager, context),
        // generator pages
        generator(manager, context),
        generatorSelect(manager, context),
        // helper pages
        textInput(manager, context),
        confirmation(manager, context),
        /**
         * @brief Executes busy.
         *
         * @note Includes an unnamed `manager` input parameter as declared by the inherited/interface signature.
         * @note Includes an unnamed `context` input parameter as declared by the inherited/interface signature.
         */
        busy(manager, context),
        // misc
        startup(manager, context),
		lfo(manager, context),
#ifdef CONFIG_ENABLE_INTRO
        intro(manager, context),
#endif
#ifdef CONFIG_ENABLE_ASTEROIDS
        asteroids(manager, context)
#endif
    {}
};

