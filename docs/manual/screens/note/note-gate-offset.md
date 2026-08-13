<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Note Steps — Gate Offset

![Note Steps — Gate Offset](../../assets/note-gate-offset.png)

## Function

Moves a Note step earlier or later within the step for per-step microtiming.

## Access

Select a Note track, open `PAGE` + `S1` (`STEPS`), then select this layer with the corresponding function key.

## Operation

Hold one or more steps and turn the encoder. `0` leaves the gate on the normal step boundary, positive values delay it, and negative values pre-trigger it. The current range is `-7` to `+7`.

Gate Offset is scaled against the step duration, so the same setting produces a larger time shift on longer step lengths/divisors. The sequence grid itself does not move.

## Notes

Function keys can group several layers. Repeated presses cycle within a group; `SHIFT` + the function key returns directly to the group’s primary layer.

Negative Gate Offset fires early only when Styr can determine the upcoming step without guessing. Deterministic forward/backward playback, including a normal loop wrap, can pre-trigger exactly. At causal boundaries such as transport start, Reset Measure, pending pattern changes, linked tracks, Fill-dependent decisions, or non-deterministic run modes, Styr falls back to the boundary instead of inventing an earlier event.


From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
