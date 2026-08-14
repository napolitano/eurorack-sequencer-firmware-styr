---
knowledge-id: styr.manual.information-architecture
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Manual information architecture

The Styr user manual is a guided learning document first and a screen reference second. A full regeneration must not produce a flat collection of UI pages and parameter lists.

The reader journey is intentionally stable. Individual chapter titles and file boundaries may evolve when the product requires it, but the conceptual order below is mandatory unless a documented human editorial decision changes it.

## Reader journey

### Part I — Meet Styr

Start with orientation, not details.

The opening material must establish:

- what Styr is and the role it serves in a Eurorack/MIDI setup;
- the high-level signal/control flow;
- the main physical controls and I/O relevant to operation;
- the display/header/footer model;
- how page navigation, track selection, step keys, function keys, modifiers, and the encoder are generally used.

A first-time reader should be able to look at the device and understand what the major regions are before learning individual features.

Suggested narrative source family:

```text
docs/manual/guide/device-overview.md
```

### Part II — How Styr thinks

Explain the product's core concepts before asking the reader to perform complex workflows.

At minimum cover the relationships among:

- Project;
- eight tracks and their track types;
- patterns;
- sequences and steps;
- play pattern versus edit pattern where relevant;
- project-wide, track-local, sequence-local, and step-local settings;
- timing, transport, clock, divisors, and synchronization at a conceptual level;
- routing/output assignment;
- Song Mode and Performance Mode as two different layers of working with prepared material.

This section should give the reader a mental model, not exhaustively enumerate settings.

Suggested narrative source family:

```text
docs/manual/guide/core-concepts.md
```

### Part III — Basic use

Provide a concrete path from a newly loaded/initialized project to useful output.

The basic path should normally teach:

1. selecting/configuring a track;
2. setting a simple musical context such as tempo/clock and scale/root where applicable;
3. creating a small Note sequence manually;
4. starting/stopping transport;
5. editing notes/gates/length at a basic level;
6. selecting or changing patterns;
7. saving the project.

Do not introduce advanced probability, complex routing, Fill logic, Song arrangement, or obscure edge cases before the reader has produced a basic sequence.

Suggested narrative source family:

```text
docs/manual/guide/getting-started.md
docs/manual/guide/editing-sequences.md
docs/manual/guide/patterns.md
```

### Part IV — Creating and recording material

Teach how material gets into Styr beyond manual step editing.

This part must include:

- Note recording from supported MIDI and CV/gate inputs;
- Monitor Mode and Record Mode in user terms;
- Step Record;
- realtime Overdub;
- realtime Overwrite, including its destructive consequence;
- a recommended recording workflow from `recording-workflows.md`;
- Curve recording from CV when supported;
- generators as a complementary creation workflow;
- what recording does and does not preserve, for example grid quantization versus later Gate Offset editing when established by current behavior.

Suggested narrative source family:

```text
docs/manual/guide/recording-sequences.md
```

Track-specific creation guides may remain under `docs/manual/features/`.

### Part V — Patterns and arrangement

Explain how prepared material becomes a longer structure.

The reader should understand:

- the 16-pattern model;
- per-track pattern selection where relevant;
- immediate versus synchronized pattern-related behavior where current UI exposes it;
- how Song Mode uses slots to select per-track patterns, mutes, and repeat counts;
- how to build, edit, start, stop, and navigate an arrangement;
- how a song advances and loops;
- what actions terminate Song playback;
- how Song Mode interacts with live mute/fill behavior.

Use `song-and-performance.md` as the conceptual baseline.

Suggested narrative source family:

```text
docs/manual/guide/song-mode.md
```

### Part VI — Performance Mode

Performance Mode deserves a workflow chapter, not a one-screen description.

Explain:

- immediate mute/solo changes;
- latched groups of changes;
- synchronized changes at the configured synchronization boundary;
- unmute/cancel behavior;
- per-track and global Fill triggering;
- Fill Amount editing;
- held Fill behavior where current evidence establishes it;
- using Performance Mode while a Song arrangement is running, including any song-slot behavior that can later override live mutes.

Use a musical scenario to demonstrate the difference between Immediate, Latch, and Sync instead of only defining the three terms.

Suggested narrative source family:

```text
docs/manual/guide/performance-mode.md
```

### Part VII — Advanced sequencing and modulation

Only after the basic workflows are clear should the manual go deep on topics such as:

- external clock and detailed synchronization;
- swing and timing relationships;
- Gate Offset/microtiming;
- retriggers;
- probability, variation, and conditions;
- run modes and sequence boundaries;
- Fill modes;
- Curve Track details;
- LFO Track details;
- MIDI/CV voice allocation;
- routing/MIDI output;
- generators and advanced generator parameters;
- track linking and other cross-track behavior supported by the product.

The exact chapter split should follow product discovery rather than this list mechanically.

Suggested source families:

```text
docs/manual/guide/advanced-*.md
docs/manual/features/**
```

### Part VIII — Projects, maintenance, and system tasks

Collect infrequent but important whole-device workflows:

- project initialization/save/load/save-as;
- SD-card operations;
- calibration;
- monitoring/diagnostics where user-relevant;
- firmware update;
- advanced/system settings.

Destructive operations require clear warnings and prerequisites.

### Part IX — Reference

Screen-by-screen material belongs late in the manual as reference support.

`docs/manual/screens/**` should remain concise and searchable. It must not carry the burden of teaching the product's overall mental model.

The narrative guide should link to screen references where useful. The publication manifest (`docs/manual/manual.toml`) should place narrative chapters before screen-reference material.

### Part X — Release information

End with product-version material:

- `What's New` for exactly the current release interval when a baseline exists;
- cumulative Version History;
- colophon/licensing/credits where appropriate.

`What's New` is not a substitute for any chapter above.

## Full-regeneration file policy

A full regeneration may create the narrative `docs/manual/guide/` layer when it does not yet exist. Use semantic, stable filenames rather than version-specific names.

Recommended initial guide identities:

```text
docs/manual/guide/device-overview.md
docs/manual/guide/core-concepts.md
docs/manual/guide/getting-started.md
docs/manual/guide/editing-sequences.md
docs/manual/guide/recording-sequences.md
docs/manual/guide/patterns.md
docs/manual/guide/song-mode.md
docs/manual/guide/performance-mode.md
```

Create additional advanced guides only when product discovery demonstrates a real conceptual need.

Do not manufacture empty chapters merely to satisfy filenames. If two adjacent subjects are genuinely clearer as one chapter, the SME may combine them while preserving the reader journey and stable concept IDs.

## Publication order

`docs/manual/manual.toml` is the deterministic publication order for the current locale. During full regeneration, the SME must update its chapter list so the generated ODT/PDF follows this reader journey rather than repository-directory order.

Do not use alphabetical file order as manual structure.

## Screen references are not chapters by default

A screenshot page explains a state. A narrative chapter explains a concept or workflow.

Do not build the main learning path by concatenating 100+ screenshot descriptions. Reference pages may be included in a dedicated reference part or selected where they genuinely support a chapter.

## Completion test

Before UX/IA review, a full-regeneration manual should let a new user answer, in order:

1. What is this device and where do signals go?
2. How do I navigate it?
3. What are Project, Track, Pattern, Sequence, and Step?
4. How do I make my first useful sequence?
5. How do I record rather than enter every step manually?
6. How do I build patterns into a Song?
7. How do I interact with the running material in Performance Mode?
8. Where do I learn the deeper timing, probability, modulation, routing, and generator functions?
9. Where do I find exact screen/reference information and system procedures?
