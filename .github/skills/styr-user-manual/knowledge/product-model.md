---
knowledge-id: styr.manual.product-model
status: canonical
locale: en-US
---

# Product model

## Product identity

Styr is an independent continuation and modernization of the Westlicht PER|FORMER sequencer firmware. The inherited hardware concept, original firmware, and technical foundation remain attributed to Simon Kallweit and PER|FORMER. Styr documentation describes the current Styr product rather than rebranding inherited authorship.

The simulator hosts the same sequencer implementation used by the embedded application. It is a verification/documentation host, not a separate product implementation.

## User-facing sequencing hierarchy

Keep these concepts distinct in user documentation:

- **Project** — project-wide persisted musical/configuration state.
- **Pattern** — a playable arrangement of track sequence state at a pattern position.
- **Song** — an arrangement/playback structure built from patterns.
- **Track** — one logical sequencing/modulation lane.
- **Sequence** — the step/playback data used by a track within the current pattern context.
- **Step** — one addressable event/data position in a Note or Curve sequence.
- **Layer** — one editable property view across steps, such as Gate, Note, Length, Shape, or Gate Probability.

Do not collapse project, pattern, sequence, and track into interchangeable terms.

## Display model

The hardware display is 256×64 pixels and is described to users as three regions:

1. **Header** — global/editing context such as clock/recording state, tempo, selected track, pattern context, page, and optional sub-mode.
2. **Body** — page-specific working area.
3. **Footer** — contextual labels for the five physical function keys.

Source: `docs/manual/screens/interface/README.md`.

## Canonical user-facing track names

- Note Track
- Curve Track
- LFO Track
- MIDI/CV Track

The maintained manual currently provides dedicated sections for Note, Curve, and LFO, with MIDI/CV under other track modes.

## Documentation boundaries

- User workflows and explanations: `docs/manual/`.
- Developer/build/architecture material: `docs/development/`.
- Verification policy: `docs/testing/`.
- Historical inherited documentation: `docs/legacy/performer/`.
- Technical facts in C++ Doxygen remain source-level reference data, not user-manual prose.

The public manual should describe observable behavior, not internal queue structures, class names, or implementation strategies unless the implementation detail is necessary to explain a user-visible boundary.
