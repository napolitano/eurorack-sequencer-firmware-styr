# Purpose: Tests bootloader packaging and CI gate tooling.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Regression tests for bootloader packaging and size-gate tooling."""

from __future__ import annotations

import hashlib
import importlib.util
from pathlib import Path
import tempfile
import unittest
import struct

ROOT = Path(__file__).resolve().parents[2]


def load_module(name: str, relative: str):
    path = ROOT / relative
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    assert spec and spec.loader
    spec.loader.exec_module(module)
    return module


package_module = load_module('package_update', 'toolchain/scripts/package_update.py')
size_module = load_module('check_bootloader_size', 'toolchain/check_bootloader_size.py')
format_module = load_module('check_bootloader_formats', 'toolchain/check_bootloader_formats.py')
abi_module = load_module('check_update_abi', 'toolchain/check_update_abi.py')


class PackageUpdateTests(unittest.TestCase):
    def test_payload_is_preserved_and_raw_md5_is_appended(self):
        payload = bytes(range(256)) * 3
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'firmware.bin'
            output = root / 'nested' / 'UPDATE.DAT'
            source.write_bytes(payload)

            size, digest = package_module.package_update(source, output)

            self.assertEqual(size, len(payload))
            self.assertEqual(digest, hashlib.md5(payload).digest())
            self.assertEqual(output.read_bytes(), payload + digest)
            self.assertEqual(len(output.read_bytes()) - len(payload), 16)

    def test_packaging_is_deterministic(self):
        payload = b'Styr bootloader update contract\x00\xff'
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / 'firmware.bin'
            first = root / 'first.dat'
            second = root / 'second.dat'
            source.write_bytes(payload)
            package_module.package_update(source, first)
            package_module.package_update(source, second)
            self.assertEqual(first.read_bytes(), second.read_bytes())


class BootloaderUpdateAbiGateTests(unittest.TestCase):
    def make_payload(self):
        values = abi_module.contract()
        payload = bytearray(4096)
        name = str(values['name']).encode('ascii') + b'\0'
        name = name.ljust(24, b'\0')
        struct.pack_into(
            '<I24sBBH', payload, int(values['offset']), int(values['magic']), name,
            int(values['major']), int(values['minor']), int(values['revision'])
        )
        return values, bytes(payload)

    def test_application_and_update_package_match_contract(self):
        values, payload = self.make_payload()
        abi_module.verify_application(payload, values, 'synthetic application')
        update = payload + hashlib.md5(payload).digest()
        self.assertEqual(abi_module.verify_update(update, values, 'synthetic update'), payload)

    def test_corrupt_update_digest_is_rejected(self):
        values, payload = self.make_payload()
        update = payload + bytes(16)
        with self.assertRaises(RuntimeError):
            abi_module.verify_update(update, values, 'corrupt update')

    def test_wrong_version_magic_is_rejected(self):
        values, payload = self.make_payload()
        broken = bytearray(payload)
        struct.pack_into('<I', broken, int(values['offset']), int(values['magic']) ^ 1)
        with self.assertRaises(RuntimeError):
            abi_module.verify_application(bytes(broken), values, 'wrong magic')

    def test_non_terminated_version_name_is_rejected(self):
        values, payload = self.make_payload()
        broken = bytearray(payload)
        start = int(values['offset']) + 4
        broken[start:start + 24] = b'X' * 24
        with self.assertRaises(RuntimeError):
            abi_module.verify_application(bytes(broken), values, 'unterminated name')


class BootloaderFormatGateTests(unittest.TestCase):
    def test_supported_subset_is_accepted(self):
        for value in ('%s', '%d', '%u', '%x', '%%', '%02x', '%08lx', '%lu', 'progress %d%%', 'trailing%'):
            self.assertEqual(format_module.validate_format(value), [], value)

    def test_unsupported_or_incomplete_formats_are_rejected(self):
        for value in ('%p', '%f', '%c', '%zu', '%lld', '%0', '%08', '%l', '%ls'):
            self.assertNotEqual(format_module.validate_format(value), [], value)


class BootloaderSizeGateTests(unittest.TestCase):
    def test_binary_at_partition_limit_passes(self):
        with tempfile.TemporaryDirectory() as directory:
            binary = Path(directory) / 'bootloader.bin'
            binary.write_bytes(b'\0' * 32768)
            self.assertEqual(size_module.check(binary, 32768, 1024), 0)

    def test_binary_over_partition_limit_fails(self):
        with tempfile.TemporaryDirectory() as directory:
            binary = Path(directory) / 'bootloader.bin'
            binary.write_bytes(b'\0' * 32769)
            self.assertEqual(size_module.check(binary, 32768, 1024), 1)

    def test_missing_binary_is_a_distinct_failure(self):
        with tempfile.TemporaryDirectory() as directory:
            binary = Path(directory) / 'missing.bin'
            self.assertEqual(size_module.check(binary, 32768, 1024), 2)


if __name__ == '__main__':
    unittest.main()
