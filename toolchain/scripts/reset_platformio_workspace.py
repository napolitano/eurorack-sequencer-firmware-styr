# Purpose: Provides Python automation for the repository build tooling.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Remove disposable PlatformIO/editor indexing state.

This is a recovery tool, not part of the normal build. It deliberately leaves
PlatformIO's globally installed packages and Styr's pinned libopencm3 vendor
cache untouched.
"""

from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[2]
PIO = ROOT / ".pio"
COMPILE_DB = ROOT / "compile_commands.json"


def remove_path(path: Path) -> None:
    if path.is_symlink() or path.is_file():
        path.unlink()
    elif path.is_dir():
        shutil.rmtree(path)


for candidate in (PIO, COMPILE_DB):
    if candidate.exists() or candidate.is_symlink():
        remove_path(candidate)
        print(f"removed: {candidate.relative_to(ROOT)}")

print("PlatformIO workspace reset. Next command: pio run -e application")
