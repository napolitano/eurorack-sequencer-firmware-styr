---
knowledge-id: styr.manual.knowledge
status: canonical
locale: en-US
purpose: shared-subject-matter-brain
---

# Styr manual knowledge base

This directory is the shared subject-matter knowledge base for Styr user-documentation agents. It is deliberately Markdown: humans can review and edit it directly, Git can diff it cleanly, and agents can consume it without a generated database or proprietary format.

It is **not** the published user manual. The maintained manual remains under [`docs/manual/`](../../../../docs/manual/). This knowledge base exists to keep authoring and review agents aligned on the same product model, terminology, evidence rules, and documentation inventory.

## Authority and scope

The knowledge base is a curated index of facts already established by current Styr behavior and maintained documentation. It must not become a competing implementation specification.

When a fact in this directory conflicts with current product behavior, the current implementation and focused regression tests win. The knowledge entry must then be corrected in the same documentation bundle.

Evidence priority remains:

1. current product behavior under `src/sequencer/`;
2. focused tests under `test/`;
3. deterministic simulator/manual capture definitions and current UI labels;
4. persisted model/default/range definitions;
5. maintained Styr user manual;
6. inherited PER|FORMER material only where behavior remains inherited.

## Knowledge modules

| Module | Purpose |
|---|---|
| [`product-model.md`](product-model.md) | Product identity, documentation boundaries, display model, and high-level sequencing concepts. |
| [`interaction-model.md`](interaction-model.md) | User-facing navigation, function-key layers, Quick Edit, context menus, and generator workflow. |
| [`tracks.md`](tracks.md) | Canonical user-facing model for Note, Curve, LFO, and MIDI/CV tracks. |
| [`timing-and-transport.md`](timing-and-transport.md) | Clock, transport, external synchronization, microtiming, and boundary behavior relevant to users. |
| [`change-catalog.md`](change-catalog.md) | User-visible Styr changes that documentation agents must consider during release synchronization. |
| [`documentation-lifecycle.md`](documentation-lifecycle.md) | Full-regeneration versus incremental-release modes, stable identities, What's New, version history, screenshot regeneration, and regeneration testing. |
| [`manual-quality.md`](manual-quality.md) | Mandatory editorial quality contract, preservation rules, user-facing writing principles, screenshot quality, and good/bad examples. |
| [`orchestration.md`](orchestration.md) | Single-issue Orchestrator delegation, structured in-context handoffs, stage gates, and the single Draft-PR contract. |
| [`documentation-map.md`](documentation-map.md) | Mapping from concepts to maintained manual sections and deterministic screenshot families. |
| [`terminology.md`](terminology.md) | Canonical English terms and stable term IDs for authoring and future localization. |
| [`localization.md`](localization.md) | Rules for future localized manuals without creating parallel technical truths. |

## Maintenance rules

- Keep entries concise and factual. Detailed explanations belong in the manual.
- Prefer links to authoritative repository sources over copying large tables of implementation data.
- When copying a value or range into this knowledge base, include the unit and the source location that establishes it.
- Do not add inferred behavior merely because it seems musically sensible.
- Mark unresolved questions as unresolved instead of choosing a likely answer.
- Update all affected modules when a product change alters more than one concept.
- The authoring agent may update this knowledge base only when the product evidence is clear and the change is part of the same documentation bundle.
- Technical review must verify knowledge-base changes as strictly as manual prose.

## Relationship to releases

The knowledge base is versioned with the source repository. A product tag therefore identifies the matching code, tests, manual sources, screenshot definitions, and documentation-agent knowledge state.

The release build must never require an agent to reconstruct missing knowledge. Agent work happens before the release commit; deterministic documentation checks and publication happen afterward.
