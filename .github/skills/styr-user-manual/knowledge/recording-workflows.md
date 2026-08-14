---
knowledge-id: styr.manual.recording-workflows
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Recording workflows

This module records verified recording semantics and an editorially recommended learning workflow for the user manual. Keep the distinction between **product fact** and **recommended workflow** explicit.

## Verified product model

### Recording activation

The top-level UI toggles recording with `PAGE` + `PLAY` (`src/sequencer/ui/pages/TopPage.cpp`). While recording is active, the header uses the recording indicator in place of the normal clock-mode indicator (`src/sequencer/ui/painters/WindowPainter.cpp`).

### Project recording controls

Project-wide recording/monitoring configuration includes:

- Monitor Mode: `Always`, `Stopped`, `Off`;
- Record Mode: `Overdub`, `Overwrite`, `Step Record`;
- MIDI Input: `Off`, `All`, or a selected MIDI source/channel configuration;
- CV/Gate Input: `Off`, `CV1/CV2`, or `CV3/CV4`;
- Curve CV Input: `Off`, `CV1`, `CV2`, `CV3`, or `CV4`.

Sources: `src/sequencer/model/Types.h`, `src/sequencer/model/Project.h`, `src/sequencer/ui/model/ProjectListModel.h`.

Project defaults currently initialize Monitor Mode to `Always` and Record Mode to `Overdub` (`src/sequencer/model/Project.cpp`). Verify these defaults again before publishing them as release facts.

### Selected-track input behavior

General MIDI/CV-gate monitoring routes incoming note performance data to the currently selected track after project input filtering (`src/sequencer/engine/Engine.cpp`). Curve recording is active only for the selected Curve Track and only when a Curve CV Input is configured (`src/sequencer/engine/CurveTrackEngine.cpp`).

### Step Record

For a Note Track, Step Record:

- starts at the sequence First Step unless a valid current monitored/selected step changes the recorder position;
- writes a note and enables its gate when a note-on arrives;
- uses a fixed initial recorded gate-length value rather than deriving expressive duration from transport timing;
- advances to the next sequence step when the matching note is released;
- wraps from Last Step back to First Step;
- can mark Slide while a note is held when pitch bend is received;
- can set the recorded step to maximum Length when MIDI CC1 is received while the note is held.

Source: `src/sequencer/engine/StepRecorder.h` and Note Track monitor-step handling.

Do not describe Step Record as realtime timing capture. Its strength is deterministic sequential note entry.

### Realtime Overdub and Overwrite

Realtime Note recording evaluates incoming note history against the sequence step grid (`src/sequencer/engine/NoteTrackEngine.cpp`). It writes gate/note and recorded length information to the targeted step.

Important user-facing consequences:

- realtime note onset is assigned to the sequence step grid rather than preserving arbitrary sub-step onset timing as Gate Offset;
- Overdub writes recorded notes without clearing an otherwise untouched selected step;
- Overwrite clears an unwritten step on the selected track as playback passes it, making Overwrite deliberately destructive to the existing loop;
- current realtime recording normalizes several step attributes of a written event, including gate/retrigger probabilities and condition, rather than preserving every prior per-step performance modifier.

The manual should explain the practical consequence rather than expose the internal record-history algorithm.

### Curve recording

With recording active, a selected Curve Track and a configured Curve CV Input, Styr samples the incoming CV across each completed step. It derives Min/Max and selects the closest supported Curve shape for that step (`src/sequencer/engine/CurveTrackEngine.cpp`, `src/sequencer/engine/CurveRecorder.h`).

A step is committed only after recording has covered the complete step duration. Describe this as step-based curve capture rather than continuous automation storage.

## Recommended Note-recording learning workflow

This is an editorial recommendation, not a hidden firmware mode.

### Recommended first approach: build a clean skeleton with Step Record

For a user learning Styr or deliberately composing a monophonic sequence, recommend this order:

1. Start with a clean/initialized pattern or consciously clear the target material when predictable results matter. Step Record changes key note/gate fields but is not intended to sanitize every unrelated pre-existing step modifier.
2. Select the target Note Track and establish the useful sequence range (First Step/Last Step), musical scale/root when needed, and an appropriate Divisor.
3. Configure the intended MIDI or CV/gate input and choose a Monitor Mode that makes auditioning comfortable.
4. Set Record Mode to `Step Record` and enable recording with `PAGE` + `PLAY`.
5. Enter notes in sequence. Releasing the current note advances the record position. Select a valid Note step first when a specific starting step is desired and current UI behavior supports that selection state.
6. Stop recording and refine Length, Slide, Retrigger, Gate Offset, probability/variation, and conditions deliberately in the normal step editor.

Why recommend this first: it separates **what notes belong on which steps** from expressive timing and probability. That produces a predictable sequence skeleton before advanced modifiers are introduced.

### Recommended second approach: capture a played phrase with Overdub

Once the user understands the grid, recommend `Overdub` for realtime capture:

1. Choose a short, clearly understood loop range first (for example a single musical phrase rather than an unexplored 64-step sequence).
2. Confirm clock/tempo and input monitoring before enabling record.
3. Use `Overdub` when existing steps should survive unless a new note is actually recorded there.
4. Play the phrase against the running sequence.
5. Disable recording, inspect the resulting note/length data, then use Gate Offset for deliberate ahead/behind-the-grid microtiming and other step modifiers.

The manual should state clearly that realtime capture is grid-oriented. Do not promise preservation of arbitrary played microtiming when current recording writes note events onto the step grid.

### Use Overwrite only deliberately

Recommend `Overwrite` when the intention is to replace the selected loop, not merely add notes.

Because an unwritten step is cleared as the selected track passes it, Overwrite is useful for "play a replacement pass" workflows but dangerous as a casual default. This deserves a concise warning/callout in the manual.

## Recommended Curve-recording workflow

For Curve Track recording:

1. Select the target Curve Track.
2. Set its voltage range appropriately for the intended modulation destination.
3. Configure the required Curve CV Input.
4. Start transport so Styr has a step timeline to capture against.
5. Enable recording and perform the incoming CV over complete steps.
6. Stop recording and review the inferred Shape/Min/Max per step; refine manually where the closest supported curve does not match the desired musical result.

Do not describe Curve recording as retaining the original high-resolution CV waveform. The product reduces each completed step to Styr's supported curve representation.

## Manual presentation rules

The recording chapter must explain:

- how to choose between Step Record, Overdub, and Overwrite;
- what input/monitor configuration is required;
- which track is receiving the data;
- what the recording modes preserve or replace;
- how recording relates to later step editing;
- Curve recording separately from Note recording.

Prefer a practical comparison table plus one complete worked workflow over three disconnected parameter definitions.

The main recording chapter should not reproduce every Project-page label. Link to the Project screen reference for exact UI location/details.
