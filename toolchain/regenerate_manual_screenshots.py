#!/usr/bin/env python3
# Purpose: Runs the canonical complete Styr manual-screenshot regeneration and strict asset validation.
# Author: Axel Napolitano — Styr documentation tooling
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Regenerate the complete deterministic manual screenshot corpus."""

from __future__ import annotations

import argparse
from pathlib import Path
import platform
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
SIMULATOR = ROOT / "src" / "simulator"


def default_preset(system: str | None = None) -> str:
    system = system or platform.system()
    return "windows-ucrt64-manual-screenshots" if system == "Windows" else "manual-screenshots"


def run(command: list[str], cwd: Path) -> None:
    print("+", " ".join(command), f"(cwd={cwd})")
    subprocess.run(command, cwd=cwd, check=True)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--preset",
        default=default_preset(),
        help="CMake preset to use (default: platform-appropriate repository preset)",
    )
    parser.add_argument(
        "--cmake",
        default=shutil.which("cmake") or shutil.which("cmake.exe") or "cmake",
        help="CMake executable",
    )
    args = parser.parse_args()

    try:
        run([args.cmake, "--preset", args.preset], SIMULATOR)
        run(
            [args.cmake, "--build", "--preset", args.preset, "--target", "manual-screenshots"],
            SIMULATOR,
        )
        run(
            [sys.executable, "toolchain/check_manual_screenshots.py", "--require-assets"],
            ROOT,
        )
    except (OSError, subprocess.CalledProcessError) as exc:
        print(f"Manual screenshot regeneration FAILED: {exc}", file=sys.stderr)
        return 1

    print("Manual screenshot regeneration OK: complete corpus generated and asset-validated.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
