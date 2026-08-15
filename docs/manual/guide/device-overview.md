<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Device Overview and Navigation

Styr is an eight-track Eurorack sequencer that can drive CV, gate, and MIDI workflows from one project. You can sequence notes, modulation curves, and LFO modulation, or convert incoming MIDI to CV/gate voices.

## What you control from one project

Each project holds:

- 8 tracks
- 16 patterns
- up to 64 steps per sequence
- up to 64 Song slots
- routing, MIDI output mapping, and user scales

## Display regions

Styr’s 256×64 display is split into:

- **Header**: transport/record state, track and pattern context, current page
- **Body**: the active editor (steps, lists, song table, routing, system pages)
- **Footer**: live labels for `F1`…`F5`

See [Display Layout](../screens/interface/README.md), [Header](../screens/interface/header.md), and [Footer](../screens/interface/footer.md).

## Core navigation gestures

- Hold `PAGE` and press a page key to jump to a page.
- Hold `PATT` for temporary Pattern page access.
- Hold `PERF` for temporary Performance page access.
- Press a track key to select the active track.
- Press `PLAY` to start/stop transport.
- Press `SHIFT` + `PLAY` for the alternate transport action set by Clock **Shift Mode** (**Restart** or **Pause/Continue**).
- Press `PAGE` + `PLAY` to toggle recording.

Global pages include Project, Layout, Track, Sequence, Steps, Song, Routing, MIDI Output, User Scale, Monitor, and System.

## LFO-track navigation exception

LFO Tracks do not use the Note/Curve Sequence page. When an LFO Track is selected, use the LFO page directly for waveform, rate, and range editing.
