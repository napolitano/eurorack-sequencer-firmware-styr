<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Recording Sequences

Use [Project](../screens/global/project.md) to set:

- **Monitor Mode** (`Always`, `Stopped`, `Off`)
- **Record Mode** (`Step Record`, `Overdub`, `Overwrite`)
- input sources for MIDI, CV/Gate, and Curve CV

Recording is toggled with `PAGE` + `PLAY`.

## Choose a recording mode first

| Goal | Mode |
|---|---|
| Enter one note at a time in a controlled order | **Step Record** |
| Play a new phrase over a running loop while keeping untouched steps | **Overdub** |
| Replace the current pass deliberately | **Overwrite** |

## Which track receives input

- Note-style recording follows the currently selected track after project input filtering.
- Curve recording applies to the selected Curve Track when a Curve CV input is configured.

## Step Record (recommended first)

Use **Step Record** when you want controlled, sequential note entry:

1. Set loop range and divisor first.
2. Select the Note Track to record.
3. Enable recording.
4. Play one note at a time; releasing the note advances to the next step.
5. After capture, refine timing/probability/conditions with normal step editing.

Step Record is best for building a clean sequence skeleton, not realtime timing capture.

## Overdub (realtime additive capture)

Use **Overdub** to add a played phrase to a running loop while preserving untouched steps.

Realtime capture is grid-based by step; use Gate Offset afterward when you want deliberate ahead/behind-grid movement.

## Overwrite (deliberately destructive)

Use **Overwrite** when you intend to replace the existing pass.

As playback passes unwritten selected-track steps, those steps are cleared. Treat Overwrite as a replacement workflow, not a safe default.

::: {custom-style="StyrNote"}
**NOTE**

Use Overdub when you want to keep existing material unless you explicitly play over it.
:::

## Curve recording

Curve recording captures step-based CV behavior on Curve Tracks:

1. Select the Curve Track.
2. Configure Curve CV input and range.
3. Start playback and recording.
4. Perform the incoming CV over complete steps.
5. Review inferred Shape/Min/Max and refine manually where needed.

Each step stores Styr’s supported curve representation, not a full high-resolution continuous recording.
