#!/usr/bin/env python3
# Purpose: Enforces the bootloader/application UPDATE.DAT ABI contract.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Validate Styr application/version-tag/update-package compatibility."""

from __future__ import annotations

from argparse import ArgumentParser
from pathlib import Path
import hashlib
import re
import struct
import sys

ROOT = Path(__file__).resolve().parents[1]
BOOT_CONFIG = ROOT / 'src' / 'bootloader' / 'Config.h'
APP_CONFIG = ROOT / 'src' / 'sequencer' / 'Config.h'
VERSION_TAG_SIZE = 32
CHECKSUM_SIZE = 16


def define(path: Path, name: str) -> str:
    text = path.read_text(encoding='utf-8')
    match = re.search(rf'^\s*#define\s+{re.escape(name)}\s+(.+?)\s*$', text, re.M)
    if not match:
        raise RuntimeError(f'{name} not found in {path.relative_to(ROOT)}')
    return match.group(1).split('//', 1)[0].strip()


def number(path: Path, name: str) -> int:
    return int(define(path, name), 0)


def string(path: Path, name: str) -> str:
    raw = define(path, name)
    if len(raw) < 2 or raw[0] != '"' or raw[-1] != '"':
        raise RuntimeError(f'{name} is not a simple string literal in {path.relative_to(ROOT)}')
    return bytes(raw[1:-1], 'utf-8').decode('unicode_escape')


def contract() -> dict[str, int | str]:
    values: dict[str, int | str] = {
        'magic': number(BOOT_CONFIG, 'CONFIG_VERSION_TAG_MAGIC'),
        'offset': number(BOOT_CONFIG, 'CONFIG_VERSION_TAG_OFFSET'),
        'application_size': number(BOOT_CONFIG, 'CONFIG_APPLICATION_SIZE'),
        'app_magic': number(APP_CONFIG, 'CONFIG_VERSION_MAGIC'),
        'name': string(APP_CONFIG, 'CONFIG_VERSION_NAME'),
        'major': number(APP_CONFIG, 'CONFIG_VERSION_MAJOR'),
        'minor': number(APP_CONFIG, 'CONFIG_VERSION_MINOR'),
        'revision': number(APP_CONFIG, 'CONFIG_VERSION_REVISION'),
    }
    if values['magic'] != values['app_magic']:
        raise RuntimeError(
            f"version magic mismatch: bootloader 0x{int(values['magic']):08x}, "
            f"application 0x{int(values['app_magic']):08x}"
        )
    name_bytes = str(values['name']).encode('ascii')
    if len(name_bytes) >= 24:
        raise RuntimeError('CONFIG_VERSION_NAME must fit in 23 bytes plus NUL in VersionTag.name[24]')
    if int(values['offset']) != 0x400:
        raise RuntimeError(f"version-tag offset changed from fixed ABI value 0x400 to 0x{int(values['offset']):x}")
    return values


def verify_application(payload: bytes, values: dict[str, int | str], label: str) -> None:
    offset = int(values['offset'])
    if len(payload) > int(values['application_size']):
        raise RuntimeError(f'{label}: payload exceeds application partition')
    if len(payload) < offset + VERSION_TAG_SIZE:
        raise RuntimeError(f'{label}: payload does not contain the complete version tag')

    magic, raw_name, major, minor, revision = struct.unpack_from('<I24sBBH', payload, offset)
    name = raw_name.split(b'\0', 1)[0]
    if b'\0' not in raw_name:
        raise RuntimeError(f'{label}: VersionTag.name is not NUL-terminated')
    expected_name = str(values['name']).encode('ascii')
    expected = (
        int(values['magic']), expected_name, int(values['major']),
        int(values['minor']), int(values['revision'])
    )
    actual = (magic, name, major, minor, revision)
    if actual != expected:
        raise RuntimeError(
            f'{label}: version tag mismatch: got magic=0x{magic:08x} name={name!r} '
            f'version={major}.{minor}.{revision}, expected magic=0x{expected[0]:08x} '
            f'name={expected_name!r} version={expected[2]}.{expected[3]}.{expected[4]}'
        )


def verify_update(data: bytes, values: dict[str, int | str], label: str) -> bytes:
    if len(data) < CHECKSUM_SIZE:
        raise RuntimeError(f'{label}: file is shorter than the MD5 trailer')
    payload, digest = data[:-CHECKSUM_SIZE], data[-CHECKSUM_SIZE:]
    expected_digest = hashlib.md5(payload).digest()
    if digest != expected_digest:
        raise RuntimeError(f'{label}: MD5 trailer does not match payload')
    verify_application(payload, values, label + ' payload')
    return payload


def main() -> int:
    parser = ArgumentParser(description=__doc__)
    parser.add_argument('--application', type=Path)
    parser.add_argument('--update', type=Path)
    args = parser.parse_args()

    try:
        values = contract()
        application_payload = None
        update_payload = None
        if args.application:
            application_payload = args.application.read_bytes()
            verify_application(application_payload, values, str(args.application))
        if args.update:
            update_payload = verify_update(args.update.read_bytes(), values, str(args.update))
        if application_payload is not None and update_payload is not None and application_payload != update_payload:
            raise RuntimeError('UPDATE.DAT payload is not byte-identical to the supplied application binary')
    except (OSError, RuntimeError, ValueError) as exc:
        print(f'Bootloader update ABI gate FAILED: {exc}', file=sys.stderr)
        return 1

    print('Bootloader update ABI contract OK')
    print(f"  version tag: offset 0x{int(values['offset']):x}, magic 0x{int(values['magic']):08x}, "
          f"{values['name']} {values['major']}.{values['minor']}.{values['revision']}")
    if args.application:
        print(f'  application: {args.application} ({args.application.stat().st_size} bytes)')
    if args.update:
        print(f'  update     : {args.update} ({args.update.stat().st_size} bytes)')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
