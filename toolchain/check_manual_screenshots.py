#!/usr/bin/env python3
# Purpose: Validates the one-to-one contract between manual captures and screen docs.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Validate the one-to-one contract between manual captures and screen docs."""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "src/simulator/tools/manual_screenshots.cpp"
SCREENS = ROOT / "docs/manual/screens"


REQUIRED_CAPTURES = {
    "header",
    "footer",
    "lfo-shape-sine",
    "lfo-shape-triangle",
    "lfo-shape-ramp-up",
    "lfo-shape-ramp-down",
    "lfo-shape-square",
    "lfo-shape-random",
    "lfo-shape-smoothed-random",
    "lfo-shape-noise",
    "note-shift-gate",
    "note-shift-retrigger",
    "note-shift-length",
    "note-shift-note",
    "note-shift-condition",
    "curve-shift-shape",
    "curve-shift-min",
    "curve-shift-max",
    "curve-shift-gate",
    "generator-euclidean-commit-menu",
    "generator-random-commit-menu",
    "acid-bassline-commit-menu",
    "acid-bassline-committed",
}

LFO_RANGE_FIXTURE_SNIPPETS = (
    "lfo.setLow(-500, false)",
    "lfo.setLow(-500, true)",
    "lfo.setHi(500, false)",
    "lfo.setHi(500, true)",
)

CAPTURE_PATTERNS = [
    r'\.screenshot\("([a-z0-9-]+)"\)',
    r'\.screenshotRegion\("([a-z0-9-]+)"',
    r'\{\s*NoteSequence::Layer::\w+,\s*"([a-z0-9-]+)"\s*\}',
    r'\{\s*CurveSequence::Layer::\w+,\s*"([a-z0-9-]+)"\s*\}',
    r'\{\s*NoteSequence::Layer::\w+,\s*Key::F\d,\s*"([a-z0-9-]+)"\s*\}',
    r'\{\s*CurveSequence::Layer::\w+,\s*Key::F\d,\s*"([a-z0-9-]+)"\s*\}',
    r'quickEdit\(Key::Step\d+,\s*"([a-z0-9-]+)"(?:,\s*\d+)?\)',
    r'openGenerator\([^\n]*"([a-z0-9-]+)"\)',
    r'captureGeneratorParameter\([^\n]*"([a-z0-9-]+)"',
    r'commitGenerator\([^\n]*"([a-z0-9-]+)",\s*"([a-z0-9-]+)"\)',
    r'\{\s*LfoTrack::Waveform::\w+,\s*"([a-z0-9-]+)"\s*\}',
]


def capture_names():
    text = SOURCE.read_text(encoding="utf-8")
    names = []
    for pattern in CAPTURE_PATTERNS:
        for match in re.finditer(pattern, text):
            names.extend(value for value in match.groups() if value)
    duplicates = sorted({name for name in names if names.count(name) > 1})
    return set(names), duplicates


def screen_docs():
    docs = [path for path in SCREENS.rglob("*.md") if path.name != "README.md"]
    return {path.stem: path for path in docs}


def main():
    errors = []
    captures, duplicates = capture_names()
    docs = screen_docs()

    if duplicates:
        errors.append("duplicate capture names in manual_screenshots.cpp: " + ", ".join(duplicates))

    missing_required = sorted(REQUIRED_CAPTURES - captures)
    if missing_required:
        errors.append("required documentation captures are missing: " + ", ".join(missing_required))

    source_text = SOURCE.read_text(encoding="utf-8")
    if re.search(r'["\'][^"\']*\.md["\']', source_text):
        errors.append("manual_screenshots.cpp must not contain Markdown output filenames")

    missing_lfo_fixture = [snippet for snippet in LFO_RANGE_FIXTURE_SNIPPETS if snippet not in source_text]
    if missing_lfo_fixture:
        errors.append("LFO documentation fixture no longer pins both base/routed lanes to +/-5.00 V")

    missing_docs = sorted(captures - set(docs))
    stale_docs = sorted(set(docs) - captures)
    if missing_docs:
        errors.append("captures without Markdown screen docs: " + ", ".join(missing_docs))
    if stale_docs:
        errors.append("screen docs without captures: " + ", ".join(stale_docs))

    for name in sorted(captures & set(docs)):
        text = docs[name].read_text(encoding="utf-8")
        expected = f"assets/{name}.png"
        if expected not in text:
            errors.append(f"{docs[name].relative_to(ROOT)} does not embed {name}.png")
        if "From Munich with" not in text or "blue-heart.svg" not in text:
            errors.append(f"{docs[name].relative_to(ROOT)} is missing the manual footer")

    # The Step 6.2 bootstrap pages contained the same generated-image NOTE on
    # every screen. Screen Markdown is now editorial source, so that boilerplate
    # must not creep back in. Useful, screen-specific GitHub notes remain valid.
    obsolete_boilerplate = "generated directly from the simulated 256×64 lcd framebuffer"
    for markdown_file in SCREENS.rglob("*.md"):
        if obsolete_boilerplate in markdown_file.read_text(encoding="utf-8").lower():
            errors.append(
                f"obsolete generated-screenshot NOTE boilerplate in {markdown_file.relative_to(ROOT)}"
            )

    if errors:
        print("Manual screenshot validation failed:", file=sys.stderr)
        for error in errors:
            print(f"  - {error}", file=sys.stderr)
        return 1


    print(f"Manual screenshot validation OK: {len(captures)} captures, {len(docs)} screen documents.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
