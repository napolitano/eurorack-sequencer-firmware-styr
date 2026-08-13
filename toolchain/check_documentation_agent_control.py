#!/usr/bin/env python3
# Purpose: Enforces the maintainer-only control surface and branch-only orchestration contract for user-documentation agents.
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
        "Do not assign this issue to Copilot",
        "Agents",
        "Styr Release Documentation Analyst",
        "Styr Manual SME",
        "Styr Manual Technical Reviewer",
        "Styr Manual US English Editor",
        ".github/documentation-sync/work/issue-",
        "Do not open a pull request",
    )
    for token in required:
        if token not in text:
            errors.append(f"documentation-sync workflow missing required contract token: {token}")

    # The control surface must remain manual-only. Reject common automatic
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

    # The workflow may create the control issue, but it must never assign it
    # to Copilot. Issue assignment always creates a PR and breaks the
    # branch-only multi-agent chain.
    if re.search(r"(?m)^\s*gh\s+issue\s+create\b[^\n]*--assignee\b", text):
        errors.append("documentation-sync workflow must not assign the control issue to Copilot or any agent")
    if re.search(r"(?m)^\s*gh\s+issue\s+edit\b[^\n]*--add-assignee\b", text):
        errors.append("documentation-sync workflow must not add an issue assignee")

agent_contracts = {
    ".github/agents/styr-release-documentation-analyst.agent.md": (
        "disable-model-invocation: true",
        "Agents",
        "Do not open a pull request",
        "impact-set.md",
        "tools: [read, search, edit, execute]",
    ),
    ".github/agents/styr-manual-sme.agent.md": (
        "disable-model-invocation: true",
        "Agents",
        "Do not open a pull request",
        "impact-set.md",
        "authoring-report.md",
    ),
    ".github/agents/styr-manual-technical-reviewer.agent.md": (
        "disable-model-invocation: true",
        "Agents",
        "Do not open a pull request",
        "authoring-report.md",
        "technical-review.md",
    ),
    ".github/agents/styr-manual-editor.agent.md": (
        "disable-model-invocation: true",
        "Agents",
        "Do not open a pull request",
        "technical-review-complete",
        "check_repository_cleanliness.py",
        "tools: [read, search, edit, execute]",
    ),
}

for relative, required_tokens in agent_contracts.items():
    path = ROOT / relative
    if not path.is_file():
        errors.append(f"documentation agent missing: {relative}")
        continue
    text = path.read_text(encoding="utf-8")
    for token in required_tokens:
        if token not in text:
            errors.append(f"documentation agent contract missing {token!r}: {relative}")

skill = ROOT / ".github" / "skills" / "styr-user-manual" / "SKILL.md"
orchestration = ROOT / ".github" / "skills" / "styr-user-manual" / "knowledge" / "orchestration.md"

if not skill.is_file():
    errors.append("shared manual skill is missing")
else:
    skill_text = skill.read_text(encoding="utf-8")
    for token in ("internal:documentation-sync", "do not assign", "orchestration.md"):
        if token.lower() not in skill_text.lower():
            errors.append(f"shared manual skill missing orchestration/authorization contract token: {token}")

if not orchestration.is_file():
    errors.append("shared documentation-agent orchestration knowledge module is missing")
else:
    orchestration_text = orchestration.read_text(encoding="utf-8")
    for token in (
        "branch-only",
        "issue-<N>",
        "impact-set.md",
        "authoring-report.md",
        "technical-review.md",
        "single bundled documentation PR",
    ):
        if token.lower() not in orchestration_text.lower():
            errors.append(f"orchestration knowledge missing required token: {token}")

if errors:
    print("Documentation agent control validation FAILED:")
    for error in errors:
        print(f" - {error}")
    sys.exit(1)

print("Documentation agent control validation OK:")
print(" - documentation sync control issue creation is workflow_dispatch-only")
print(" - public documentation-sync issue form is absent")
print(" - workflow token permissions are limited to contents:read and issues:write")
print(" - control issues are not assigned to Copilot/agents")
print(" - all documentation stages are explicit branch-only custom-agent sessions")
print(" - Analyst -> SME -> Technical Reviewer -> US-English Editor handoffs are contracted")
print(" - only the human maintainer opens the final bundled documentation PR")
