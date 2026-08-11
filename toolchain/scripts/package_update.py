# Purpose: Provides Python automation for the repository build tooling.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
#!/usr/bin/env python3
"""Create the bootloader-compatible SD-card update image."""

from pathlib import Path
import argparse
import binascii
import hashlib


def package_update(infile, outfile):
    infile = Path(infile)
    outfile = Path(outfile)
    payload = infile.read_bytes()
    digest = hashlib.md5(payload).digest()
    outfile.parent.mkdir(parents=True, exist_ok=True)
    outfile.write_bytes(payload + digest)
    return len(payload), digest


def main():
    parser = argparse.ArgumentParser(description="Append the raw MD5 digest expected by the Styr bootloader")
    parser.add_argument("infile")
    parser.add_argument("outfile")
    args = parser.parse_args()

    size, digest = package_update(args.infile, args.outfile)
    print(f"package update {args.infile} -> {args.outfile}")
    print(f"size: {size} bytes")
    print(f"md5sum: {binascii.hexlify(digest).decode('utf-8')}")


if __name__ == "__main__":
    main()
