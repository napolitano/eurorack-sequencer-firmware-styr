---
knowledge-id: styr.manual.song-and-performance
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Song Mode and Performance Mode

Song Mode and Performance Mode are different user workflows built on the same prepared project material. The manual must explain them as such rather than treating them as two isolated screens.

## Song Mode — verified product model

Current product constants provide 16 patterns, 8 tracks, and 64 song slots (`src/sequencer/Config.h`).

Each active Song slot stores:

- one pattern assignment per track;
- one mute state per track;
- a repeat count.

Source: `src/sequencer/model/Song.h` / `Song.cpp`.

### Editing behavior

The current Song page supports, among other operations:

- Add / Shift+Add (Insert);
- Remove;
- Duplicate;
- per-slot repeat editing;
- a global pattern assignment for a slot;
- per-track pattern overrides by holding/selecting track keys while assigning/editing a pattern;
- per-track song mutes;
- reordering slots with Shift + left/right;
- Chain mode for quickly appending global patterns;
- Play/Stop and starting a selected slot;
- synchronized song start when requested while the clock is already running and the Shift-modified action is used.

Source: `src/sequencer/ui/pages/SongPage.cpp`.

### Playback behavior

When Song playback starts, Styr activates the selected slot's patterns and applicable song mutes. Song playback starts the clock if it is not already running.

While playing:

- slot repeats advance on measure boundaries;
- after the configured repeat count, playback advances to the next slot;
- after the final active slot, playback wraps to slot 0;
- track engines restart when the song moves to a new slot;
- a direct pattern-change request outside the Song flow stops Song playback.

Synchronized Song requests execute at the project's configured synchronization boundary (`Sync Measure`), not at an arbitrary UI delay.

Source: `src/sequencer/engine/Engine.cpp` / `PlayState.cpp`.

### Important mute interaction

A Song slot's mute value is applied for a track only if that track has at least one Song mute anywhere in the current Song. If a track has no Song mutes at all, Song slot changes leave that track's current live mute state alone.

This behavior matters when combining a structured Song with Performance Mode and must be explained in user terms.

## Recommended Song workflow

This is an editorial recommendation based on the verified model.

1. Build a small vocabulary of useful patterns before arranging. Treat patterns as reusable musical states, not as one-off song slots.
2. Start Song construction with global pattern assignments when most tracks change together.
3. Use per-track pattern overrides only where one track needs to continue or vary independently. This keeps the arrangement readable.
4. Use slot Repeat counts when a musical state simply needs to last longer instead of duplicating identical slots repeatedly.
5. Use Song mutes intentionally as arrangement automation. If a track should remain under live Performance mute control across the entire Song, avoid adding Song mutes for that track unless the slot transitions should deliberately reassert mute state.
6. When auditioning a transition against a running clock, prefer the synchronized start/request where the musical intent is to enter on the configured Sync Measure boundary.
7. Keep a short example arrangement in the manual that demonstrates: same pattern repeated, one per-track override, one mute change, and a return/wrap.

The Song chapter should explain the arrangement model before presenting every key gesture.

## Performance Mode — verified product model

Performance Mode operates on eight tracks and exposes performance actions without changing the underlying sequence data.

Current behavior includes:

- track key: toggle mute;
- `SHIFT` + track key: solo that track;
- `Unmute`: unmute all tracks;
- immediate execution by default;
- `Latch`: collect mute/solo requests and commit them as a group when the latch interaction is completed;
- `Sync`: queue mute/solo requests for the configured synchronization boundary;
- `Cancel`: cancel pending Performance mute requests;
- per-track Fill triggering;
- global Fill triggering;
- held/latching Fill behavior using Shift;
- per-track Fill Amount editing by holding the corresponding fill-amount selector and turning the encoder.

Source: `src/sequencer/ui/pages/PerformerPage.cpp`, `src/sequencer/model/PlayState.h` / `PlayState.cpp`.

### Immediate, Latch, and Sync mental model

Teach these as three ways to execute the same performance decision:

- **Immediate** — make the mute/solo change now.
- **Latch** — prepare several mute/solo changes, then commit the group together.
- **Sync** — prepare changes that should take effect on Styr's configured synchronization boundary.

A musical example is more useful than three abstract definitions. For example: prepare a transition by muting drums and soloing a melodic track, then compare Immediate versus Latch versus Sync behavior.

### Fill mental model

A Fill is a performance request whose musical result depends on each track's configured Fill mode and Fill Amount. Performance Mode decides **when/where Fill is active**; the track configuration determines **what Fill means** for that track.

The manual must connect those two concepts rather than describing the Performance Fill keys in isolation.

## Recommended live workflow

A useful manual recommendation is:

1. Prepare patterns and per-track Fill behavior before the performance.
2. Use Song Mode when the broad arrangement should advance predictably by slot/repeat.
3. Keep Performance Mode for interventions: fills, temporary mutes, solos, and grouped/synchronized transitions.
4. Use `Sync` for structural transitions that should land on the configured musical boundary; use Immediate for intentional cuts; use Latch when several changes should be staged and committed together.
5. If Song mutes are programmed for a track, explain that a later Song slot may reapply the arrangement's mute state. This is useful when intentional and surprising when not understood.

Do not imply that Performance Mode edits the underlying Note/Curve/LFO sequence data. It manipulates runtime performance state.

## Screenshot expectations

A full regeneration should consider distinct deterministic states for:

- Performance idle overview;
- a pending synchronized mute/solo change;
- a latched group before commit if visually distinct;
- one or more active fills with readable Fill Amount;
- a Song with multiple populated slots;
- Song playback with current slot/repeat progress;
- per-track Song pattern/mute variation if the display can teach it clearly.

Existing screenshot count is not proof that these instructional states are covered.
