# GitHub Wiki publishing model

The GitHub Wiki is **not** a canonical Styr documentation source.

GitHub stores a repository wiki as a separate Git repository (`<repository>.wiki.git`). Because it is outside the main repository's normal branches and release tags, maintaining authoritative text independently in both places would create two version histories and two opportunities for documentation drift.

## Intended model

Canonical Markdown remains in the main Styr repository:

- end-user documentation under `docs/manual/`;
- technical/developer documentation under `docs/development/`;
- shared agent subject-matter knowledge under `.github/skills/styr-user-manual/knowledge/`.

A future wiki publisher may project selected, already-reviewed Markdown into the separate GitHub Wiki repository after the source documentation has been merged.

```text
main repository Markdown
        ↓
validated documentation state
        ↓
explicit wiki publication job
        ↓
<repository>.wiki.git
```

## Agent policy

Documentation agents should not directly author a second independent version of a page in the Wiki.

If Wiki publication is implemented later:

1. the Manual SME edits the canonical main-repository Markdown through the normal documentation PR;
2. technical and editorial review happen on that same source;
3. deterministic documentation checks pass;
4. after merge, an explicit publisher clones the Wiki Git repository, updates only the mapped pages from canonical source, and pushes the publication commit;
5. the Wiki commit records the source repository commit/tag it was generated from where practical.

The publisher should be a script/workflow rather than an LLM decision path. This keeps Wiki updates reproducible and prevents a language or factual divergence between the release manual and the Wiki.

## What the Wiki may contain

A future Wiki can reasonably expose selected long-form material such as:

- user guide entry/navigation pages;
- feature guides;
- troubleshooting and update guidance;
- contributor/developer landing pages.

Generated release artifacts such as ODT/PDF remain products of the documentation build, not Wiki sources.

No Wiki publication workflow is implemented yet.
