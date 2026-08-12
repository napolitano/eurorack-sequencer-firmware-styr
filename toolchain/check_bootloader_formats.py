#!/usr/bin/env python3
# Purpose: Enforces the compact bootloader formatter contract.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Reject printf-style conversions unsupported by Styr's compact bootloader formatter."""

from __future__ import annotations

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
BOOTLOADER = ROOT / "src" / "bootloader"

STRING_RE = re.compile(r'"(?:\\.|[^"\\])*"')
BLOCK_COMMENT_RE = re.compile(r'/\*.*?\*/', re.S)
LINE_COMMENT_RE = re.compile(r'//.*')


def strip_comments(text: str) -> str:
    text = BLOCK_COMMENT_RE.sub('', text)
    return '\n'.join(LINE_COMMENT_RE.sub('', line) for line in text.splitlines())


def validate_format(value: str) -> list[str]:
    errors: list[str] = []
    i = 0
    while i < len(value):
        if value[i] != '%':
            i += 1
            continue

        start = i
        i += 1
        if i == len(value):
            # The formatter deliberately preserves a bare trailing '%' as a
            # literal character. No flags/width/length may precede EOF.
            break
        if value[i] == '%':
            i += 1
            continue

        if value[i] == '0':
            i += 1
        while i < len(value) and value[i].isdigit():
            i += 1
        is_long = i < len(value) and value[i] == 'l'
        if is_long:
            i += 1
        if i >= len(value):
            errors.append(value[start:])
            break

        conv = value[i]
        i += 1
        if conv not in 'sdux':
            errors.append(value[start:i])
            continue
        if is_long and conv == 's':
            errors.append(value[start:i])
    return errors


def main() -> int:
    failures: list[str] = []
    checked = 0

    build_glue = (ROOT / "toolchain" / "scripts" / "platformio" / "styr_build.py").read_text(encoding="utf-8")
    if '"+<lib/stb_sprintf.c>"' in build_glue or "'+<lib/stb_sprintf.c>'" in build_glue:
        failures.append("toolchain/scripts/platformio/styr_build.py: generic stb_sprintf re-entered the bootloader build graph")

    for path in sorted(BOOTLOADER.glob('*')):
        if path.suffix not in {'.cpp', '.h'}:
            continue
        text = strip_comments(path.read_text(encoding='utf-8'))
        if re.search(r'(?<![A-Za-z0-9_])(?:printf|snprintf)\s*\(', text):
            failures.append(f'{path.relative_to(ROOT)}: generic printf/snprintf call would bypass the compact formatter')
        if 'stb_sprintf' in text:
            failures.append(f'{path.relative_to(ROOT)}: direct stb_sprintf dependency is forbidden in bootloader source')
        for line_number, line in enumerate(text.splitlines(), start=1):
            # Inline assembly uses GCC operand syntax such as "%0"; it is not
            # a formatter string and must not participate in this contract.
            if 'asm ' in line or 'asm(' in line:
                continue
            for match in STRING_RE.finditer(line):
                literal = match.group(0)[1:-1]
                if '%' not in literal:
                    continue
                unsupported = validate_format(literal)
                checked += 1
                for token in unsupported:
                    failures.append(f'{path.relative_to(ROOT)}:{line_number}: unsupported {token!r} in {match.group(0)}')

    if failures:
        print('Bootloader format contract FAILED:', file=sys.stderr)
        for failure in failures:
            print(f'  {failure}', file=sys.stderr)
        return 1

    print(f'Bootloader format contract OK: {checked} format-bearing literals use the supported subset')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
