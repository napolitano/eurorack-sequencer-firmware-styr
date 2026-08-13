# User-manual agent workflow

Styr uses a maintainer-triggered GitHub Copilot documentation pipeline with one visible agent assignment and one bundled Draft PR. The repository keeps the manual, screenshot definitions, and shared subject-matter Brain versioned with the firmware, while specialist responsibilities remain separated internally.

## Start a synchronization

1. Open **Actions**.
2. Run **Create documentation sync control issue**.
3. Select `full-regeneration` or `incremental-release-sync`.
4. Supply the target, the previous-release baseline when incremental, and the final PR base branch.
5. Open the generated issue.
6. Assign the issue **once** to **Styr Documentation Orchestrator** using GitHub's custom-agent selector.

That is the complete maintainer trigger. Do not start the Analyst, Manual SME, Technical Reviewer, or Editor manually.

The workflow is manual-only (`workflow_dispatch`) and requires repository write access. It creates the authorized issue but does not auto-assign an agent, so public users cannot turn arbitrary issues into documentation-agent tasks.

## What happens after assignment

GitHub issue assignment creates the one Draft PR for the task. The Orchestrator then uses the custom-agent `agent` tool to run the internal specialists in order:

```text
Styr Documentation Orchestrator
        |
        +--> Styr Release Documentation Analyst
        |
        +--> Styr Manual SME
        |
        +--> Styr Manual Technical Reviewer
        |
        `--> Styr Manual US English Editor
        |
        v
same Draft PR -> CI -> human review
```

The maintainer does not copy prompts, select branches, or hand work from one specialist to another.

### Release Documentation Analyst

The Analyst is evidence-only. It inventories user-visible coverage/changes, affected stable documentation IDs, screenshot needs, What's New/version-history scope, evidence, exclusions, and unresolved questions. It returns that impact set to the Orchestrator and does not edit repository files.

### Manual SME

The SME receives the Analyst result and authors the complete documentation bundle. It understands Styr sequencing, Eurorack/MIDI/CV terminology, UI workflows, current implementation evidence, and the distinction between full regeneration and incremental maintenance.

For a large regeneration the Orchestrator may invoke the SME more than once on the same worktree until the SME reports the authoring scope complete.

### Technical Reviewer

The Technical Reviewer verifies and, when evidence is clear, corrects user-facing facts against implementation, tests, UI state, defaults/ranges, and screenshot definitions. A factual blocker stops the pipeline before editorial review.

### US-English Editor

The Editor performs the final en-US language pass without changing established technical meaning.

## Handoffs are not repository files

The four specialists return structured results to the Orchestrator. Internal impact/review handoffs are kept in agent task context and are not committed to the repository.

The following paths remain forbidden in the maintained repository:

```text
.github/documentation-sync/work/
docs/analysis/
PROVENANCE.md
```

This avoids publishing internal analysis merely to orchestrate the agents.

## Full regeneration versus incremental sync

`full-regeneration` reconstructs the complete current en-US manual from authoritative evidence. It is intended for initial bootstrap, completeness testing, periodic audit, and deliberate recovery.

`incremental-release-sync` is the normal release-maintenance mode after the initial manual exists. It preserves unaffected structure, stable IDs/paths, and unaffected prose while incorporating the complete user-visible delta since the selected baseline. `What's New` covers only that interval; the version history remains cumulative.

## Failure and continuation

The Orchestrator never advances past an incomplete or blocked specialist result. It may reinvoke an incomplete Analyst or SME automatically. A blocked Technical Review requires human intervention.

If a long task must be resumed on its existing Draft PR, continue with the same Orchestrator. It is instructed to inspect the current diff and resume conservatively instead of restarting the complete pipeline blindly.

## Completion

After the Editor finishes, the Orchestrator runs the available deterministic documentation/control/cleanliness checks and leaves the existing Draft PR ready for human review. Agents do not merge their own documentation. CI and human approval remain mandatory.

## Authorization

Only issues created by the maintainer-only workflow and carrying `internal:documentation-sync` authorize this process. The Orchestrator verifies that authorization before delegating. Specialist agents are `user-invocable: false`, so they cannot be selected manually as alternative public entry points.
