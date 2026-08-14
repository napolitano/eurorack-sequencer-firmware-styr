---
knowledge-id: styr.manual.full-product-discovery
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Full product discovery contract

`full-regeneration` is not a large incremental documentation update. It is a ground-up reconstruction of the current Styr product from authoritative repository evidence.

The analyst must answer this question independently of the existing manual:

> What is Styr as a product, what can a user do with it, what must a user understand to operate it confidently, and what documentation structure follows from that product model?

The existing manual and `CHANGELOG.md` are useful evidence, but neither defines the scope of a full regeneration.

## Scope authority

For full regeneration, determine product scope from the current product surface, not from prior prose.

Use, as applicable:

1. `src/sequencer/Config.h` for product-scale constants that are genuinely user-visible;
2. the current model under `src/sequencer/model/`;
3. runtime behavior under `src/sequencer/engine/`;
4. pages, painters, UI models, labels, context menus, modifiers, and interaction state under `src/sequencer/ui/`;
5. current routing, MIDI, CV/gate, clock, persistence, project, system, calibration, and update behavior;
6. focused regression/product tests under `test/`;
7. fresh deterministic simulator screenshots and their capture definitions;
8. current README feature/fix tables where they identify Styr-specific behavior;
9. maintained manual prose as a comparison and quality-regression oracle;
10. inherited PER|FORMER material only where current Styr behavior remains inherited.

Do not use the changelog as a feature inventory. Do not use the existing manual tree as the boundary of the product.

## Mandatory discovery passes

A full-regeneration analysis must perform all of these passes before authoring begins.

### Pass 1 — Product surface inventory

Inventory every user-visible product surface, including at minimum:

- physical controls and connectors that the software/documentation can establish;
- top-level pages and modal pages;
- project-wide settings;
- track types;
- track-level settings;
- sequence-level settings;
- step layers and their grouped/modifier states;
- pattern selection and pattern state;
- Song Mode;
- Performance Mode;
- transport and clocking;
- recording and monitoring;
- routing and output layout;
- MIDI/CV behavior;
- generators;
- system, calibration, maintenance, and firmware-update functions;
- save/load/persistence workflows;
- status/header/footer information that changes how the user operates the device.

Do not count a source file as a product surface merely because it exists. The inventory must remain user-facing.

### Pass 2 — Interaction inventory

Map how the user reaches and manipulates each surface:

- normal key actions;
- `PAGE`, `SHIFT`, `TEMP`, `PATT`, `PERF`, track, step, and function-key modifiers where applicable;
- encoder press/turn behavior;
- transient versus persistent selection;
- context menus;
- Quick Edit/layer cycling;
- Commit/Revert or confirmation flows;
- immediate, synced, or latched execution where exposed to the user.

### Pass 3 — Product/data mental model

Establish the concepts a user must understand, without leaking implementation internals. At minimum determine the relationships among:

- Project;
- Track;
- track type;
- Pattern;
- Sequence;
- Step;
- play pattern versus edit pattern where applicable;
- Song slot;
- routing/output assignment;
- transport/clock/time base;
- project-wide versus track-local versus sequence-local versus step-local state.

### Pass 4 — Workflow discovery

Identify complete user tasks rather than isolated screens. Examples include:

- creating a first playable sequence;
- editing notes and gates;
- recording a Note sequence;
- recording Curve data;
- selecting/changing patterns;
- arranging a song;
- performing with mute/solo/fill actions;
- synchronizing to external clock;
- routing tracks to outputs;
- saving/loading a project;
- updating firmware;
- calibrating outputs.

The list must come from current evidence and may be larger than these examples.

### Pass 5 — Feature interaction discovery

Identify behavior that crosses feature boundaries. A good manual must explain these relationships where they matter to users.

Examples of interaction classes to inspect:

- clock -> divisor -> sequence timing -> gate length -> swing -> microtiming;
- scale/root -> Note Track -> generators -> transpose -> MIDI/CV behavior;
- patterns -> per-track pattern state -> Song Mode -> Performance Mode;
- recording -> input selection -> monitor mode -> selected track -> sequence state;
- song-slot mutes -> live performance mutes;
- Fill -> track fill mode -> next-pattern/variation behavior;
- project save/load -> persisted song/play/routing state.

Do not invent an interaction because it seems musically plausible. Verify it.

### Pass 6 — Boundaries and failure modes

Find user-relevant limits and special behavior, such as:

- finite pattern/step/song capacities;
- behavior at transport/reset boundaries;
- destructive operations;
- modes that overwrite rather than add data;
- actions that stop another mode;
- input/output or routing prerequisites;
- cases where a user-visible option is intentionally unavailable.

Describe observable consequences, not internal implementation mechanics.

### Pass 7 — Screenshot/state coverage

Compare the discovered UI surface with `src/simulator/tools/manual_screenshots.cpp` and current generated assets.

The question is not merely whether every declared screenshot was generated. Determine whether the declared screenshot set is itself complete enough to teach the product.

Identify:

- missing screens;
- missing modifier states;
- missing sublayers;
- missing recording states;
- missing Song/Performance states;
- poor fixture choices;
- screenshots that are technically valid but instructionally weak.

### Pass 8 — User mental model

Before proposing chapters, summarize the smallest coherent mental model a new Styr user needs. This must explain the product in user terms, not code terms.

A reader should understand how sound/control data moves from a project and track configuration through patterns/sequences/steps to CV/gate/MIDI outputs and how transport, Song Mode, and Performance Mode affect playback.

### Pass 9 — Manual architecture plan

Apply `manual-information-architecture.md`. Map the discovered product to the required reader journey. Do not simply mirror source-code directories or UI page order.

### Pass 10 — Coverage audit

Before returning `analyst-complete`, cross-check the proposed manual against the independently discovered product surface.

The structured handoff must explicitly report:

- discovered product surfaces;
- discovered workflows;
- feature interactions that need explanation;
- required narrative-guide sections;
- required feature/reference sections;
- required screenshot states;
- unresolved product questions;
- known existing-manual gaps;
- useful existing material worth preserving.

## Changelog relationship

For `full-regeneration`, `CHANGELOG.md` is supplemental history only. It can help identify:

- Styr-specific additions;
- changed behavior;
- implemented upstream TODOs;
- inherited bug fixes with user-visible consequences;
- release-history prose.

It must never answer the question "what features does Styr have?" by itself.

## Existing manual relationship

The existing manual must be inspected after independent discovery for three reasons:

1. detect functionality the fresh discovery accidentally missed;
2. preserve stronger existing prose and terminology;
3. identify stale, contradictory, or structurally weak material.

Do not derive the product inventory from the existing manual and then call the result a full regeneration.

## Completion standard

`analyst-complete` in full-regeneration mode means the Analyst has bounded the **current product**, not merely the change since a prior release.

A useful handoff should make it possible to answer:

- What does Styr do?
- How does a new user learn it?
- How does a user create and record material?
- How does a user arrange material into a song?
- How does a user perform with it live?
- Which advanced concepts deserve deeper treatment?
- Which screens are reference material rather than the main teaching path?
- What is still unknown?
