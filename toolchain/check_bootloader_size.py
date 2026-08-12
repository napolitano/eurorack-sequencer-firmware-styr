#!/usr/bin/env python3
# Purpose: Enforces the fixed 32 KiB bootloader binary-size limit.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Hard gate for the fixed 32 KiB Styr bootloader binary."""

from __future__ import annotations

from argparse import ArgumentParser
from pathlib import Path
import sys

DEFAULT_LIMIT = 32 * 1024
DEFAULT_WARNING_HEADROOM = 1024


def check(path: Path, limit: int, warning_headroom: int) -> int:
    if not path.is_file():
        print(f'Bootloader size gate FAILED: binary not found: {path}', file=sys.stderr)
        return 2

    used = path.stat().st_size
    headroom = limit - used
    percent = used * 100.0 / limit

    print(f'Bootloader flash image: {used} / {limit} bytes ({percent:.2f}%)')
    if headroom < 0:
        print(f'Bootloader size gate FAILED: exceeds partition by {-headroom} bytes', file=sys.stderr)
        return 1

    print(f'Bootloader headroom: {headroom} bytes')
    if headroom < warning_headroom:
        print(f'WARNING: bootloader has less than {warning_headroom} bytes of flash headroom', file=sys.stderr)
    print('Bootloader size gate OK')
    return 0


def main() -> int:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('binary', type=Path, nargs='?', default=Path('.pio/build/bootloader/bootloader.bin'))
    parser.add_argument('--limit', type=int, default=DEFAULT_LIMIT)
    parser.add_argument('--warning-headroom', type=int, default=DEFAULT_WARNING_HEADROOM)
    args = parser.parse_args()
    return check(args.binary, args.limit, args.warning_headroom)


if __name__ == '__main__':
    raise SystemExit(main())
