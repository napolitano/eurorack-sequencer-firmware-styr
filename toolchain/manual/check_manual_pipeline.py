#!/usr/bin/env python3
# Purpose: Validates Styr manual publication configuration and ODT reference safety.
# Author: Axel Napolitano — Styr documentation toolchain
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Validate the deterministic Markdown -> ODT -> PDF publication contract."""

from __future__ import annotations

from pathlib import Path
import re
import sys
import tomllib
import xml.etree.ElementTree as ET
from zipfile import ZipFile

ROOT = Path(__file__).resolve().parents[2]
CONFIG = ROOT / "docs" / "manual" / "manual.toml"

NS = {
    "style": "urn:oasis:names:tc:opendocument:xmlns:style:1.0",
    "fo": "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0",
}


def main() -> int:
    errors: list[str] = []
    with CONFIG.open("rb") as handle:
        config = tomllib.load(handle)
    manual = config.get("manual", {})
    locales = config.get("locales", {})

    source_locale = manual.get("source_locale")
    if source_locale not in locales or not locales[source_locale].get("enabled", False):
        errors.append("source locale must exist and be enabled")

    codes: set[str] = set()
    for locale, record in locales.items():
        if not record.get("enabled", False):
            continue
        code = record.get("language_code", "")
        if not re.fullmatch(r"[a-z]{2}", code):
            errors.append(f"{locale}: language_code must be lower-case ISO 639-1")
        elif code in codes:
            errors.append(f"duplicate release language_code: {code}")
        codes.add(code)
        chapters = record.get("chapters")
        if not isinstance(chapters, list) or not chapters:
            errors.append(f"{locale}: enabled locale must define an ordered, non-empty chapters list")
        else:
            for chapter in chapters:
                path = ROOT / chapter
                if not path.is_file():
                    errors.append(f"{locale}: manual chapter does not exist: {chapter}")

    reference = ROOT / manual.get("reference_document", "")
    theme = ROOT / manual.get("theme_document", "")
    if not reference.is_file():
        errors.append(f"reference ODT missing: {reference.relative_to(ROOT) if reference.is_absolute() else reference}")
    if not theme.is_file():
        errors.append("manual theme TOML missing")
    else:
        with theme.open("rb") as handle:
            theme_config = tomllib.load(handle)
        if theme_config.get("colors", {}).get("accent", "").lower() != "#000000":
            errors.append("current Styr manual accent must be configurable black (#000000)")
        fonts = theme_config.get("fonts", {})
        if fonts.get("body") != "Ubuntu Light" or fonts.get("heading") != "Ubuntu":
            errors.append("manual theme must retain Ubuntu Light body and Ubuntu heading typography")

    style_dir = ROOT / "docs" / "manual" / "style"
    direct_fonts = [
        path.relative_to(ROOT).as_posix()
        for path in style_dir.rglob("*")
        if path.is_file() and path.suffix.lower() in {".ttf", ".otf", ".woff", ".woff2", ".eot"}
    ]
    if direct_fonts:
        errors.append("manual style directory must not redistribute font binaries: " + ", ".join(direct_fonts))

    if reference.is_file():
        try:
            with ZipFile(reference) as archive:
                names = archive.namelist()
                if "mimetype" not in names or archive.read("mimetype") != b"application/vnd.oasis.opendocument.text":
                    errors.append("reference document is not a valid ODT")
                embedded = [name for name in names if Path(name).suffix.lower() in {".ttf", ".otf", ".woff", ".woff2", ".eot"}]
                if embedded:
                    errors.append("reference ODT must not embed font binaries: " + ", ".join(embedded))
                styles = archive.read("styles.xml").decode("utf-8", errors="replace")
                if "Ubuntu Light" not in styles or "Ubuntu" not in styles:
                    errors.append("reference ODT must explicitly reference Ubuntu and Ubuntu Light")
                if "#0563c1" in styles.lower():
                    errors.append("reference ODT still contains the inherited blue accent #0563c1")
                if "Pictures/10000001000003F20000086072BB7A0C.png" not in names:
                    errors.append("reference ODT is missing the supplied Styr cover artwork")
                root = ET.fromstring(archive.read("styles.xml"))
                heading1 = None
                for style in root.iter(f"{{{NS['style']}}}style"):
                    if style.get(f"{{{NS['style']}}}name") == "Heading_20_1":
                        heading1 = style
                        break
                if heading1 is None:
                    errors.append("reference ODT is missing Heading 1")
                else:
                    text_props = heading1.find(f"{{{NS['style']}}}text-properties")
                    para_props = heading1.find(f"{{{NS['style']}}}paragraph-properties")
                    if text_props is None or text_props.get(f"{{{NS['fo']}}}font-size") != "13pt":
                        errors.append("Heading 1 must match the supplied 13 pt chapter treatment")
                    if para_props is not None and para_props.get(f"{{{NS['fo']}}}break-before") == "page":
                        errors.append("Heading 1 must not combine master-page switching with break-before")
                    if heading1.get(f"{{{NS['style']}}}master-page-name") != "Converted1":
                        errors.append("Heading 1 must switch from the static cover to the content master page")
                widths = []
                for props in root.iter(f"{{{NS['style']}}}page-layout-properties"):
                    widths.append((props.get(f"{{{NS['fo']}}}page-width"), props.get(f"{{{NS['fo']}}}page-height")))
                if ("9.901cm", "21.001cm") not in widths:
                    errors.append("reference ODT does not preserve the 99.01 x 210.01 mm page size")
        except Exception as error:  # report malformed archive/XML as validation failure
            errors.append(f"could not inspect reference ODT: {error}")

    if errors:
        print("Manual publication pipeline validation FAILED:")
        for error in errors:
            print(f" - {error}")
        return 1

    print("Manual publication pipeline validation OK:")
    print(f" - source locale: {source_locale}")
    print(" - release language codes are unique ISO 639-1 values")
    print(" - reference ODT preserves Styr page/layout assets")
    print(" - reference ODT references Ubuntu/Ubuntu Light without embedding font binaries")
    print(" - inherited blue accent is absent from the reference styles")
    print(" - chapter heading/master-page transition matches the supplied Styr template")
    print(" - configurable accent is black and no manual font binaries are stored in the repository")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
