---
knowledge-id: styr.manual.quality
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Styr manual quality contract

The Styr manual is a maintained product document for musicians and technically capable Eurorack users. Correct facts are necessary but not sufficient. Every authoring and review stage must also protect clarity, usefulness, information architecture, and the quality of already-good maintained prose.

## Core principle

Write for the person operating Styr, not for the implementation that makes Styr work.

The manual must form a deliberate learning path. A reader should encounter device orientation and core concepts before basic workflows, basic workflows before advanced sequencing concepts, and narrative workflows before screen-by-screen reference material. A technically complete pile of screen descriptions is not a complete user manual.

A strong manual answers, in this order when applicable:

1. What is this feature or screen for?
2. Why would the user use it?
3. How is it reached and operated?
4. What are the important controls or parameters?
5. What practical boundary, limitation, or interaction matters?
6. Is a concrete musical example useful?

Do not reverse this order merely because implementation details or parameter lists are easier to extract from source code.


## Teach the whole product, not the repository tree

For full regeneration, do not mirror C++ directory structure, UI page registration order, screenshot order, or changelog order. Use `full-product-discovery.md` to determine product scope and `manual-information-architecture.md` to determine the reader journey.

The manual should explicitly connect concepts that users experience together, especially recording/input/monitoring, patterns/Song Mode, Song Mode/Performance Mode, timing/divisors/microtiming, and Fill activation/track Fill behavior.

A workflow chapter may link to multiple concise screen-reference pages. Do not force the reader to reconstruct a workflow by following several disconnected screen pages.

## Existing prose is an asset

Maintained prose that is accurate, clear, and useful is part of the product. Do not rewrite it merely because alternative wording is possible.

A rewrite needs a concrete reason:

- the existing text is factually wrong;
- the product behavior changed;
- the text is incomplete for the documented workflow;
- the text is ambiguous or materially harder to understand than the replacement;
- the information architecture changed for a real user-facing reason;
- terminology must be normalized across affected sections.

In `incremental-release-sync`, unnecessary churn in unaffected prose is a correctness defect.

In `full-regeneration`, complete reconstruction is permitted for audit and bootstrap purposes, but the maintained manual remains a quality regression oracle. If an existing passage is still correct and is clearer, more concise, or more useful than the regenerated passage, preserve or restore the better passage. Regeneration is not permission to replace good writing with merely different writing.

## User-facing language

Prefer language that describes what the user does, sees, hears, configures, or can rely on.

Avoid publishing internal validation language such as:

- `evidence-backed`;
- `current implementation` when the distinction is irrelevant to the user;
- `the code does`;
- `scheduler`, `internal state`, `causal boundary`, or similar implementation concepts unless understanding them is genuinely necessary to operate the feature correctly.

State limitations in user terms. Explain the observable consequence first; explain internal rationale only when it directly helps operation or troubleshooting.

## Progressive disclosure

Present the normal workflow before exceptional cases.

Use the following order where possible:

1. normal operation;
2. common variations;
3. limitations and edge cases;
4. deep reference detail.

Do not force a first-time reader through rare boundary conditions before explaining the normal feature.

## Do not turn the manual into a UI inventory

A complete list of rows, enum values, or menu entries may belong in reference material, but a useful manual must also explain the workflow and relationships between controls.

Parameter lists are appropriate when they help scanning, but they must not replace explanation of the feature's purpose and use.

## Screen pages versus feature guides

Screen reference pages under `docs/manual/screens/` should stay concise. They normally explain:

- what the shown state represents;
- how to reach it;
- how the relevant controls work;
- important screen-specific notes.

Broader concepts, multi-screen workflows, musical use, interactions between settings, and longer examples belong in feature/category guides rather than being duplicated on every screen page.

## Examples

Use concrete examples when they materially reduce cognitive load, especially for:

- clocking and divisors;
- microtiming;
- probability, conditions, and variation;
- LFO timing/ranges;
- generators;
- Curve behavior;
- MIDI/CV voice allocation;
- routing and performance workflows.

Examples must use values and behavior supported by current Styr evidence. Avoid filler examples that merely restate the preceding sentence.

## Screenshot quality

Screenshots are instructional content, not decoration.

For every screenshot used or regenerated, ask:

- Does it show the exact state the accompanying text discusses?
- Is the chosen state readable and representative?
- Does a waveform, pattern, value, or selected row make the feature visually obvious?
- Is a different deterministic state more useful to the reader?
- Are multiple screenshots unnecessarily similar?
- Is a missing state forcing prose to compensate for absent visual evidence?

For LFO captures, use a waveform/range that makes the shape clearly visible. For generators and Note/Curve submodes, capture the actual parameter or Commit/Shift state needed to understand the workflow.

## Quality questions before completion

For every materially changed page, the author/reviewer should be able to answer yes to these questions:

- Does this page help a user accomplish or understand something concrete?
- Is the feature's purpose clear before implementation detail appears?
- Is the normal workflow obvious before edge cases?
- Has useful existing information been preserved?
- Is any sentence present only because the agent discovered an implementation detail?
- Are exact UI labels, values, ranges, and semantics preserved?
- Does the screenshot support the explanation rather than merely exist beside it?
- Is the page in the right part of the manual, with broader concepts kept out of narrow screen-reference pages?

## Good and bad transformations

### Internal validation language

Bad:

> The current evidence-backed build exposes an Advanced Settings page containing Enhanced UI and Language rows.

Better:

> Advanced Settings contains options that affect Styr's general user interface. Select a setting with the encoder and press it to edit when the option is available.

The better version describes the user-facing purpose and operation. If current evidence cannot establish whether a setting is editable, do not invent the edit instruction; document only what is established.

### Microtiming boundary

Bad:

> Negative Gate Offset fires early only when Styr can determine the upcoming step without guessing. Deterministic forward/backward playback can pre-trigger exactly, while causal boundaries force fallback behavior.

Better:

> Negative offsets move a note ahead of its normal step boundary. At the start of playback, after a reset, or where the next step cannot be known in advance, Styr plays the note on the boundary instead.

The technical behavior is preserved, but the user does not need internal scheduling terminology.

### Parameter inventory

Weak:

> The page contains Source, Voices, Voice Config, Note Priority, Low Note, High Note, Pitch Bend, Mod Range, Retrigger, Slide Time, Transpose, and Arpeggiator.

Better:

> Use this page to turn incoming MIDI into one or more CV/gate voices. First choose the MIDI source and channel, then set how many voices Styr should allocate and which CVs each voice produces. Note priority and note-range settings decide which incoming notes receive those voices when more notes are held than are available.

A compact parameter reference can follow after the workflow explanation.

## Quality is not marketing

High-quality documentation is precise, calm, and useful. Do not add hype, product praise, vague claims, or sales language to make prose feel polished.
