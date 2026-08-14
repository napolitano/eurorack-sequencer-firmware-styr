---
knowledge-id: styr.manual.orchestration
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Documentation-agent orchestration

Styr documentation synchronization uses **one authorized issue assignment, one maintainer-facing Orchestrator, internal specialist sub-agents, and one Draft PR**.

The maintainer does not manually chain branches or specialist sessions. GitHub issue assignment is used intentionally for the first and only visible agent: assigning the authorized issue to **Styr Documentation Orchestrator** creates the single Draft PR that carries the complete documentation bundle.

## Official entry point

Run `.github/workflows/create-documentation-sync.yml` manually with `workflow_dispatch`. The workflow creates an issue carrying `internal:documentation-sync` and records:

- sync issue number;
- authoring mode;
- target ref;
- comparison baseline when required;
- final PR base branch;
- optional documentation emphasis.

The workflow itself does not assign an agent. The maintainer opens the generated issue and assigns it **once** to **Styr Documentation Orchestrator**.

Do not assign the issue directly to a specialist agent. The specialist profiles are programmatic-only (`user-invocable: false`).

## Single-task specialist chain

The Orchestrator uses the custom-agent `agent` tool to delegate stages in this exact order inside the current Copilot task/worktree:

```text
authorized documentation-sync issue
        |
        | maintainer assigns once
        v
Styr Documentation Orchestrator
        |
        | agent tool
        v
Styr Release Documentation Analyst
        |
        | structured result
        v
Styr Manual SME
        |
        | structured result
        v
Styr Manual UX and Information Architecture Reviewer
        |
        | structured result
        v
complete screenshot regeneration + asset validation
        |
        v
Styr Manual Technical Reviewer
        |
        | structured result
        v
Styr Manual US English Editor
        |
        v
same Draft PR -> deterministic CI -> human review
```

There are no manual branch handoffs between specialists and no specialist-created pull requests. The Draft PR created by the initial issue assignment is the only PR for the documentation sync.

For `full-regeneration`, the Orchestrator must regenerate the complete screenshot corpus **before invoking the Analyst** so visible UI evidence comes from the selected target state. It must regenerate the complete corpus again after the UX/IA stage and before Technical Review. For incremental mode, the final pre-Technical-Review regeneration is mandatory as well. Screenshot generation is atomic: a failed generation must not be treated as partial success. The canonical orchestration command is `python3 toolchain/regenerate_manual_screenshots.py`.

## Handoff model

Specialist-to-specialist state is passed through structured sub-agent results in the Orchestrator context. Internal analysis handoffs are **not committed as repository files**.

### Analyst result

The Analyst returns a Markdown block headed:

```text
STYR_DOCUMENTATION_IMPACT_SET
```

It must include:

- `status: analyst-complete` or `analyst-incomplete`;
- sync issue number;
- mode;
- target;
- baseline when applicable;
- final PR base branch;
- complete user-visible coverage/impact inventory;
- in full regeneration: product-surface map, user-workflow map, feature-interaction map, user mental model, manual-architecture plan, and product-based coverage audit;
- affected existing documentation IDs/paths;
- required new documentation IDs/paths;
- screenshot additions/changes, including missing instructional states discovered beyond the existing capture list;
- What's New candidates when applicable;
- release-history candidates;
- evidence/source locations;
- explicitly excluded internal changes;
- unresolved evidence questions.

The Analyst does not edit the repository.

### Manual SME result

The Manual SME receives the complete Analyst result, edits the manual and permitted supporting documentation sources in the shared worktree, and returns:

```text
STYR_MANUAL_AUTHORING_REPORT
```

Use `status: manual-sme-complete` only when the requested authoring scope is complete enough for technical review. Use `manual-sme-incomplete` with explicit remaining scope otherwise. The Orchestrator may invoke the SME again without requiring maintainer action.

### Manual UX/IA review result

The Manual UX and Information Architecture Reviewer receives the Analyst context and completed authoring report, reviews/edits the shared worktree for user usefulness and coherent structure, and returns:

```text
STYR_MANUAL_UX_REVIEW
```

Use `status: ux-review-complete` only when the bundle is coherent enough for factual Technical Review. Use `ux-review-blocked` when authoring is too incomplete or a factual ambiguity prevents safe restructuring. The reviewer may improve deterministic capture definitions for documentation value but must not change product behavior.

After this stage, the Orchestrator regenerates and validates the complete screenshot corpus before invoking Technical Review.

### Technical review result

The Technical Reviewer receives the Analyst context, completed authoring report, and completed `STYR_MANUAL_UX_REVIEW`, verifies/corrects the current worktree, and returns:

```text
STYR_MANUAL_TECHNICAL_REVIEW
```

Use `status: technical-review-complete` only when no unresolved factual blocker remains. Use `technical-review-blocked` otherwise. Editorial work must not start on a blocked review.

### Editorial result

The US-English Editor receives the completed technical review, performs the final language pass, and returns:

```text
STYR_MANUAL_EDITORIAL_REPORT
```

Use `status: editorial-complete` only after the permitted editorial scope is complete and established technical meaning has been preserved.

## No internal handoff artifacts

Do not create or commit internal orchestration state such as:

```text
.github/documentation-sync/work/
docs/analysis/
PROVENANCE.md
```

The repository-cleanliness gate intentionally rejects these paths. The Orchestrator carries handoffs in task context, not in public repository history.

## Full regeneration versus incremental sync

`full-regeneration` reconstructs the complete current en-US manual from authoritative evidence after mandatory ground-up discovery. It is not a large release-delta pass: the current product surface, workflows, interactions, and UI states define scope; the changelog and existing manual are secondary history/comparison sources. Authoring then follows the reader journey in `manual-information-architecture.md`. It is appropriate for initial bootstrap, completeness testing, and deliberate audits. The result remains review material until human approval.

`incremental-release-sync` is the normal release-maintenance mode once the initial manual is established. It preserves unaffected structure, stable IDs/paths, and unaffected prose while updating the complete user-visible release delta and transitive documentation impact.

## Failure and resume behavior

The Orchestrator advances only after the preceding specialist reports a valid completed status.

- An incomplete Analyst is reinvoked to close identified coverage gaps.
- An incomplete SME may be reinvoked on the same worktree.
- A blocked UX/IA review stops the pipeline until the documentation-quality problem is resolved.
- A blocked Technical Review stops the pipeline for human intervention.
- The Editor never resolves factual ambiguity by rewriting around it.

If the Copilot task is later resumed on the same Draft PR, the Orchestrator inspects the existing documentation diff and available task context and continues conservatively from the first stage that is not demonstrably complete. If stage state is uncertain, rerun the necessary analysis/review rather than guessing.

## Final PR and release

The initial issue assignment already created the one Draft PR. After all specialist stages complete, the Orchestrator runs the required deterministic documentation/control/cleanliness checks and leaves that Draft PR ready for human review.

The Orchestrator and specialists never merge their own work. Release documentation is accepted only after deterministic CI and human approval.

## Authorization

The public issue tracker is not an agent-control API. The official issue is created only by the maintainer-only manual workflow and carries `internal:documentation-sync`. A copied issue body, public comment, or unlabeled issue does not authorize the Orchestrator.

The Orchestrator is manually selectable (`user-invocable: true`, `disable-model-invocation: true`). Specialist agents are programmatic-only (`user-invocable: false`) and are invoked only through the Orchestrator's `agent` tool.
