/**
 * @file ProjectVersion.h
 * @brief Declares the ProjectVersion component used by the sequencer model.
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
enum ProjectVersion {
    // added NoteTrack::cvUpdateMode
    Version4 = 4, ///< Selects the version4 project version.

    // added storing user scales with project
    // added Project::name
    // added UserScale::name
    Version5 = 5, ///< Selects the version5 project version.

    // added Project::cvGateInput
    Version6 = 6, ///< Selects the version6 project version.

    // added NoteSequence::Step::gateOffset
    Version7 = 7, ///< Selects the version7 project version.

    // added CurveTrack::slideTime
    Version8 = 8, ///< Selects the version8 project version.

    // added MidiCvTrack::arpeggiator
    Version9 = 9, ///< Selects the version9 project version.

    // expanded divisor to 16 bits
    Version10 = 10, ///< Selects the version10 project version.

    // added ClockSetup::clockOutputSwing
    // added Project::curveCvInput
    Version11 = 11, ///< Selects the version11 project version.

    // added TrackState::fillAmount
    // added NoteSequence::Step::condition
    Version12 = 12, ///< Selects the version12 project version.

    // added Routing::MidiSource::Event::NoteRange
    Version13 = 13, ///< Selects the version13 project version.

    // swapped Curve::Type::Low with Curve::Type::High
    Version14 = 14, ///< Selects the version14 project version.

    // added MidiCvTrack::lowNote/highNote
    // changed CurveSequence::Step layout
    // added CurveTrack::shapeProbabilityBias
    // added CurveTrack::gateProbabilityBias
    Version15 = 15, ///< Selects the version15 project version.

    // added MidiCvTrack::notePriority
    Version16 = 16, ///< Selects the version16 project version.

    // changed Arpeggiator::octaves
    Version17 = 17, ///< Selects the version17 project version.

    // added Project::timeSignature
    Version18 = 18, ///< Selects the version18 project version.

    // expanded Song::slots to 64 entries
    Version19 = 19, ///< Selects the version19 project version.

    // added MidiCvTrack::slideTime
    Version20 = 20, ///< Selects the version20 project version.

    // added MidiCvTrack::transpose
    Version21 = 21, ///< Selects the version21 project version.

    // added CurveTrack::muteMode
    Version22 = 22, ///< Selects the version22 project version.

    // added Route::Target::Scale and Route::Target::RootNote
    Version23 = 23, ///< Selects the version23 project version.

    // expanded MidiOutput::outputs to 16 entries
    Version24 = 24, ///< Selects the version24 project version.

    // added Song::Slot::mutes
    Version25 = 25, ///< Selects the version25 project version.

    // added NoteTrack::fillMuted
    Version26 = 26, ///< Selects the version26 project version.

    // expanded NoteSequence::Step to 64 bits, expanded NoteSequence::Step::Condition to 7 bits
    Version27 = 27, ///< Selects the version27 project version.

    // added CurveTrack::offset
    Version28 = 28, ///< Selects the version28 project version.

    // added Project::midiInput
    Version29 = 29, ///< Selects the version29 project version.

    // added Project::monitorMode
    Version30 = 30, ///< Selects the version30 project version.

    // changed MidiCvTrack::VoiceConfig to 8-bit value
    Version31 = 31, ///< Selects the version31 project version.

    // added LfoTrack persistence (new LFO track fields persisted)
    Version32 = 32, ///< Selects the version32 project version.

    // automatically derive latest version
    Last, ///< Sentinel marking the end of the valid enumeration range.
    Latest = Last - 1, ///< Selects the latest project version.
};
