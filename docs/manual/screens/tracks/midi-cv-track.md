<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# MIDI/CV Track

![MIDI/CV Track](../../assets/midi-cv-track.png)

## Function

Configures a track as a MIDI/CV voice allocator and converter.

## Access

Select a MIDI/CV track and press `PAGE` + `S3` (`TRACK`).

## Operation

The page covers the complete MIDI/CV voice setup:

- **Source** — DIN MIDI or USB, plus the input channel; channels can be set to Omni.
- **Voices** — polyphony from `1` to `8`.
- **Voice Config** — Pitch, Velocity, Pitch+Velocity, or Pitch+Velocity+Pressure outputs per voice.
- **Note Priority** — Last Note, First Note, Lowest Note, or Highest Note when notes compete for available voices.
- **Low Note / High Note** — accepted MIDI-note range.
- **Pitch Bend** — bend span from `Off` to `48` semitones.
- **Mod Range** — voltage range used for modulation/velocity/pressure-style outputs.
- **Retrigger** — whether a new note briefly retriggers the gate instead of staying legato.
- **Slide Time** — glide time from `0` to `100`.
- **Transpose** — track transposition.
- **Arpeggiator** — Enabled, Hold, Mode, Divisor, Gate Length, and Octaves.

Physical CV/gate placement is configured separately on the Layout page.


From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
