<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Song Mode

Song Mode arranges patterns into up to 64 slots. Each slot stores:

- repeats
- one pattern assignment per track
- per-track song mute state

See [Song screen reference](../screens/song/README.md).

## Basic arrangement workflow

1. Build a few useful patterns first.
2. Open [Song](../screens/song/song.md).
3. Add slots and assign patterns globally.
4. Use track-held edits when one track needs a different pattern than the rest.
5. Set repeats instead of duplicating identical slots.
6. Start playback from Song page.

## Useful controls

- **Chain**: quickly append pattern-based slots
- **Add** / `SHIFT`+Add (**Insert**)
- **Remove**
- **Duplicate**
- **Play/Stop** (`F5`)

`SHIFT` + `F5` (Play) while clock is already running requests a synced Song start.

## Playback behavior that matters

- Song advances at measure boundaries, following each slot's repeat count, then returns to the first active slot after the last active slot.
- Synced Song start/stop requests follow the configured Sync Measure boundary.
- Direct pattern changes outside the Song flow stop Song playback.
- Song-slot mutes only reassert on tracks that have Song mutes configured in the Song.

That last point matters in live sets: if a track has no Song mutes in the arrangement, its live mute state can carry across slot changes.
