# User-manual agent workflow

Styr uses a maintainer-triggered, branch-only GitHub Copilot custom-agent chain for user-manual maintenance. The repository keeps the manual, screenshot definitions, and shared subject-matter Brain versioned with the firmware, but the agents do not write directly to the protected development branch.

## Why the control issue is not assigned to Copilot

GitHub issue assignment always starts Copilot with a pull request. That is useful for a single implementation task, but it is the wrong execution model for Styr documentation because the first stage is analysis-only and several specialist stages must share one linear work history before a single bundled PR is reviewed.

The documentation-sync issue is therefore a **control/status object only**. Do not assign it to Copilot or a custom agent.

## Start a synchronization

1. Open **Actions**.
2. Run **Create documentation sync control issue**.
3. Select `full-regeneration` or `incremental-release-sync`.
4. Supply the target, the previous-release baseline when incremental, and the final PR base branch.
5. Open the generated issue. It contains the authorization label, exact parameters, stage checklist, and prompt templates.

The workflow is manual-only (`workflow_dispatch`) and requires repository write access. It does not start Copilot.

## Run the agent chain

For every stage, open the GitHub **Agents** panel/tab and start a task with a prompt. Explicitly select the named custom agent. Prompt-started tasks work on a branch by default; do not ask intermediate stages to create a PR.

### 1. Release Documentation Analyst

- Agent: **Styr Release Documentation Analyst**
- Base branch: requested target branch/state
- Prompt: reference the authorized sync issue and request branch-only analysis
- Output: transient `.github/documentation-sync/work/issue-<N>/impact-set.md`

The Analyst does not write manual prose.

### 2. Manual SME

- Agent: **Styr Manual SME**
- Base branch: **the completed Analyst branch**
- Prompt: reference the same sync issue and require the Analyst handoff
- Output: complete manual/Brain/screenshot-definition authoring bundle plus transient `authoring-report.md`

For a large full regeneration, additional SME sessions may continue from the current SME branch until authoring is complete.

### 3. Technical Reviewer

- Agent: **Styr Manual Technical Reviewer**
- Base branch: **the completed SME branch**
- Output: factual corrections plus transient `technical-review.md`

The review must be `technical-review-complete` before editorial work begins.

### 4. US-English Editor

- Agent: **Styr Manual US English Editor**
- Base branch: **the completed Technical Reviewer branch**
- Output: final en-US prose cleanup

The Editor also removes the complete transient sync workspace and runs the available documentation/control/cleanliness checks. It does not open a PR.

### 5. Human PR

After the Editor finishes:

1. verify that `.github/documentation-sync/work/issue-<N>/` is absent from the final diff;
2. open exactly one **Draft PR** from the Editor branch to the base branch recorded in the control issue;
3. review the complete documentation bundle;
4. require deterministic CI;
5. merge only after human approval.

The final PR contains all earlier agent commits through branch ancestry, but the temporary handoff files are absent from the final tree.

## Branch lineage is mandatory

Do not start every specialist from `main`.

```text
main/target
   -> Analyst branch
      -> SME branch
         -> Technical Reviewer branch
            -> Editor branch
               -> one PR back to main/final base
```

The previous agent branch is the next agent's base branch. Starting a later stage from the original base loses the preceding stage's work and breaks the handoff contract.

## Full regeneration versus incremental sync

`full-regeneration` reconstructs the complete current en-US manual from authoritative evidence. It is appropriate for initial bootstrap, completeness testing, and deliberate audits. It is still review material; the result is not automatically merged.

`incremental-release-sync` is the normal release-maintenance mode once the initial manual is established. It preserves unaffected structure, stable IDs/paths, and unaffected prose while updating the complete user-visible release delta and transitive documentation impact.

## Authorization

The public issue tracker is not an agent-control interface. The official control issue is created only by the maintainer-only workflow and carries `internal:documentation-sync`. A copied issue body or public issue does not authorize documentation work.

The agent profiles are `disable-model-invocation: true`; agent selection remains an explicit maintainer action.
