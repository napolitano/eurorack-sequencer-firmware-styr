<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Core Concepts

## Project -> Track -> Pattern -> Sequence -> Step

Styr is easiest to use when you treat its data model as layers:

- **Project**: the full set (tracks, patterns, song, routing, system-relevant settings)
- **Track**: one musical lane (Note, Curve, LFO, or MIDI/CV)
- **Pattern**: one variation of all track states
- **Sequence**: the step timeline inside a track/pattern
- **Step**: one position that stores note or curve data (or contributes to LFO sync timing)

## Track types

- **Note Track**: pitch/gate sequencing with timing, probability, retrigger, and condition layers
- **Curve Track**: per-step CV shape with min/max and optional gate behavior
- **LFO Track**: continuous modulation waveform (sync or free-running)
- **MIDI/CV Track**: converts incoming MIDI into CV/gate voice outputs

## Play pattern vs edit pattern

Pattern playback and pattern editing can differ. This lets you prepare changes in another pattern, then switch when ready.

## Scope of settings

- **Project-wide**: tempo, swing, time signature, sync measure, input and record settings
- **Track-level**: output/play behavior for the selected track
- **Sequence-level**: first/last step, run mode, divisor, reset measure, musical context
- **Step-level**: gate/note/shape/probability/condition details

## Song Mode vs Performance Mode

- **Song Mode** arranges prepared patterns in slot order with repeats and optional per-track mutes.
- **Performance Mode** changes live playback state (mute/solo/fill timing) without rewriting the underlying sequence data.

Use Song for repeatable structure, Performance for live intervention.
