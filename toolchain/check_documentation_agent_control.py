#!/usr/bin/env python3
# Purpose: Enforces the maintainer-only control surface for user-documentation agents.
# Author: Axel Napolitano — Styr documentation tooling
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Validate documentation-agent trigger and authorization boundaries."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
errors: list[str] = []

public_form = ROOT / ".github" / "ISSUE_TEMPLATE" / "documentation_sync.yml"
workflow = ROOT / ".github" / "workflows" / "create-documentation-sync.yml"

if public_form.exists():
    errors.append("public documentation-sync issue form must not exist")

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
    )
    for token in required:
        if token not in text:
            errors.append(f"documentation-sync workflow missing required contract token: {token}")

    # This control surface must remain manual-only. Reject common automatic
    # workflow triggers even if someone later adds them beside workflow_dispatch.
    on_block_match = re.search(r"(?ms)^on:\s*\n(?P<body>.*?)(?=^[A-Za-z_][A-Za-z0-9_-]*:\s*$)", text)
    if on_block_match:
        on_block = on_block_match.group("body")
        for forbidden in ("push:", "pull_request:", "issues:", "schedule:"):
            if re.search(rf"(?m)^\s{{2}}{re.escape(forbidden)}\s*$", on_block):
                errors.append(f"documentation-sync workflow must not use automatic trigger {forbidden}")
    else:
        errors.append("unable to inspect documentation-sync workflow trigger block")

for relative in (
    ".github/agents/styr-release-documentation-analyst.agent.md",
    ".github/agents/styr-manual-sme.agent.md",
):
    path = ROOT / relative
    if not path.is_file():
        errors.append(f"documentation agent missing: {relative}")
        continue
    text = path.read_text(encoding="utf-8")
    if "disable-model-invocation: true" not in text:
        errors.append(f"documentation agent must disable automatic model invocation: {relative}")
    if "internal:documentation-sync" not in text:
        errors.append(f"issue-driven authorization guard missing from: {relative}")

skill = ROOT / ".github" / "skills" / "styr-user-manual" / "SKILL.md"
if not skill.is_file() or "internal:documentation-sync" not in skill.read_text(encoding="utf-8"):
    errors.append("shared manual skill must define the issue-driven authorization label")

if errors:
    print("Documentation agent control validation FAILED:")
    for error in errors:
        print(f" - {error}")
    sys.exit(1)

print("Documentation agent control validation OK:")
print(" - documentation sync creation is workflow_dispatch-only")
print(" - public documentation-sync issue form is absent")
print(" - workflow token permissions are limited to contents:read and issues:write")
print(" - authorized issues carry internal:documentation-sync")
print(" - issue-driven analyst/SME runs require the authorization label")
