---
name: styr-user-manual
description: Repository-specific subject-matter and writing contract for authoring or reviewing Styr end-user documentation. Use for any task that changes, checks, or edits docs/manual, manual screenshots, or user-facing sequencer behavior documentation.
---

# Styr user-manual skill

Use this skill whenever working on the maintained Styr end-user manual or reviewing documentation for user-visible behavior.

Before authoring or reviewing, read `.github/skills/styr-user-manual/knowledge/README.md`. Load the knowledge modules relevant to the task before inspecting implementation detail. The knowledge base is shared across all present and future language agents; it is a curated navigation/fact layer, not a substitute for verifying current product behavior.

## Product context

Styr is a firmware modernization of the Westlicht PER|FORMER sequencer. It is a performance-oriented Eurorack sequencer with multiple track types, CV/gate and MIDI behavior, pattern/song structure, routing, clocking, generators, and a compact hardware UI.

The user manual is maintained under `docs/manual/`. It is versioned with the product in the same repository so a release tag identifies the matching firmware, documentation sources, screenshot definitions, and committed screenshot assets.

The simulator is a verification host for the same sequencer implementation, not a separate product implementation.

## Sources of truth

Use evidence in this order, selecting the source that actually governs the claim:

1. current Styr product behavior in `src/sequencer/`;
2. focused tests under `test/` that define or regress that behavior;
3. deterministic simulator/manual capture state and current UI labels;
4. persisted model/default/range definitions;
5. existing maintained Styr manual content;
6. inherited PER|FORMER documentation and legacy material for behavior that remains inherited.

A later item cannot override a current implementation fact. A source-code implementation detail is not automatically a user-facing feature; document the observable behavior and workflow.

Never invent a value, behavior, interaction, menu path, compatibility claim, or limitation. If evidence is insufficient, leave the claim unresolved and identify the missing evidence.

## Repository boundaries

User-facing manual sources:

```text
docs/manual/
├── README.md
├── assets/          generated screenshot PNGs plus manual-owned assets
├── features/        feature-level guides
└── screens/         one maintained reference page per captured UI state
```

Screenshot behavior and validation are described by:

- `README_SCREENSHOTS.md`;
- `docs/development/documentation/README.md`;
- `toolchain/check_manual_screenshots.py`;
- `src/simulator/tools/manual_screenshots.cpp`.

Do not treat `docs/development/` as end-user prose. Do not put user-manual prose into C++ API comments.

Shared documentation-agent knowledge lives under `.github/skills/styr-user-manual/knowledge/`. Keep it concise, factual, and locale-neutral in meaning. US English is the canonical source locale; future localized manuals must reuse the same technical knowledge rather than creating language-specific factual forks.

## Subject-matter model

When documenting behavior, reason in the vocabulary of a hardware sequencer rather than in C++ implementation terms.

### Core sequencing concepts

Keep these concepts distinct unless the current implementation explicitly couples them:

- project, pattern, song, track, sequence, step, layer;
- transport start/stop/continue/reset;
- internal clock versus external/slave clock;
- BPM versus pulse period versus PPQN/division;
- gate state versus gate length versus retrigger;
- note timing versus gate timing versus microtiming;
- probability versus variation versus condition;
- deterministic run modes versus random/random-walk behavior;
- current pattern versus pending/next pattern;
- Free versus synchronized track behavior;
- source routing versus track-local configuration.

### Track terminology

Use the user-facing track names consistently:

- Note Track
- Curve Track
- LFO Track
- MIDI/CV Track

Do not call an LFO a Curve Track or describe MIDI/CV behavior as if it were pitch/gate-only unless the current mode actually has that limitation.

### Eurorack and MIDI terminology

Use established technical terms accurately:

- CV, gate, trigger, V/oct, voltage, clock, reset;
- MIDI note, velocity, channel, clock, start, stop, continue where applicable;
- BPM and PPQN only for their actual quantities;
- division/divisor only according to Styr's UI/model semantics.

Always include units where a numeric value is meaningless without them. Preserve the difference between milliseconds, microseconds, volts, semitones, steps, pulses, percentages, and raw/internal units.

## Manual writing contract

Write in US English.

A screen reference should normally tell the user:

1. what state/screen they are looking at;
2. how it is reached when that is not obvious from the surrounding section;
3. what the relevant controls change;
4. what the visible values mean;
5. any boundary, interaction, or side effect that matters during use.

A feature guide should explain the feature as a workflow rather than enumerate every UI field again.

Prefer:

- "Sets the gate probability for the selected step."
- "A negative Gate Offset moves a predictable step before its nominal boundary; at a transport or reset boundary, Styr cannot schedule an event before the boundary and triggers it there instead."

Avoid:

- implementation narration such as "the engine pushes an event into a queue" unless the implementation detail itself is necessary to explain observable behavior;
- vague wording such as "improves timing" when the actual behavior can be stated;
- claims such as "perfect", "instant", "always", or "zero latency" unless explicitly established;
- marketing language.

## Consistency rules

- Use exact UI capitalization for labels and named modes.
- Keep one term for one concept throughout the manual.
- When a behavior changes, search the complete manual for all descriptions of that behavior.
- Update feature guides and screen references together when both are affected.
- Do not leave two sections describing mutually incompatible defaults or workflows.
- Do not describe a fixed upstream defect as a current limitation.
- Do not expose internal provenance notes or development-only reasoning in end-user prose.

## Screenshot contract

Manual screenshots are deterministic documentation artifacts generated by the simulator.

Rules:

- every capture has a stable semantic name;
- every capture maps one-to-one to its maintained screen Markdown and expected PNG asset;
- generated PNGs are not edited manually;
- modifier states, sublayers, Commit/Revert behavior, Quick Edit states, LFO waveforms, and generator parameter states require separate captures when they convey distinct user behavior;
- use the capture itself as evidence only after confirming the fixture represents the intended current state.

The validation command is:

```sh
python3 toolchain/check_manual_screenshots.py
```

When screenshot generation is available, use the repository's canonical `manual-screenshots` target rather than ad-hoc rendering.

## Documentation-sync workflow

For a bundled documentation sync or release candidate:

1. establish the previous release/baseline and target branch from the task;
2. inspect the whole diff from baseline to target;
3. identify all user-visible changes, including fixes, implemented TODOs, improvements, new features, and changed limitations;
4. inspect existing manual coverage before writing;
5. update all affected sections as one coherent change set;
6. reconcile screenshot requirements;
7. run deterministic documentation checks;
8. leave the branch ready for technical review, then editorial review.

The documentation agents must not merge their own changes. A release is ready only after the documentation sources are merged and the deterministic documentation build/checks succeed from the release commit.

## Reviewer separation

The three documentation roles have different authority:

- **Styr Manual SME:** establishes and writes the user-facing content.
- **Styr Manual Technical Reviewer:** verifies and corrects factual meaning against current behavior.
- **Styr Manual US English Editor:** improves language while preserving established semantics.

Do not collapse these stages by letting the editorial pass reinterpret technical behavior or the technical-review pass perform an unnecessary stylistic rewrite.
