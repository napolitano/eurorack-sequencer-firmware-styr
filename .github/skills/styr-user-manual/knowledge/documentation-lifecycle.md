---
knowledge-id: styr.manual.documentation-lifecycle
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Documentation lifecycle

The Styr manual is a versioned product artifact, not disposable generated prose. Agent-assisted authoring therefore has two explicit modes with different stability rules.

Both modes use the single-issue Orchestrator pipeline defined in [`orchestration.md`](orchestration.md). The maintainer assigns the authorized documentation-sync issue once to **Styr Documentation Orchestrator**; the Orchestrator delegates all specialist stages programmatically inside the same task and Draft PR.

## Mode 1: Full regeneration

`full-regeneration` reconstructs the complete source-locale manual for a selected target ref from authoritative product evidence.

Use it for:

- the initial manual bootstrap;
- validating whether the agent knowledge and evidence model are sufficient;
- periodic completeness audits;
- comparing a clean reconstruction with the maintained manual;
- recovery when the maintained documentation is known to be structurally unreliable.

A full regeneration may reorganize or rewrite content when that materially improves correctness and usability, but it must still preserve stable documentation IDs whenever an existing concept is semantically the same.

A regeneration result is a review artifact. It must **not** automatically overwrite or merge into the maintained manual. Compare it with the maintained manual and review meaningful differences before adoption.

## Mode 2: Incremental release sync

`incremental-release-sync` is the normal maintenance mode after the initial manual exists.

Inputs:

- previous release tag/baseline;
- current target ref/branch;
- existing maintained source-locale manual.

Rules:

- preserve existing information architecture unless the product change requires a structural change;
- preserve stable documentation IDs and paths for unchanged concepts;
- preserve unaffected prose instead of stylistically regenerating it;
- update all sections transitively affected by the release delta;
- add new sections only for genuinely new user-facing concepts;
- remove or deprecate text only when current behavior makes it false or obsolete;
- editorial review defaults to changed sections plus enough neighboring context to ensure consistency.

Incremental maintenance is deliberately semi-deterministic: facts, scope, IDs, screenshot identities, release interval, and validation are controlled; wording inside genuinely changed prose remains editorial.

## Stable identities

Published/manual concepts should progressively receive stable documentation identities independent of their visible title. A title may improve without creating a new concept.

Conceptual example:

```yaml
doc_id: tracks.note.microtiming
locale: en-US
```

Do not introduce IDs mechanically without a migration plan. Once assigned, treat an ID as persistent unless the underlying concept is genuinely replaced.

## What's New

`What's New` is **not** cumulative.

For an incremental release sync it describes only:

```text
previous release -> target release
```

It should include user-visible new features, meaningful improvements, implemented user-facing TODOs, relevant behavior changes, and important inherited bug fixes. It should omit refactoring, test infrastructure, provenance work, and other internal changes unless they directly affect how users operate or update Styr.

A full-regeneration run may reconstruct the current `What's New` only when an explicit previous-release baseline is also supplied. Without a baseline, it must not invent one.

## Version history

The manual should also contain a cumulative, human-readable version history. Each released version gets one concise entry explaining its character and important user-facing changes in prose. This complements `CHANGELOG.md`; it does not duplicate every engineering change.

The complete historical manual for a release remains recoverable from that Git tag/release artifact, so the current branch does not need copies of every old manual tree.

## Changelog relationship

`CHANGELOG.md` and the README change tables are impact inputs, not authoritative manual prose. The analyst and SME must verify user-visible entries against current implementation/tests and exclude internal-only work.

## Regeneration comparison

For testing, a full regeneration should be comparable to the maintained manual at the same target ref. Useful differences include:

- missing feature/workflow coverage;
- contradictory facts;
- stale screenshots or section mappings;
- unnecessary structural churn;
- terminology drift.

The comparison should not expect byte-identical prose. Structural stability, fact coverage, identifiers, and behavior claims matter more than wording identity.
