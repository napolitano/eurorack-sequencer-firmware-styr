#!/usr/bin/env python3
# Purpose: Enforces the minimum release-critical bootloader Unity regression inventory.
# Author: Axel Napolitano — Styr build-system modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Prevent accidental loss of bootloader regression coverage.

This is an inventory/behavior-contract gate, not a statement of source-line
coverage. It complements PlatformIO/Unity execution and the target build gates.
"""

from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
BOOT = ROOT / "test" / "bootloader"

contracts = {
    "test_format": {
        "minimum": 13,
        "required": {
            "test_exact_fit_and_truncation_follow_snprintf_contract",
            "test_unsupported_and_trailing_percent_are_literal",
            "test_boot_printf_writes_through_console_contract",
        },
    },
    "test_update_policy": {
        "minimum": 6,
        "required": {
            "test_oversize_and_size_t_extremes_are_rejected_without_overflow",
            "test_flash_word_rounding_is_exact_for_partial_words",
            "test_every_valid_tail_size_near_partition_end_stays_inside_partition",
        },
    },
    "test_update_file": {
        "minimum": 12,
        "required": {
            "test_minimum_and_maximum_valid_payload_sizes_open_successfully",
            "test_payload_read_accepts_partial_fatfs_reads_until_request_is_complete",
            "test_zero_length_payload_read_is_a_noop",
            "test_error_formatting_remains_bounded_for_tiny_error_buffers",
        },
    },
    "test_flash_layout": {
        "minimum": 6,
        "required": {
            "test_addresses_inside_sectors_are_not_mistaken_for_erase_boundaries",
            "test_application_erase_boundaries_never_select_protected_sectors",
            "test_protected_bootloader_and_hwconfig_sectors_are_outside_application_partition",
        },
    },
    "test_md5": {
        "minimum": 6,
        "required": {
            "test_rfc1321_reference_vectors",
            "test_padding_boundaries_around_single_md5_block",
            "test_data_crossing_bootloader_chunk_boundary_has_known_digest",
            "test_same_payload_digest_is_independent_of_update_chunking",
        },
    },
}

errors = []
total = 0
for suite, contract in contracts.items():
    suite_dir = BOOT / suite
    sources = list(suite_dir.glob("*.cpp"))
    if len(sources) != 1:
        errors.append(f"{suite}: expected exactly one C++ test source, found {len(sources)}")
        continue
    text = sources[0].read_text(encoding="utf-8", errors="replace")
    cases = re.findall(r"\bRUN_TEST\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)", text)
    total += len(cases)
    if len(cases) < contract["minimum"]:
        errors.append(f"{suite}: {len(cases)} cases, minimum is {contract['minimum']}")
    missing = sorted(contract["required"] - set(cases))
    if missing:
        errors.append(f"{suite}: missing critical cases: {', '.join(missing)}")

if total < 43:
    errors.append(f"bootloader: {total} Unity cases, minimum release contract is 43")

if errors:
    print("Bootloader test coverage contract FAILED:")
    for error in errors:
        print(f" - {error}")
    sys.exit(1)

print("Bootloader test coverage contract OK:")
for suite, contract in contracts.items():
    source = next((BOOT / suite).glob("*.cpp"))
    count = len(re.findall(r"\bRUN_TEST\s*\(", source.read_text(encoding="utf-8")))
    print(f" - {suite}: {count} Unity cases")
print(f" - total: {total} Unity cases")
