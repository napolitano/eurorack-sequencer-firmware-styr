#!/usr/bin/env python3
# Purpose: Validates generated Styr manual ODT/PDF publication artifacts.
# Author: Axel Napolitano — Styr documentation toolchain
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Validate naming, metadata, page geometry and font embedding of manual artifacts."""

from __future__ import annotations

import argparse
from pathlib import Path
import re
import shutil
import subprocess
import sys
import xml.etree.ElementTree as ET
from zipfile import ZipFile

NS = {
    "office": "urn:oasis:names:tc:opendocument:xmlns:office:1.0",
    "dc": "http://purl.org/dc/elements/1.1/",
    "meta": "urn:oasis:names:tc:opendocument:xmlns:meta:1.0",
}


def q(prefix: str, local: str) -> str:
    return f"{{{NS[prefix]}}}{local}"


def expected_stem(language_code: str, version: str) -> str:
    return f"styr-user-manual.{language_code}.{version}"


def check_odt(path: Path, *, version: str, locale: str, language_code: str) -> list[str]:
    errors: list[str] = []
    if path.name != f"{expected_stem(language_code, version)}.odt":
        errors.append(f"unexpected ODT filename: {path.name}")
        return errors
    try:
        with ZipFile(path) as archive:
            names = set(archive.namelist())
            embedded_fonts = [
                name for name in names
                if Path(name).suffix.lower() in {".ttf", ".otf", ".woff", ".woff2", ".eot"}
            ]
            if embedded_fonts:
                errors.append("generated ODT unexpectedly embeds font binaries: " + ", ".join(embedded_fonts))
            root = ET.fromstring(archive.read("meta.xml"))
            office_meta = root.find(q("office", "meta"))
            if office_meta is None:
                errors.append("ODT meta.xml has no office:meta element")
            else:
                def text(tag: str) -> str:
                    element = office_meta.find(tag)
                    return (element.text or "") if element is not None else ""

                if text(q("dc", "language")) != locale:
                    errors.append(f"ODT locale metadata mismatch: {text(q('dc', 'language'))!r}")
                expected_id = expected_stem(language_code, version)
                if text(q("dc", "identifier")) != expected_id:
                    errors.append(f"ODT identifier mismatch: {text(q('dc', 'identifier'))!r}")
                user = {
                    element.get(q("meta", "name")): (element.text or "")
                    for element in office_meta.findall(q("meta", "user-defined"))
                }
                if user.get("StyrManualVersion") != version:
                    errors.append("ODT StyrManualVersion metadata mismatch")
                if user.get("StyrManualLocale") != locale:
                    errors.append("ODT StyrManualLocale metadata mismatch")
                if user.get("StyrManualLanguageCode") != language_code:
                    errors.append("ODT StyrManualLanguageCode metadata mismatch")
    except (OSError, KeyError, ET.ParseError) as error:
        errors.append(f"could not inspect ODT: {error}")
    return errors


def parse_pdfinfo(pdfinfo: str, path: Path) -> dict[str, str]:
    result = subprocess.run(
        [pdfinfo, str(path)], check=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE
    ).stdout
    values: dict[str, str] = {}
    for line in result.splitlines():
        if ":" in line:
            key, value = line.split(":", 1)
            values[key.strip()] = value.strip()
    return values


def check_pdf(path: Path, *, version: str, language_code: str, require_ubuntu: bool) -> list[str]:
    errors: list[str] = []
    if path.name != f"{expected_stem(language_code, version)}.pdf":
        errors.append(f"unexpected PDF filename: {path.name}")
        return errors

    pdfinfo = shutil.which("pdfinfo")
    pdffonts = shutil.which("pdffonts")
    if not pdfinfo:
        errors.append("pdfinfo is required for PDF artifact validation")
        return errors
    if not pdffonts:
        errors.append("pdffonts is required for PDF artifact validation")
        return errors

    try:
        info = parse_pdfinfo(pdfinfo, path)
        pages = int(info.get("Pages", "0"))
        if pages < 2:
            errors.append(f"PDF has implausible page count: {pages}")
        size = info.get("Page size", "")
        match = re.match(r"([0-9.]+) x ([0-9.]+) pts", size)
        if not match:
            errors.append(f"could not parse PDF page size: {size!r}")
        else:
            width, height = map(float, match.groups())
            # 99.01 x 210.01 mm in PostScript points, with small exporter tolerance.
            if abs(width - 280.66) > 0.8 or abs(height - 595.30) > 0.8:
                errors.append(f"PDF page size does not match the Styr manual format: {size}")

        font_output = subprocess.run(
            [pdffonts, str(path)], check=True, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE
        ).stdout
        font_rows = [line for line in font_output.splitlines()[2:] if line.strip()]
        if not font_rows:
            errors.append("PDF contains no detectable fonts")
        for row in font_rows:
            columns = row.split()
            # pdffonts columns: name type encoding emb sub uni object ID
            if len(columns) >= 5 and columns[3].lower() != "yes":
                errors.append(f"PDF contains a non-embedded font: {row}")
        if require_ubuntu and not any("ubuntu" in row.lower() for row in font_rows):
            errors.append("PDF does not contain the required Ubuntu font family")
    except (OSError, ValueError, subprocess.CalledProcessError) as error:
        errors.append(f"could not inspect PDF: {error}")
    return errors


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--odt", type=Path, required=True)
    parser.add_argument("--pdf", type=Path, required=True)
    parser.add_argument("--version", required=True)
    parser.add_argument("--locale", required=True)
    parser.add_argument("--language-code", required=True)
    parser.add_argument(
        "--allow-substituted-fonts",
        action="store_true",
        help="Allow a smoke/debug PDF without Ubuntu; release validation must not use this flag",
    )
    args = parser.parse_args()

    errors = []
    errors.extend(check_odt(args.odt, version=args.version, locale=args.locale, language_code=args.language_code))
    errors.extend(
        check_pdf(
            args.pdf,
            version=args.version,
            language_code=args.language_code,
            require_ubuntu=not args.allow_substituted_fonts,
        )
    )
    if errors:
        print("Manual artifact validation FAILED:")
        for error in errors:
            print(f" - {error}")
        return 1

    print("Manual artifact validation OK:")
    print(f" - {args.odt.name}")
    print(f" - {args.pdf.name}")
    print(" - ODT metadata and release naming are consistent")
    print(" - PDF page geometry and font embedding are valid")
    if not args.allow_substituted_fonts:
        print(" - PDF uses the required Ubuntu font family")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
