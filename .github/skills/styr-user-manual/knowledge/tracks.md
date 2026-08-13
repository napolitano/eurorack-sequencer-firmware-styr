---
knowledge-id: styr.manual.tracks
status: canonical
locale: en-US
---

# Track model

## Note Track

A Note Track sequences gates and pitch-oriented note data. The maintained Note sequence supports up to 64 steps, shown 16 at a time on the Steps page.

User-facing step properties currently documented include:

- Gate
- Gate Probability
- Gate Offset
- Slide
- Retrigger
- Retrigger Probability
- Length
- Length Variation Range
- Length Variation Probability
- Note
- Note Variation Range
- Note Variation Probability
- Condition

Sequence-level controls currently documented include First Step, Last Step, Run Mode, Divisor, Reset Measure, Scale, and Root Note.

Source: `docs/manual/screens/note/README.md`.

### Negative Gate Offset

Styr implements the upstream TODO for signed Note Gate Offset. Negative offsets use deterministic look-ahead only when the future step is knowable. Forward/Backward loop wraps can be planned; uncertain future state is conservatively triggered at the boundary rather than guessed before it.

Known conservative boundaries include transport start, Reset Measure, linked tracks, pending pattern changes, dynamic Fill decisions, and non-deterministic run modes.

Source summary: root `README.md`, sections **Implemented upstream TODOs** and **Improvements**. Verify exact behavior against current tests before expanding the manual.

## Curve Track

A Curve Track uses the same 64-step sequencing structure as a Note Track, but each step describes a CV shape between minimum and maximum levels. It can also emit gates and apply shape variation probabilistically.

Documented step properties include:

- Shape
- Shape Variation
- Shape Variation Probability
- Minimum
- Maximum
- Gate
- Gate Probability

Sequence-level controls currently documented include First Step, Last Step, Run Mode, Divisor, Reset Measure, and Voltage Range.

Source: `docs/manual/screens/curve/README.md`.

## LFO Track

The LFO Track is a dedicated continuous modulation source rather than a conventional 64-step Note/Curve data editor.

Current maintained feature facts:

- modes: Sync and Free;
- eight documented waveforms: Sine, Triangle, Ramp Up, Ramp Down, Square, Random, Smoothed Random, Noise;
- Sync mode uses a step-based synchronized cycle length;
- Free mode exposes Frequency;
- Min and Max set the output limits;
- Square exposes Pulse Width;
- Clip is an alternate control;
- Nudge offsets synchronized waveform phase where supported.

The feature guide currently documents Sync Steps as 1–64 and Free Frequency as 0.00–20.00 Hz stored in 0.01 Hz increments. Before changing these values in published prose, verify them against current model/range definitions.

Source: `docs/manual/features/lfo/README.md` and `docs/manual/screens/lfo/README.md`.

## MIDI/CV Track

MIDI/CV is a distinct user-facing track type. Do not describe it as a Note Track with a different connector. Its current screen reference is `docs/manual/screens/tracks/midi-cv-track.md`; current behavior must be verified against the implementation/tests before expanding the high-level manual because the existing category guide is intentionally concise.
