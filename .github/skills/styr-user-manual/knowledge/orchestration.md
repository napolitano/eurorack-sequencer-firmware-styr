---
knowledge-id: styr.manual.orchestration
status: canonical
locale: en-US
source-locale: en-US
active-locales: [en-US]
---

# Documentation-agent orchestration

Styr documentation synchronization is a **prompt-driven, branch-only agent chain**. The maintainer-only control issue records authorization and parameters; it is not assigned to Copilot.

GitHub issue assignment is intentionally not used for this process because assigning an issue to Copilot always creates a pull request. Styr needs several specialist stages to operate on one linear branch history before a single bundled documentation PR exists.

## Official entry point

Run `.github/workflows/create-documentation-sync.yml` manually with `workflow_dispatch`. The workflow creates an issue carrying `internal:documentation-sync` and records:

- sync issue number;
- authoring mode;
- target ref;
- comparison baseline when required;
- final PR base branch;
- optional documentation emphasis.

Do **not** assign that issue to Copilot or a custom agent.

## Branch-only stage chain

Start every agent from the GitHub Agents panel/tab using a prompt and explicitly select the required custom agent.

```text
requested target/base state
        |
        v
Styr Release Documentation Analyst
        |
        | analyst branch
        v
Styr Manual SME
        |
        | author branch
        v
Styr Manual Technical Reviewer
        |
        | technically reviewed branch
        v
Styr Manual US English Editor
        |
        | clean final documentation branch
        v
maintainer opens one Draft PR to the requested final base branch
```

Each stage after the Analyst **must use the immediately preceding agent branch as its base branch**. This linear ancestry is how the complete bundle and its agent commit history are preserved. Starting a later stage from the original base branch discards earlier work.

Intermediate stages must not request or open a pull request. The maintainer opens exactly one final Draft PR after the editorial stage has completed and removed the transient handoff workspace.

## Transient handoff workspace

Agent-to-agent state is committed temporarily under:

```text
.github/documentation-sync/work/issue-<N>/
```

This workspace exists only on the chained agent branches. It must be absent from the final PR diff and must never be merged into the maintained branch.

### Analyst handoff

The Analyst creates:

```text
.github/documentation-sync/work/issue-<N>/impact-set.md
```

Required frontmatter:

```yaml
sync_issue: <N>
mode: full-regeneration | incremental-release-sync
target: <ref>
baseline: <ref-or-null>
final_base_branch: <branch>
stage: analyst-complete
```

Required body sections:

- Coverage / user-visible impact
- Existing documentation IDs and paths affected
- Required new documentation IDs and paths
- Screenshot additions or changes
- What's New candidates when applicable
- Version-history candidates
- Evidence and source locations
- Explicitly excluded internal changes
- Unresolved evidence questions

The Analyst may edit only this transient workspace. It does not author manual prose.

### Manual SME handoff

The Manual SME requires a valid `impact-set.md`, performs the complete authoring task, and then creates:

```text
.github/documentation-sync/work/issue-<N>/authoring-report.md
```

with `stage: manual-sme-complete` and a concise record of changed manual/knowledge/screenshot sources, validation performed, and unresolved blockers. If authoring is incomplete, use `stage: manual-sme-incomplete`; the Technical Reviewer must not proceed.

A large full regeneration may require more than one Manual SME session. Continue from the current SME branch until `manual-sme-complete` is justified.

### Technical review handoff

The Technical Reviewer requires `manual-sme-complete`, reviews and corrects the documentation bundle, then creates:

```text
.github/documentation-sync/work/issue-<N>/technical-review.md
```

Use `stage: technical-review-complete` only when no unresolved factual blocker remains. Otherwise use `stage: technical-review-blocked` and describe the blocker. The Editor must not proceed on a blocked review.

### Editorial completion

The US-English Editor requires `technical-review-complete`. It performs only the final language pass permitted by its agent contract. When editorial work is complete it may create a temporary `editorial-review.md` for its own final verification.

Before ending the session, the Editor must:

1. ensure there is no unresolved technical-review blocker;
2. remove the complete `.github/documentation-sync/work/issue-<N>/` directory;
3. run the required deterministic documentation/control/cleanliness checks available in the environment;
4. leave the branch ready for the maintainer to open the single bundled Draft PR.

The Editor must not open the PR itself. The human maintainer selects the final PR base branch explicitly.

## Final PR and release

The maintainer opens one Draft PR from the completed Editor branch to the base branch recorded by the control issue. The final PR must contain the documentation/product-facing screenshot-definition changes only; transient orchestration files must not appear in its final diff.

The agent chain does not replace CI or human review. Merge only after deterministic repository/documentation checks pass and the bundled result has been reviewed by a human.

## Authorization boundary

The public issue tracker is not an agent API. The official control issue can only be created through the maintainer-only manual workflow. Direct prompt sessions from the Agents UI are valid when they explicitly reference that authorized sync issue. A copied issue body or arbitrary public issue does not grant write authority.
