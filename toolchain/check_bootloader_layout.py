#!/usr/bin/env python3
# Purpose: Enforces the fixed bootloader/application flash layout contract.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Validate the fixed bootloader/application flash layout across all build definitions."""

from __future__ import annotations

from configparser import ConfigParser
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]


def fail(message: str) -> None:
    raise RuntimeError(message)


def parse_c_macro(path: Path, name: str) -> int:
    text = path.read_text(encoding='utf-8')
    match = re.search(rf'^\s*#define\s+{re.escape(name)}\s+([^\s/]+)', text, re.M)
    if not match:
        fail(f'{name} not found in {path.relative_to(ROOT)}')
    return int(match.group(1), 0)


def parse_linker_rom(path: Path) -> tuple[int, int]:
    text = path.read_text(encoding='utf-8')
    match = re.search(r'ROM\s*\([^)]*\)\s*:\s*ORIGIN\s*=\s*(0x[0-9A-Fa-f]+)\s*,\s*LENGTH\s*=\s*([0-9]+)([KkMm]?)', text)
    if not match:
        fail(f'ROM memory declaration not found in {path.relative_to(ROOT)}')
    origin = int(match.group(1), 0)
    length = int(match.group(2), 10)
    suffix = match.group(3).upper()
    if suffix == 'K':
        length *= 1024
    elif suffix == 'M':
        length *= 1024 * 1024
    return origin, length


def parse_int(value: str) -> int:
    return int(value.strip(), 0)


def main() -> int:
    config_h = ROOT / 'src' / 'bootloader' / 'Config.h'
    app_addr = parse_c_macro(config_h, 'CONFIG_APPLICATION_ADDR')
    app_size = parse_c_macro(config_h, 'CONFIG_APPLICATION_SIZE')
    tag_offset = parse_c_macro(config_h, 'CONFIG_VERSION_TAG_OFFSET')
    tag_magic = parse_c_macro(config_h, 'CONFIG_VERSION_TAG_MAGIC')

    parser = ConfigParser(interpolation=None, strict=False)
    parser.read(ROOT / 'platformio.ini', encoding='utf-8')

    boot_pio_origin = parse_int(parser['env:bootloader']['board_upload.offset_address'])
    boot_pio_size = parse_int(parser['env:bootloader']['board_upload.maximum_size'])
    app_pio_origin = parse_int(parser['env:application']['board_upload.offset_address'])
    app_pio_size = parse_int(parser['env:application']['board_upload.maximum_size'])
    hw_pio_origin = parse_int(parser['hwconfig']['board_upload.offset_address'])
    hw_pio_size = parse_int(parser['hwconfig']['board_upload.maximum_size'])

    boot_ld_origin, boot_ld_size = parse_linker_rom(ROOT / 'toolchain' / 'linker' / 'bootloader.ld')
    app_ld_origin, app_ld_size = parse_linker_rom(ROOT / 'toolchain' / 'linker' / 'application.ld')
    hw_ld_origin, hw_ld_size = parse_linker_rom(ROOT / 'toolchain' / 'linker' / 'hwconfig.ld')

    expected_boot_origin = 0x08000000
    expected_boot_size = 32 * 1024
    expected_hw_origin = 0x08008000
    expected_hw_size = 16 * 1024
    expected_app_origin = 0x08010000
    expected_app_size = 960 * 1024
    expected_tag_offset = 0x400
    expected_tag_magic = 0xFADEBABE

    checks = {
        'bootloader PlatformIO origin': (boot_pio_origin, expected_boot_origin),
        'bootloader PlatformIO size': (boot_pio_size, expected_boot_size),
        'bootloader linker origin': (boot_ld_origin, expected_boot_origin),
        'bootloader linker size': (boot_ld_size, expected_boot_size),
        'application fixed origin': (app_addr, expected_app_origin),
        'application fixed size': (app_size, expected_app_size),
        'application version-tag offset': (tag_offset, expected_tag_offset),
        'application version-tag magic': (tag_magic, expected_tag_magic),
        'application Config origin vs PlatformIO': (app_addr, app_pio_origin),
        'application Config size vs PlatformIO': (app_size, app_pio_size),
        'application Config origin vs linker': (app_addr, app_ld_origin),
        'application Config size vs linker': (app_size, app_ld_size),
        'hwconfig PlatformIO origin': (hw_pio_origin, expected_hw_origin),
        'hwconfig PlatformIO size': (hw_pio_size, expected_hw_size),
        'hwconfig linker origin': (hw_ld_origin, expected_hw_origin),
        'hwconfig linker size': (hw_ld_size, expected_hw_size),
    }

    errors = [f'{name}: 0x{actual:x} != 0x{expected:x}' for name, (actual, expected) in checks.items() if actual != expected]

    if boot_ld_origin + boot_ld_size > hw_ld_origin:
        errors.append('bootloader partition overlaps hwconfig partition')
    if hw_ld_origin + hw_ld_size > app_addr:
        errors.append('hwconfig partition overlaps application partition')
    if tag_offset + 32 > app_size:
        errors.append('VersionTag extent lies outside application partition')

    if errors:
        print('Bootloader memory-layout contract FAILED:', file=sys.stderr)
        for error in errors:
            print(f'  {error}', file=sys.stderr)
        return 1

    print('Bootloader memory-layout contract OK')
    print(f'  bootloader : 0x{boot_ld_origin:08x} .. 0x{boot_ld_origin + boot_ld_size - 1:08x} ({boot_ld_size} bytes)')
    print(f'  hwconfig   : 0x{hw_ld_origin:08x} .. 0x{hw_ld_origin + hw_ld_size - 1:08x} ({hw_ld_size} bytes)')
    print(f'  application: 0x{app_addr:08x} .. 0x{app_addr + app_size - 1:08x} ({app_size} bytes)')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
