#!/usr/bin/env python3
# Purpose: Enforces the maintainer-only control surface and single-issue documentation-agent orchestration contract.
# Author: Axel Napolitano — Styr documentation tooling
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Validate documentation-agent trigger, authorization, and orchestration boundaries."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
errors: list[str] = []

public_form = ROOT / ".github" / "ISSUE_TEMPLATE" / "documentation_sync.yml"
workflow = ROOT / ".github" / "workflows" / "create-documentation-sync.yml"

if public_form.exists():
    errors.append(
        "public documentation-sync issue form must not exist; remove "
        ".github/ISSUE_TEMPLATE/documentation_sync.yml "
        "(or run: python toolchain/migrate_repository_layout.py)"
    )

if not workflow.is_file():
    errors.append("maintainer-only documentation-sync workflow is missing")
else:
    text = workflow.read_text(encoding="utf-8")
    required = (
        "workflow_dispatch:",
        "contents: read",
        "issues: write",
        "internal:documentation-sync",
        "gh issue create",
        "Assign this issue **once**",
        "Styr Documentation Orchestrator",
        "Styr Release Documentation Analyst",
        "Styr Manual SME",
        "Styr Manual UX and Information Architecture Reviewer",
        "Styr Manual Technical Reviewer",
        "Styr Manual US English Editor",
        "single bundled documentation PR",
        "Deterministic CI and human review",
    )
    for token in required:
        if token not in text:
            errors.append(f"documentation-sync workflow missing required contract token: {token}")

    # The control surface is maintainer-triggered only. Reject common automatic
    # triggers even if someone later adds them beside workflow_dispatch.
    on_block_match = re.search(
        r"(?ms)^on:\s*\n(?P<body>.*?)(?=^[A-Za-z_][A-Za-z0-9_-]*:\s*$)", text
    )
    if on_block_match:
        on_block = on_block_match.group("body")
        for forbidden in ("push:", "pull_request:", "issues:", "schedule:"):
            if re.search(rf"(?m)^\s{{2}}{re.escape(forbidden)}\s*$", on_block):
                errors.append(f"documentation-sync workflow must not use automatic trigger {forbidden}")
    else:
        errors.append("unable to inspect documentation-sync workflow trigger block")

    # The workflow creates an authorized issue but must not choose an assignee.
    # Selecting the repository custom Orchestrator is an explicit maintainer action.
    if re.search(r"(?m)^\s*gh\s+issue\s+create\b[^\n]*--assignee\b", text):
        errors.append("documentation-sync workflow must not auto-assign Copilot or an agent")
    if re.search(r"(?m)^\s*gh\s+issue\s+edit\b[^\n]*--add-assignee\b", text):
        errors.append("documentation-sync workflow must not add an issue assignee")

orchestrator = ROOT / ".github" / "agents" / "styr-documentation-orchestrator.agent.md"
if not orchestrator.is_file():
    errors.append("Styr Documentation Orchestrator agent is missing")
else:
    text = orchestrator.read_text(encoding="utf-8")
    required = (
        "name: Styr Documentation Orchestrator",
        "disable-model-invocation: true",
        "user-invocable: true",
        "agent",
        '"github/*"',
        "internal:documentation-sync",
        "Styr Release Documentation Analyst",
        "Styr Manual SME",
        "Styr Manual UX and Information Architecture Reviewer",
        "Styr Manual Technical Reviewer",
        "Styr Manual US English Editor",
        "STYR_DOCUMENTATION_IMPACT_SET",
        "STYR_MANUAL_AUTHORING_REPORT",
        "STYR_MANUAL_UX_REVIEW",
        "STYR_MANUAL_TECHNICAL_REVIEW",
        "STYR_MANUAL_EDITORIAL_REPORT",
        "ux-review-blocked",
        "technical-review-blocked",
        "regenerate_manual_screenshots.py",
        "check_manual_screenshots.py --require-assets",
        "manual-quality.md",
        "full-product-discovery.md",
        "manual-information-architecture.md",
        "product-surface map",
        "user-workflow map",
        "check_documentation_agent_control.py",
        "check_repository_cleanliness.py",
        "single bundled documentation PR",
    )
    for token in required:
        if token not in text:
            errors.append(f"documentation Orchestrator missing required contract token: {token}")

specialist_contracts = {
    ".github/agents/styr-release-documentation-analyst.agent.md": (
        "disable-model-invocation: true",
        "user-invocable: false",
        "Styr Documentation Orchestrator",
        "custom-agent `agent` tool",
        "STYR_DOCUMENTATION_IMPACT_SET",
        "status: analyst-complete",
        "tools: [read, search, execute]",
        "Do not edit repository files",
        "full-product-discovery.md",
        "manual-information-architecture.md",
        "recording-workflows.md",
        "song-and-performance.md",
        "product-surface map",
        "coverage-audit",
    ),
    ".github/agents/styr-manual-sme.agent.md": (
        "disable-model-invocation: true",
        "user-invocable: false",
        "Styr Documentation Orchestrator",
        "STYR_DOCUMENTATION_IMPACT_SET",
        "STYR_MANUAL_AUTHORING_REPORT",
        "status: manual-sme-complete",
        "manual-information-architecture.md",
        "recording-workflows.md",
        "song-and-performance.md",
        "docs/manual/manual.toml",
    ),
    ".github/agents/styr-manual-ux-reviewer.agent.md": (
        "disable-model-invocation: true",
        "user-invocable: false",
        "Styr Documentation Orchestrator",
        "STYR_MANUAL_AUTHORING_REPORT",
        "STYR_MANUAL_UX_REVIEW",
        "status: ux-review-complete",
        "ux-review-blocked",
        "manual-quality.md",
        "manual-information-architecture.md",
        "recording-workflows.md",
        "song-and-performance.md",
        "src/simulator/tools/manual_screenshots.cpp",
    ),
    ".github/agents/styr-manual-technical-reviewer.agent.md": (
        "disable-model-invocation: true",
        "user-invocable: false",
        "Styr Documentation Orchestrator",
        "STYR_MANUAL_AUTHORING_REPORT",
        "STYR_MANUAL_UX_REVIEW",
        "STYR_MANUAL_TECHNICAL_REVIEW",
        "status: technical-review-complete",
        "technical-review-blocked",
    ),
    ".github/agents/styr-manual-editor.agent.md": (
        "disable-model-invocation: true",
        "user-invocable: false",
        "Styr Documentation Orchestrator",
        "STYR_MANUAL_TECHNICAL_REVIEW",
        "STYR_MANUAL_EDITORIAL_REPORT",
        "status: editorial-complete",
        "check_repository_cleanliness.py",
    ),
}

for relative, required_tokens in specialist_contracts.items():
    path = ROOT / relative
    if not path.is_file():
        errors.append(f"documentation specialist agent missing: {relative}")
        continue
    text = path.read_text(encoding="utf-8")
    for token in required_tokens:
        if token not in text:
            errors.append(f"documentation specialist contract missing {token!r}: {relative}")

skill = ROOT / ".github" / "skills" / "styr-user-manual" / "SKILL.md"
orchestration = ROOT / ".github" / "skills" / "styr-user-manual" / "knowledge" / "orchestration.md"

if not skill.is_file():
    errors.append("shared manual skill is missing")
else:
    skill_text = skill.read_text(encoding="utf-8")
    for token in (
        "internal:documentation-sync",
        "Styr Documentation Orchestrator",
        "custom-agent `agent` tool",
        "STYR_DOCUMENTATION_IMPACT_SET",
        "Styr Manual UX and Information Architecture Reviewer",
        "manual-quality.md",
        "full-product-discovery.md",
        "manual-information-architecture.md",
        "recording-workflows.md",
        "song-and-performance.md",
        "user-invocable: false",
        "orchestration.md",
    ):
        if token.lower() not in skill_text.lower():
            errors.append(f"shared manual skill missing orchestration/authorization contract token: {token}")

if not orchestration.is_file():
    errors.append("shared documentation-agent orchestration knowledge module is missing")
else:
    orchestration_text = orchestration.read_text(encoding="utf-8")
    for token in (
        "one authorized issue assignment",
        "Styr Documentation Orchestrator",
        "agent` tool",
        "STYR_DOCUMENTATION_IMPACT_SET",
        "STYR_MANUAL_AUTHORING_REPORT",
        "STYR_MANUAL_UX_REVIEW",
        "STYR_MANUAL_TECHNICAL_REVIEW",
        "STYR_MANUAL_EDITORIAL_REPORT",
        "not committed as repository files",
        "same Draft PR",
        "Styr Manual UX and Information Architecture Reviewer",
        "complete screenshot corpus",
        "user-invocable: false",
    ):
        if token.lower() not in orchestration_text.lower():
            errors.append(f"orchestration knowledge missing required token: {token}")

screenshot_regenerator = ROOT / "toolchain" / "regenerate_manual_screenshots.py"
if not screenshot_regenerator.is_file():
    errors.append("canonical manual screenshot regeneration helper is missing")
else:
    regenerator_text = screenshot_regenerator.read_text(encoding="utf-8")
    for token in ("manual-screenshots", "--require-assets", "windows-ucrt64-debug", "release"):
        if token not in regenerator_text:
            errors.append(f"manual screenshot regeneration helper missing required token: {token}")

quality = ROOT / ".github" / "skills" / "styr-user-manual" / "knowledge" / "manual-quality.md"
if not quality.is_file():
    errors.append("shared manual quality contract is missing")
else:
    quality_text = quality.read_text(encoding="utf-8")
    for token in (
        "Existing prose is an asset",
        "Regeneration is not permission",
        "Do not turn the manual into a UI inventory",
        "Screenshot quality",
        "Good and bad transformations",
    ):
        if token.lower() not in quality_text.lower():
            errors.append(f"manual quality contract missing required token: {token}")

required_knowledge_modules = {
    ".github/skills/styr-user-manual/knowledge/full-product-discovery.md": (
        "Full product discovery contract",
        "Mandatory discovery passes",
        "Product surface inventory",
        "Workflow discovery",
        "Feature interaction discovery",
        "Screenshot/state coverage",
        "Manual architecture plan",
        "Changelog relationship",
        "Existing manual relationship",
    ),
    ".github/skills/styr-user-manual/knowledge/manual-information-architecture.md": (
        "Manual information architecture",
        "Part I — Meet Styr",
        "Part II — How Styr thinks",
        "Part III — Basic use",
        "Part IV — Creating and recording material",
        "Part V — Patterns and arrangement",
        "Part VI — Performance Mode",
        "Part VII — Advanced sequencing and modulation",
        "Part IX — Reference",
        "docs/manual/manual.toml",
    ),
    ".github/skills/styr-user-manual/knowledge/recording-workflows.md": (
        "Recording workflows",
        "Step Record",
        "Overdub",
        "Overwrite",
        "Curve recording",
        "Recommended Note-recording learning workflow",
        "PAGE` + `PLAY",
    ),
    ".github/skills/styr-user-manual/knowledge/song-and-performance.md": (
        "Song Mode and Performance Mode",
        "Recommended Song workflow",
        "Immediate, Latch, and Sync mental model",
        "Recommended live workflow",
        "Song slot",
        "Fill Amount",
    ),
}
for relative, tokens in required_knowledge_modules.items():
    path = ROOT / relative
    if not path.is_file():
        errors.append(f"required manual knowledge module is missing: {relative}")
        continue
    text = path.read_text(encoding="utf-8")
    for token in tokens:
        if token.lower() not in text.lower():
            errors.append(f"manual knowledge module missing required token {token!r}: {relative}")

# The obsolete manual-branch-chain architecture must not creep back into the
# maintainer workflow. Specialist documents may discuss branches generically,
# but these exact operational instructions are retired.
retired_phrases = (
    "Do not assign this issue to Copilot or to a custom agent",
    "every stage after the Analyst must use the previous stage's branch",
    "Start each stage from the GitHub **Agents** panel",
)
for relative in (
    ".github/workflows/create-documentation-sync.yml",
    "docs/development/documentation/AGENT_WORKFLOW.md",
    ".github/skills/styr-user-manual/knowledge/orchestration.md",
):
    path = ROOT / relative
    if path.is_file():
        text = path.read_text(encoding="utf-8")
        for phrase in retired_phrases:
            if phrase in text:
                errors.append(f"retired manual branch-chain instruction remains in {relative}: {phrase}")

if errors:
    print("Documentation agent control validation FAILED:")
    for error in errors:
        print(f" - {error}")
    sys.exit(1)

print("Documentation agent control validation OK:")
print(" - documentation sync issue creation is workflow_dispatch-only")
print(" - public documentation-sync issue form is absent")
print(" - workflow token permissions are limited to contents:read and issues:write")
print(" - workflow does not auto-assign an agent")
print(" - Styr Documentation Orchestrator is the single maintainer-facing agent")
print(" - Orchestrator can invoke specialist custom agents with the agent tool")
print(" - specialist agents are programmatic-only (user-invocable:false)")
print(" - Analyst -> SME -> UX/IA Reviewer -> Technical Reviewer -> US-English Editor handoffs stay in agent context")
print(" - full-regeneration requires complete screenshot regeneration and strict asset validation")
print(" - the manual quality contract is present and enforced by agent profiles")
print(" - full regeneration is governed by ground-up product discovery and a stable reader journey")
print(" - recording, Song Mode, and Performance Mode workflow knowledge is present")
print(" - the initial issue assignment produces the single bundled Draft PR")
