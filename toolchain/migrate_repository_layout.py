#!/usr/bin/env python3
# Purpose: Removes repository artifacts that became obsolete after documented layout changes.
# Author: Axel Napolitano — Styr repository tooling
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Apply safe, one-way cleanup for obsolete Styr repository paths.

This migration is intentionally conservative: it only removes paths that are
known to have been retired from the public repository structure. It does not
rewrite current files or modify product source code.
"""

from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]

OBSOLETE_FILES = (
    ROOT / ".github" / "ISSUE_TEMPLATE" / "documentation_sync.yml",
    ROOT / "PROVENANCE.md",
)

OBSOLETE_DIRECTORIES = (
    ROOT / "docs" / "analysis",
)

removed: list[str] = []

for path in OBSOLETE_FILES:
    if path.is_file() or path.is_symlink():
        path.unlink()
        removed.append(path.relative_to(ROOT).as_posix())

for path in OBSOLETE_DIRECTORIES:
    if path.is_dir():
        shutil.rmtree(path)
        removed.append(path.relative_to(ROOT).as_posix() + "/")

if removed:
    print("Repository layout migration removed obsolete paths:")
    for relative in removed:
        print(f" - {relative}")
else:
    print("Repository layout migration: no obsolete paths found")
