<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Performance

![Performance](../../assets/performance.png)

## Function

Provides live mute/solo/fill control with immediate, latched, or synchronized execution timing.

## Access

Hold `PERF` for temporary access or open the corresponding page through the page selector.

## Operation

Track keys toggle mute. `SHIFT` + track keys solo tracks.
Function keys provide:

- **Latch**: stage mute/solo changes and commit them together
- **Sync**: queue changes for execution at the sync boundary
- **Unmute**: clear all mutes
- **Fill**: activate fill behavior (global/per-track)
- **Cancel**: remove pending mute requests

Use Fill with track step keys to target fills per track.
Hold a track fill selector and turn the encoder to adjust Fill Amount.

Performance Mode changes runtime playback state and does not rewrite underlying step data.


From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
