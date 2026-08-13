#!/usr/bin/env python3
# Purpose: Rejects generated build/test artifacts accidentally left in repository source roots.
# Author: Axel Napolitano — Styr build-system modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Keep generated test/build artifacts out of source/package roots."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
errors = []

root_generated_patterns = (
    re.compile(r"^shape-\d{3}\.png$", re.IGNORECASE),
    re.compile(r".*\.(?:elf|hex|srec|map|gcda|gcno|gcov|profraw)$", re.IGNORECASE),
)

for path in ROOT.iterdir():
    if not path.is_file():
        continue
    for pattern in root_generated_patterns:
        if pattern.fullmatch(path.name):
            errors.append(f"generated artifact in repository root: {path.name}")
            break

for generated_dir in (ROOT / ".pio", ROOT / "build"):
    if generated_dir.exists():
        errors.append(f"generated build directory in repository root: {generated_dir.name}/")


# Maintainer analysis/provenance working notes are intentionally kept outside
# the public repository. User-facing attribution belongs in README/source
# headers; detailed audit ledgers remain private working material.
for internal_path in (ROOT / "PROVENANCE.md", ROOT / "docs" / "analysis"):
    if internal_path.exists():
        errors.append(f"internal-only documentation packaged in repository: {internal_path.relative_to(ROOT)}")

for cache_dir in ROOT.rglob("__pycache__"):
    if cache_dir.is_dir():
        errors.append(f"Python bytecode cache in repository: {cache_dir.relative_to(ROOT)}/")
for pyc in ROOT.rglob("*.pyc"):
    if pyc.is_file():
        errors.append(f"Python bytecode file in repository: {pyc.relative_to(ROOT)}")

# Product unit tests are assertions, not documentation/rendering jobs. Keeping
# file-producing graphics helpers here previously leaked shape-*.png into full
# repository packages when tests were executed from PROJECT_DIR.
for path in (ROOT / "test").rglob("*.cpp"):
    text = path.read_text(encoding="utf-8", errors="replace")
    relative = path.relative_to(ROOT)
    for token in ("stbi_write_png", "shape-%", "shape-001.png", "test_01_markdown"):
        if token in text:
            errors.append(f"generated-artifact side effect {token!r} in product test: {relative}")

if errors:
    print("Repository cleanliness validation FAILED:")
    for error in errors:
        print(f" - {error}")
    sys.exit(1)

print("Repository cleanliness validation OK:")
print(" - no generated shape-* images in repository root")
print(" - no root .pio/ or build/ directories")
print(" - no Python __pycache__/ or .pyc artifacts")
print(" - no build/coverage binaries in repository root")
print(" - no documentation-image writers in PlatformIO product tests")
print(" - no internal docs/analysis or root PROVENANCE.md packaged")
