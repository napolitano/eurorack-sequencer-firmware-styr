<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Performance Mode

Performance Mode is for live playback control, not sequence rewriting.

Open it with `PERF` (temporary hold) or through page navigation. See [Performance screen reference](../screens/global/performance.md).

## Mute and solo actions

- Track key: toggle mute
- `SHIFT` + track key: solo
- **Unmute**: clear mutes

## Immediate vs Latch vs Sync

Use these as timing choices for the same decision:

- **Immediate**: execute now
- **Latch**: collect changes and commit as one group
- **Sync**: queue changes for execution at the sync boundary

Use **Cancel** to clear pending requests.

Example transition:

- **Immediate**: mute drums now for a hard cut.
- **Latch**: stage "mute drums + solo bass", then commit both together.
- **Sync**: queue that same change so it lands on the next configured sync boundary.

## Fill workflow

Performance Fill controls **when Fill is active**. Each track’s own Fill settings define **what Fill does** musically.

- Trigger fill per track or globally
- Hold/shift behavior supports held Fill interactions
- Adjust per-track Fill Amount by holding the track’s fill selector and turning the encoder

## Working with Song Mode

Run Song for the arranged structure, then use Performance for interventions (fills, mutes, solos, staged transitions).
If Song mutes exist for a track, later slot changes can reapply those arrangement mutes.
