---
knowledge-id: styr.manual.terminology
status: canonical
locale: en-US
---

# Canonical terminology

Stable term IDs allow future localization agents to share one technical vocabulary even when the rendered words differ by language.

| Term ID | Canonical en-US term | Meaning / usage rule |
|---|---|---|
| `product.styr` | Styr | Current project/product name. |
| `upstream.performer` | PER\|FORMER | Upstream Westlicht project; preserve its spelling when referring to upstream. |
| `track.note` | Note Track | Track for gates and pitch-oriented note data. |
| `track.curve` | Curve Track | Step-based CV-curve track with optional gates. |
| `track.lfo` | LFO Track | Dedicated continuous modulation track. |
| `track.midi_cv` | MIDI/CV Track | Distinct MIDI/CV track type. |
| `sequence.step` | step | One addressable sequence position. |
| `sequence.layer` | layer | One editable property view across sequence steps. |
| `sequence.run_mode` | Run Mode | User-facing sequence traversal mode. |
| `sequence.divisor` | Divisor | Use Styr's UI/model meaning; do not substitute PPQN. |
| `transport.start` | Start | Transport start operation/message where applicable. |
| `transport.stop` | Stop | Transport stop operation/message where applicable. |
| `transport.continue` | Continue | Resume/continue operation where applicable. |
| `clock.external` | external clock | Clock supplied by an external source. |
| `clock.internal` | internal clock | Styr's own master clock. |
| `clock.bpm` | BPM | Beats per minute; do not use for pulse frequency. |
| `clock.ppqn` | PPQN | Pulses per quarter note. |
| `note.gate` | Gate | Gate state/event for a Note step. |
| `note.gate_offset` | Gate Offset | Note gate microtiming relative to nominal step boundary. |
| `note.retrigger` | Retrigger | Repeated gate pulses within a step, subject to representable timing. |
| `note.slide` | Slide | Note-track slide behavior; preserve the UI term. |
| `probability.gate` | Gate Probability | Probability controlling gate realization. |
| `variation` | variation | Alternate/randomized parameter behavior as defined by the specific layer. |
| `condition` | Condition | Step condition logic; do not replace with probability. |
| `generator.commit` | Commit | Accept current generated candidate into editable sequence state. |
| `generator.revert` | Revert | Restore pre-generator state. |
| `generator.init` | Init | Initialize according to the active generator/context. |
| `lfo.sync` | Sync | LFO mode synchronized to sequencer progression. |
| `lfo.free` | Free | LFO mode using free-running frequency. |

## Style rules

- Use exact UI capitalization when referring to a visible label or named mode.
- Use sentence-case common nouns when discussing the concept generically.
- Preserve units: V, Hz, ms, µs, %, steps, pulses, semitones, BPM, PPQN.
- Do not translate MIDI, CV, LFO, BPM, PPQN, V/oct, or other established technical abbreviations merely to make prose look localized.
