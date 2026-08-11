<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# LFO Screen Reference

The Styr LFO track is a dedicated modulation source rather than a conventional 64-step Note/Curve editor. Its page shows the waveform and exposes the controls directly through the five function keys.

For documentation captures, the resolved LFO output range is forced to **−5.00 V … +5.00 V** so each waveform uses the full vertical display area. Both the base and routed value lanes are set explicitly before capture.

## Normal function layer

| Function key | Sync mode | Free mode |
| --- | --- | --- |
| `F1` | [Shape](lfo-shape.md) | [Shape](lfo-shape.md) |
| `F2` | [Steps](lfo-steps.md) | [Frequency](lfo-frequency.md) |
| `F3` | [Mode](lfo-mode.md) | [Mode](lfo-mode.md) |
| `F4` | [Min](lfo-min.md) | [Min](lfo-min.md) |
| `F5` | [Max](lfo-max.md) | [Max](lfo-max.md) |

- [Sync overview](lfo-sync.md)
- [Free overview](lfo-free.md)

## Waveforms

Every supported waveform has a separate reference state:

- [Sine](lfo-shape-sine.md)
- [Triangle](lfo-shape-triangle.md)
- [Ramp Up](lfo-shape-ramp-up.md)
- [Ramp Down](lfo-shape-ramp-down.md)
- [Square](lfo-shape-square.md)
- [Random](lfo-shape-random.md)
- [Smoothed Random](lfo-shape-smoothed-random.md)
- [Noise](lfo-shape-noise.md)

## Shift layer

Hold `SHIFT` to expose the alternate controls:

- [`SHIFT` + `F1` — Pulse Width](lfo-pulse-width.md)
- [`SHIFT` + `F2` — Clip](lfo-clip.md)
- [`SHIFT` + `F5` — Nudge](lfo-nudge.md)
- [Shift-layer overview](lfo-shift.md)

The alternate controls are shape/mode dependent; for example Pulse Width is meaningful for Square, while Nudge offsets the synchronized waveform phase.

From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
