#!/usr/bin/env python3
# Purpose: Builds all enabled Styr manual locales with the common publication pipeline.
# Author: Axel Napolitano — Styr documentation toolchain
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Build one ODT/PDF pair for every enabled manual locale."""

from __future__ import annotations

import argparse
from pathlib import Path
import subprocess
import sys
import tomllib

ROOT = Path(__file__).resolve().parents[2]
CONFIG = ROOT / "docs" / "manual" / "manual.toml"
BUILDER = ROOT / "toolchain" / "manual" / "build_manual.py"


def enabled_locales() -> list[str]:
    with CONFIG.open("rb") as handle:
        config = tomllib.load(handle)
    return [locale for locale, data in config["locales"].items() if data.get("enabled", False)]


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--odt-only", action="store_true")
    args = parser.parse_args()

    locales = enabled_locales()
    if not locales:
        print("Manual build FAILED: no enabled locales", file=sys.stderr)
        return 1

    for locale in locales:
        command = [
            sys.executable,
            str(BUILDER),
            "--version", args.version,
            "--locale", locale,
            "--output-dir", str(args.output_dir),
        ]
        if args.odt_only:
            command.append("--odt-only")
        print(f"Building Styr manual locale {locale}")
        result = subprocess.run(command, cwd=ROOT)
        if result.returncode:
            return result.returncode
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
