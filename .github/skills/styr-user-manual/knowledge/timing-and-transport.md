---
knowledge-id: styr.manual.timing-and-transport
status: canonical
locale: en-US
---

# Timing and transport

## Distinct quantities

Never use these as synonyms:

- **BPM** — tempo.
- **Pulse period** — elapsed time between external clock edges.
- **PPQN** — pulses per quarter note.
- **Divisor/division** — Styr's sequence/clock division semantics as defined by the current UI/model.
- **Gate length** — duration of a gate event.
- **Gate Offset / microtiming** — displacement of a Note gate relative to its nominal step boundary.

## Internal versus external clock

User documentation must distinguish project/master tempo from detected external/slave tempo. Current Styr behavior keeps detected slave tempo independent from the project/master tempo.

External-clock handling currently includes:

- second-edge acquisition rather than waiting for a third edge;
- period-domain tempo estimation;
- separate handling of alternating swing phase;
- adaptive loss-of-clock timeout after acquisition;
- rejection/confirmation behavior that distinguishes isolated early/late edges from sustained tempo changes;
- rollover-safe time comparisons;
- clearing stale interpolation state across Stop/Continue;
- releasing a disabled active MIDI/USB clock source;
- clock-output pulse timing based on the active tempo source.

These are user-visible fixes/improvements summarized in the root README. Technical details belong in developer/test documentation unless they explain an observable workflow.

## Microtiming causality rule

A negative Note Gate Offset can only be emitted before a nominal boundary when Styr can determine the future event without guessing.

Current documented strategy:

- deterministic Forward/Backward progression, including normal loop wrap: eligible for exact pre-triggering;
- transport start: cannot emit an event before the transport exists, so the first event is bounded by the start boundary;
- Reset Measure: treated as a state boundary;
- linked tracks: conservative boundary behavior when the future source state is not safely predictable;
- pending pattern changes: conservative boundary behavior;
- dynamic Fill decisions: conservative boundary behavior;
- non-deterministic run modes: conservative boundary behavior.

Do not describe this as "perfect" or "zero-latency" microtiming. The correct user-level statement is that Styr pre-triggers predictable events and falls back safely at causal boundaries.

## Gate-duration floor

Styr fixes inherited cases where a logically present Note/Curve gate or retrigger high pulse could quantize to zero engine ticks at very fast divisors. User documentation should describe the resulting behavior, not the internal tick-floor implementation, unless a timing-resolution explanation is specifically needed.

Source summary: root `README.md`, **Fixed upstream bugs**, **Implemented upstream TODOs**, and **Improvements**.
