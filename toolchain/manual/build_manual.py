#!/usr/bin/env python3
# Purpose: Builds Styr user-manual ODT/PDF artifacts from maintained Markdown.
# Author: Axel Napolitano — Styr documentation toolchain
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Build deterministic-layout Styr manual artifacts from Markdown.

The publication path is deliberately two-stage:

    Markdown -> Pandoc ODT -> Styr ODT finalizer -> LibreOffice PDF

The ODT reference document carries layout/style information but no embedded
font binaries. Strict PDF builds require Ubuntu/Ubuntu Light to be available in
the build environment and fail rather than silently accepting substitution.
"""

from __future__ import annotations

import argparse
import copy
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile
import tomllib
import xml.etree.ElementTree as ET
from zipfile import ZIP_DEFLATED, ZIP_STORED, ZipFile, ZipInfo

ROOT = Path(__file__).resolve().parents[2]
CONFIG_PATH = ROOT / "docs" / "manual" / "manual.toml"
SMOKE_SOURCE = ROOT / "toolchain" / "manual" / "smoke" / "manual.md"

NS = {
    "office": "urn:oasis:names:tc:opendocument:xmlns:office:1.0",
    "style": "urn:oasis:names:tc:opendocument:xmlns:style:1.0",
    "text": "urn:oasis:names:tc:opendocument:xmlns:text:1.0",
    "table": "urn:oasis:names:tc:opendocument:xmlns:table:1.0",
    "draw": "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0",
    "fo": "urn:oasis:names:tc:opendocument:xmlns:xsl-fo-compatible:1.0",
    "svg": "urn:oasis:names:tc:opendocument:xmlns:svg-compatible:1.0",
    "xlink": "http://www.w3.org/1999/xlink",
    "dc": "http://purl.org/dc/elements/1.1/",
    "meta": "urn:oasis:names:tc:opendocument:xmlns:meta:1.0",
    "loext": "urn:org:documentfoundation:names:experimental:office:xmlns:loext:1.0",
}
for prefix, uri in NS.items():
    ET.register_namespace(prefix, uri)

FIXED_ZIP_TIMESTAMP = (1980, 1, 1, 0, 0, 0)


def q(prefix: str, local: str) -> str:
    return f"{{{NS[prefix]}}}{local}"


def load_toml(path: Path) -> dict:
    with path.open("rb") as handle:
        return tomllib.load(handle)


def normalize_version(raw: str) -> str:
    value = raw.strip()
    if value.startswith("v"):
        value = value[1:]
    if not re.fullmatch(r"\d+\.\d+\.\d+(?:[-+][0-9A-Za-z.-]+)?", value):
        raise ValueError(f"unsupported manual version/tag: {raw!r}")
    return value


def resolve_tool(names: tuple[str, ...], *, required: bool = True) -> str | None:
    for name in names:
        path = shutil.which(name)
        if path:
            return path
    if required:
        raise RuntimeError(f"required tool not found: {' or '.join(names)}")
    return None


def ensure_outside_repo(path: Path) -> None:
    try:
        path.resolve().relative_to(ROOT.resolve())
    except ValueError:
        return
    raise RuntimeError(f"generated manual output must not live inside the repository tree: {path}")


def odt_extract(odt: Path, directory: Path) -> None:
    with ZipFile(odt) as archive:
        archive.extractall(directory)


def odt_pack(directory: Path, output: Path) -> None:
    """Pack a valid ODT with stable entry ordering/timestamps."""
    output.parent.mkdir(parents=True, exist_ok=True)
    with ZipFile(output, "w") as archive:
        mimetype = directory / "mimetype"
        info = ZipInfo("mimetype", FIXED_ZIP_TIMESTAMP)
        info.compress_type = ZIP_STORED
        archive.writestr(info, mimetype.read_bytes())
        for path in sorted(directory.rglob("*")):
            if not path.is_file() or path == mimetype:
                continue
            relative = path.relative_to(directory).as_posix()
            info = ZipInfo(relative, FIXED_ZIP_TIMESTAMP)
            info.compress_type = ZIP_DEFLATED
            archive.writestr(info, path.read_bytes())


def ensure_child(parent: ET.Element, tag: str) -> ET.Element:
    child = parent.find(tag)
    if child is None:
        child = ET.SubElement(parent, tag)
    return child


def find_named_style(root: ET.Element, name: str) -> ET.Element | None:
    for element in root.iter(q("style", "style")):
        if element.get(q("style", "name")) == name:
            return element
    return None


def set_text_property(style: ET.Element, prefix: str, local: str, value: str) -> None:
    props = ensure_child(style, q("style", "text-properties"))
    props.set(q(prefix, local), value)


def set_para_property(style: ET.Element, prefix: str, local: str, value: str) -> None:
    props = ensure_child(style, q("style", "paragraph-properties"))
    props.set(q(prefix, local), value)


def remove_para_property(style: ET.Element, prefix: str, local: str) -> None:
    props = style.find(q("style", "paragraph-properties"))
    if props is not None:
        props.attrib.pop(q(prefix, local), None)


def theme_reference(reference: Path, theme_path: Path, output: Path) -> None:
    """Create an ephemeral themed reference ODT without modifying the canonical one."""
    theme = load_toml(theme_path)
    accent = theme["colors"]["accent"]
    text_color = theme["colors"]["text"]
    secondary = theme["colors"]["secondary_text"]
    body_font = theme["fonts"]["body"]
    heading_font = theme["fonts"]["heading"]

    with tempfile.TemporaryDirectory(prefix="styr-reference-") as raw:
        directory = Path(raw)
        odt_extract(reference, directory)
        styles_path = directory / "styles.xml"
        tree = ET.parse(styles_path)
        root = tree.getroot()

        link = find_named_style(root, "Internet_20_link")
        if link is not None:
            set_text_property(link, "fo", "color", accent)

        standard = find_named_style(root, "Standard")
        if standard is not None:
            set_text_property(standard, "fo", "color", text_color)
            set_text_property(standard, "style", "font-name", body_font)
            set_text_property(standard, "fo", "font-family", f"'{body_font}'")
            set_text_property(standard, "fo", "font-size", "8pt")

        # The supplied Styr template uses 13 pt bold chapter headings with a
        # strong bottom rule on the actual manual pages. Pandoc's Heading 1 is
        # mapped to that visible chapter treatment. The named Heading 2/3 sizes
        # come directly from the template's style set.
        for name, size in (("Heading_20_1", "13pt"), ("Heading_20_2", "13pt"), ("Heading_20_3", "12pt")):
            style = find_named_style(root, name)
            if style is None:
                continue
            set_text_property(style, "fo", "color", text_color)
            set_text_property(style, "style", "font-name", heading_font)
            set_text_property(style, "fo", "font-family", f"'{heading_font}'")
            set_text_property(style, "fo", "font-size", size)
            set_text_property(style, "fo", "font-weight", "bold")
            set_para_property(style, "fo", "text-align", "left")
        heading1 = find_named_style(root, "Heading_20_1")
        if heading1 is not None:
            heading1.set(q("style", "master-page-name"), "Converted1")
            set_para_property(heading1, "fo", "margin-top", "0.388cm")
            set_para_property(heading1, "fo", "margin-bottom", "0.194cm")
            set_para_property(heading1, "fo", "padding-bottom", "0.212cm")
            set_para_property(heading1, "fo", "border-bottom", f"2.01pt solid {accent}")
            set_para_property(heading1, "fo", "keep-with-next", "always")
            # Selecting the content master page already starts the content page.
            # A simultaneous break-before creates an otherwise empty page between
            # the static cover and the first chapter in LibreOffice.
            remove_para_property(heading1, "fo", "break-before")

        caption = find_named_style(root, "Caption")
        if caption is not None:
            set_text_property(caption, "fo", "color", secondary)
            set_text_property(caption, "style", "font-name", heading_font)
            set_text_property(caption, "fo", "font-size", "7.5pt")
            set_text_property(caption, "fo", "font-style", "italic")

        table_contents = find_named_style(root, "Table_20_Contents")
        if table_contents is not None:
            set_text_property(table_contents, "style", "font-name", body_font)
            set_text_property(table_contents, "fo", "font-size", "7.5pt")
            set_text_property(table_contents, "fo", "color", text_color)

        # Semantic Markdown callouts use custom Writer styles. Keeping layout
        # here means Markdown carries intent (note/warning), not colors/padding.
        office_styles = root.find(q("office", "styles"))
        if office_styles is None:
            raise RuntimeError("reference ODT styles.xml has no office:styles element")
        note = find_named_style(root, "StyrNote")
        if note is None:
            note = ET.SubElement(office_styles, q("style", "style"), {
                q("style", "name"): "StyrNote",
                q("style", "display-name"): "Styr Note",
                q("style", "family"): "paragraph",
                q("style", "parent-style-name"): "Standard",
            })
        note_para = ensure_child(note, q("style", "paragraph-properties"))
        note_para.set(q("fo", "background-color"), theme["colors"]["warning_background"])
        note_para.set(q("fo", "border-left"), f"3pt solid {theme['colors']['warning']}")
        note_para.set(q("fo", "padding-left"), "0.16cm")
        note_para.set(q("fo", "padding-right"), "0.16cm")
        note_para.set(q("fo", "padding-top"), "0.05cm")
        note_para.set(q("fo", "padding-bottom"), "0.05cm")
        note_para.set(q("fo", "margin-top"), "0cm")
        note_para.set(q("fo", "margin-bottom"), "0cm")
        set_text_property(note, "fo", "color", text_color)
        set_text_property(note, "style", "font-name", body_font)
        set_text_property(note, "fo", "font-family", f"'{body_font}'")
        set_text_property(note, "fo", "font-size", "8pt")

        note_label_paragraph = find_named_style(root, "StyrNoteLabelParagraph")
        if note_label_paragraph is None:
            note_label_paragraph = ET.SubElement(office_styles, q("style", "style"), {
                q("style", "name"): "StyrNoteLabelParagraph",
                q("style", "display-name"): "Styr Note Label Paragraph",
                q("style", "family"): "paragraph",
                q("style", "parent-style-name"): "StyrNote",
            })
        note_label_para = ensure_child(note_label_paragraph, q("style", "paragraph-properties"))
        note_label_para.set(q("fo", "background-color"), theme["colors"]["warning_background"])
        note_label_para.set(q("fo", "border-left"), f"3pt solid {theme['colors']['warning']}")
        note_label_para.set(q("fo", "padding-left"), "0.16cm")
        note_label_para.set(q("fo", "padding-right"), "0.16cm")
        note_label_para.set(q("fo", "padding-top"), "0.05cm")
        note_label_para.set(q("fo", "padding-bottom"), "0cm")
        note_label_para.set(q("fo", "margin-top"), "0cm")
        note_label_para.set(q("fo", "margin-bottom"), "0cm")
        note_label_para.set(q("fo", "keep-with-next"), "always")

        label = find_named_style(root, "StyrNoteLabel")
        if label is None:
            label = ET.SubElement(office_styles, q("style", "style"), {
                q("style", "name"): "StyrNoteLabel",
                q("style", "display-name"): "Styr Note Label",
                q("style", "family"): "text",
            })
        set_text_property(label, "fo", "color", theme["colors"]["warning"])
        set_text_property(label, "style", "font-name", heading_font)
        set_text_property(label, "fo", "font-family", f"'{heading_font}'")
        set_text_property(label, "fo", "font-size", "8pt")
        set_text_property(label, "fo", "font-weight", "bold")
        set_text_property(label, "fo", "letter-spacing", "0.04cm")

        tree.write(styles_path, encoding="UTF-8", xml_declaration=True)
        odt_pack(directory, output)


def _copy_cover(reference_content: ET.Element, generated_content: ET.Element) -> None:
    """Prepend the supplied full-page cover artwork to Pandoc output."""
    reference_text = reference_content.find(f"./{q('office', 'body')}/{q('office', 'text')}")
    generated_text = generated_content.find(f"./{q('office', 'body')}/{q('office', 'text')}")
    reference_auto = reference_content.find(q("office", "automatic-styles"))
    generated_auto = generated_content.find(q("office", "automatic-styles"))
    if reference_text is None or generated_text is None or reference_auto is None or generated_auto is None:
        raise RuntimeError("ODT content.xml is missing required body/automatic-style sections")

    cover = None
    for paragraph in reference_text.findall(q("text", "p")):
        image = paragraph.find(f".//{q('draw', 'image')}")
        frame = paragraph.find(f".//{q('draw', 'frame')}")
        if image is not None and frame is not None:
            width = frame.get(q("svg", "width"), "")
            height = frame.get(q("svg", "height"), "")
            if width.startswith("9.89") and height.startswith("21.00"):
                cover = copy.deepcopy(paragraph)
                break
    if cover is None:
        raise RuntimeError("reference ODT does not contain the expected full-page cover image")

    old_paragraph_style = cover.get(q("text", "style-name"))
    if not old_paragraph_style:
        raise RuntimeError("reference cover paragraph has no style")
    cover.set(q("text", "style-name"), "StyrCoverParagraph")

    frame = cover.find(f".//{q('draw', 'frame')}")
    old_graphic_style = frame.get(q("draw", "style-name")) if frame is not None else None
    if frame is not None:
        frame.set(q("draw", "style-name"), "StyrCoverGraphic")

    def clone_auto_style(old_name: str | None, new_name: str) -> None:
        if not old_name:
            return
        old = None
        for style in reference_auto.findall(q("style", "style")):
            if style.get(q("style", "name")) == old_name:
                old = copy.deepcopy(style)
                break
        if old is None:
            raise RuntimeError(f"reference cover requires missing automatic style {old_name!r}")
        # Remove any stale same-name style in generated output.
        for style in list(generated_auto.findall(q("style", "style"))):
            if style.get(q("style", "name")) == new_name:
                generated_auto.remove(style)
        old.set(q("style", "name"), new_name)
        generated_auto.append(old)

    clone_auto_style(old_paragraph_style, "StyrCoverParagraph")
    clone_auto_style(old_graphic_style, "StyrCoverGraphic")

    # Remove a Pandoc metadata title block if one was supplied by Markdown;
    # the reference cover is the document title surface.
    for child in list(generated_text):
        if child.tag not in (q("text", "p"), q("text", "h")):
            continue
        style = child.get(q("text", "style-name"), "")
        if style in {"Title", "Subtitle", "Author"}:
            generated_text.remove(child)
            continue
        break

    # Insert after sequence-decls, which ODF requires near the start of office:text.
    insert_at = 1 if len(generated_text) and generated_text[0].tag == q("text", "sequence-decls") else 0
    generated_text.insert(insert_at, cover)

    # Normally the first manual block is Heading 1. Its reference style both
    # forces a new page and selects the content master page. For defensive use,
    # add an invisible page-transition paragraph when the source starts with
    # something else.
    following = generated_text[insert_at + 1] if len(generated_text) > insert_at + 1 else None
    is_h1 = following is not None and following.tag == q("text", "h") and following.get(q("text", "outline-level")) == "1"
    if not is_h1:
        transition = ET.Element(q("text", "p"), {q("text", "style-name"): "StyrContentStart"})
        transition_style = ET.Element(q("style", "style"), {
            q("style", "name"): "StyrContentStart",
            q("style", "family"): "paragraph",
            q("style", "parent-style-name"): "Standard",
            q("style", "master-page-name"): "Converted1",
        })
        props = ET.SubElement(transition_style, q("style", "paragraph-properties"))
        props.set(q("fo", "break-before"), "page")
        props.set(q("fo", "margin-top"), "0cm")
        props.set(q("fo", "margin-bottom"), "0cm")
        text_props = ET.SubElement(transition_style, q("style", "text-properties"))
        text_props.set(q("fo", "font-size"), "1pt")
        generated_auto.append(transition_style)
        generated_text.insert(insert_at + 1, transition)


def _style_headings(content_root: ET.Element, styles_root: ET.Element, theme: dict) -> None:
    """Map Markdown headings to the visible chapter treatment in the supplied template."""
    accent = theme["colors"]["accent"]
    text_color = theme["colors"]["text"]
    heading_font = theme["fonts"]["heading"]
    auto = content_root.find(q("office", "automatic-styles"))
    body = content_root.find(f"./{q('office', 'body')}/{q('office', 'text')}")
    if auto is None or body is None:
        raise RuntimeError("ODT content.xml is missing heading style/body sections")

    specs = {
        "1": ("StyrHeading1", "13pt", "0.388cm", "0.194cm"),
        "2": ("StyrHeading2", "13pt", "0.30cm", "0.15cm"),
        "3": ("StyrHeading3", "12pt", "0.22cm", "0.12cm"),
    }
    for level, (name, size, margin_top, margin_bottom) in specs.items():
        for existing in list(auto.findall(q("style", "style"))):
            if existing.get(q("style", "name")) == name:
                auto.remove(existing)
        attrs = {
            q("style", "name"): name,
            q("style", "family"): "paragraph",
            q("style", "parent-style-name"): "Standard",
        }
        if level == "1":
            # The supplied cover uses a first-page master; assigning Converted1
            # starts every top-level chapter on a normal content page.
            attrs[q("style", "master-page-name")] = "Converted1"
        style = ET.SubElement(auto, q("style", "style"), attrs)
        para = ET.SubElement(style, q("style", "paragraph-properties"))
        para.set(q("fo", "margin-top"), margin_top)
        para.set(q("fo", "margin-bottom"), margin_bottom)
        para.set(q("fo", "keep-with-next"), "always")
        para.set(q("fo", "text-align"), "left")
        if level == "1":
            para.set(q("fo", "padding-bottom"), "0.212cm")
            para.set(q("fo", "border-bottom"), f"2.01pt solid {accent}")
        text = ET.SubElement(style, q("style", "text-properties"))
        text.set(q("fo", "color"), text_color)
        text.set(q("style", "font-name"), heading_font)
        text.set(q("fo", "font-family"), f"'{heading_font}'")
        text.set(q("fo", "font-size"), size)
        text.set(q("fo", "font-weight"), "bold")

    for heading in body.iter(q("text", "h")):
        level = heading.get(q("text", "outline-level"))
        if level in specs:
            heading.set(q("text", "style-name"), specs[level][0])

    # Pandoc uses a logical FigureCaption style name that is not necessarily
    # emitted as a named Writer style. Map it to the template's Caption style.
    for paragraph in body.iter(q("text", "p")):
        if paragraph.get(q("text", "style-name")) == "FigureCaption":
            paragraph.set(q("text", "style-name"), "Caption")
        if paragraph.get(q("text", "style-name")) == "StyrNote":
            visible = "".join(paragraph.itertext()).strip().upper()
            if visible in {"NOTE", "WARNING"}:
                paragraph.set(q("text", "style-name"), "StyrNoteLabelParagraph")
                for span in paragraph.iter(q("text", "span")):
                    span.set(q("text", "style-name"), "StyrNoteLabel")

    # The visible template numbers top-level chapters ("1 Introduction",
    # "2 Controls", ...), while lower-level headings remain unnumbered.
    for outline in styles_root.iter(q("text", "outline-style")):
        for level_style in outline.findall(q("text", "outline-level-style")):
            level = level_style.get(q("text", "level"))
            if level == "1":
                level_style.set(q("style", "num-format"), "1")
                level_style.set(q("style", "num-suffix"), " ")
            elif level in {"2", "3"}:
                level_style.set(q("style", "num-format"), "")


def _style_tables(content_root: ET.Element, styles_root: ET.Element, theme: dict) -> None:
    accent = theme["colors"]["accent"]
    text_color = theme["colors"]["text"]
    heading_font = theme["fonts"]["heading"]
    body_font = theme["fonts"]["body"]

    auto = content_root.find(q("office", "automatic-styles"))
    if auto is not None:
        for style_name, background, padding in (
            ("TableHeaderRowCell", accent, "0.08cm"),
            ("TableRowCell", "#ffffff", "0.08cm"),
        ):
            style = None
            for candidate in auto.findall(q("style", "style")):
                if candidate.get(q("style", "name")) == style_name:
                    style = candidate
                    break
            if style is None:
                continue
            cell_props = ensure_child(style, q("style", "table-cell-properties"))
            cell_props.set(q("fo", "background-color"), background)
            cell_props.set(q("fo", "border"), "none")
            cell_props.set(q("fo", "padding"), padding)

    office_styles = styles_root.find(q("office", "styles"))
    if office_styles is None:
        return

    heading = find_named_style(styles_root, "Table_20_Heading")
    if heading is None:
        heading = ET.SubElement(office_styles, q("style", "style"), {
            q("style", "name"): "Table_20_Heading",
            q("style", "display-name"): "Table Heading",
            q("style", "family"): "paragraph",
            q("style", "parent-style-name"): "Standard",
        })
    set_para_property(heading, "fo", "margin-top", "0cm")
    set_para_property(heading, "fo", "margin-bottom", "0cm")
    set_text_property(heading, "fo", "color", "#ffffff")
    set_text_property(heading, "style", "font-name", heading_font)
    set_text_property(heading, "fo", "font-size", "8pt")
    set_text_property(heading, "fo", "font-weight", "bold")

    body = find_named_style(styles_root, "Table_20_Contents")
    if body is not None:
        set_para_property(body, "fo", "margin-top", "0cm")
        set_para_property(body, "fo", "margin-bottom", "0cm")
        set_text_property(body, "fo", "color", text_color)
        set_text_property(body, "style", "font-name", body_font)
        set_text_property(body, "fo", "font-size", "7.5pt")


def _set_metadata(directory: Path, *, title: str, version: str, locale: str, language_code: str, repository_url: str) -> None:
    meta_path = directory / "meta.xml"
    tree = ET.parse(meta_path)
    root = tree.getroot()
    office_meta = root.find(q("office", "meta"))
    if office_meta is None:
        raise RuntimeError("ODT meta.xml has no office:meta element")

    def set_simple(tag: str, value: str) -> None:
        element = office_meta.find(tag)
        if element is None:
            element = ET.SubElement(office_meta, tag)
        element.text = value

    set_simple(q("dc", "title"), title)
    set_simple(q("dc", "language"), locale)
    set_simple(q("dc", "creator"), "Axel Napolitano")
    set_simple(q("dc", "publisher"), "Axel Napolitano")
    set_simple(q("dc", "source"), repository_url)
    set_simple(q("dc", "identifier"), f"styr-user-manual.{language_code}.{version}")
    set_simple(q("dc", "description"), f"Styr User Manual, firmware {version}, locale {locale}.")

    # Strip edit-session metadata that would otherwise make the ODT noisier.
    for tag in (q("meta", "editing-duration"), q("meta", "editing-cycles")):
        for element in list(office_meta.findall(tag)):
            office_meta.remove(element)

    wanted = {
        "StyrManualVersion": version,
        "StyrManualLocale": locale,
        "StyrManualLanguageCode": language_code,
    }
    existing = {}
    for element in office_meta.findall(q("meta", "user-defined")):
        name = element.get(q("meta", "name"))
        if name:
            existing[name] = element
    for name, value in wanted.items():
        element = existing.get(name)
        if element is None:
            element = ET.SubElement(office_meta, q("meta", "user-defined"), {q("meta", "name"): name})
        element.text = value

    tree.write(meta_path, encoding="UTF-8", xml_declaration=True)


def finalize_odt(intermediate: Path, reference: Path, theme_path: Path, output: Path, *, title: str, version: str, locale: str, language_code: str, repository_url: str) -> None:
    theme = load_toml(theme_path)
    with tempfile.TemporaryDirectory(prefix="styr-final-odt-") as generated_raw, tempfile.TemporaryDirectory(prefix="styr-reference-odt-") as reference_raw:
        generated_dir = Path(generated_raw)
        reference_dir = Path(reference_raw)
        odt_extract(intermediate, generated_dir)
        odt_extract(reference, reference_dir)

        generated_content_tree = ET.parse(generated_dir / "content.xml")
        reference_content_tree = ET.parse(reference_dir / "content.xml")
        generated_styles_tree = ET.parse(generated_dir / "styles.xml")

        _copy_cover(reference_content_tree.getroot(), generated_content_tree.getroot())
        _style_headings(generated_content_tree.getroot(), generated_styles_tree.getroot(), theme)
        _style_tables(generated_content_tree.getroot(), generated_styles_tree.getroot(), theme)

        generated_content_tree.write(generated_dir / "content.xml", encoding="UTF-8", xml_declaration=True)
        generated_styles_tree.write(generated_dir / "styles.xml", encoding="UTF-8", xml_declaration=True)

        # Pandoc normally copies reference-document images, but make the cover
        # and footer assets robust against writer-version differences.
        reference_pictures = reference_dir / "Pictures"
        generated_pictures = generated_dir / "Pictures"
        generated_pictures.mkdir(exist_ok=True)
        if reference_pictures.exists():
            for image in reference_pictures.iterdir():
                if image.is_file() and not (generated_pictures / image.name).exists():
                    shutil.copy2(image, generated_pictures / image.name)

        # Likewise preserve matching manifest entries for reference pictures.
        ref_manifest = reference_dir / "META-INF" / "manifest.xml"
        gen_manifest = generated_dir / "META-INF" / "manifest.xml"
        if ref_manifest.exists() and gen_manifest.exists():
            manifest_ns = "urn:oasis:names:tc:opendocument:xmlns:manifest:1.0"
            ET.register_namespace("manifest", manifest_ns)
            mq = lambda local: f"{{{manifest_ns}}}{local}"
            rt = ET.parse(ref_manifest); gt = ET.parse(gen_manifest)
            gr = gt.getroot()
            existing_paths = {e.get(mq("full-path")) for e in gr.findall(mq("file-entry"))}
            for entry in rt.getroot().findall(mq("file-entry")):
                path = entry.get(mq("full-path"), "")
                if path.startswith("Pictures/") and path not in existing_paths:
                    gr.append(copy.deepcopy(entry))
            gt.write(gen_manifest, encoding="UTF-8", xml_declaration=True)

        _set_metadata(generated_dir, title=title, version=version, locale=locale, language_code=language_code, repository_url=repository_url)
        odt_pack(generated_dir, output)


def font_preflight(required_families: list[str]) -> list[str]:
    fc_match = resolve_tool(("fc-match",), required=False)
    if not fc_match:
        return ["fontconfig fc-match is not available"]
    errors = []
    for family in required_families:
        result = subprocess.run(
            [fc_match, "-f", "%{family}|%{style}\n", family],
            check=True,
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        ).stdout.strip()
        normalized = result.lower()
        if "ubuntu" not in normalized:
            errors.append(f"{family}: resolved to {result or '(nothing)'}")
        elif family.lower().endswith(" light") and "light" not in normalized:
            errors.append(f"{family}: light style not resolved ({result})")
    return errors


def run_pandoc(sources: list[Path], reference: Path, output: Path, locale: str) -> None:
    pandoc = resolve_tool(("pandoc",))
    resource_paths = {str(ROOT), str(ROOT / "docs" / "manual")}
    resource_paths.update(str(source.parent) for source in sources)
    command = [
        pandoc,
        *[str(source) for source in sources],
        "--from=markdown+fenced_divs+link_attributes+implicit_figures",
        "--to=odt",
        "--standalone",
        f"--reference-doc={reference}",
        f"--resource-path={os.pathsep.join(sorted(resource_paths))}",
        f"--metadata=lang:{locale}",
        f"--output={output}",
    ]
    subprocess.run(command, check=True, cwd=ROOT)


def run_libreoffice(odt: Path, output_dir: Path) -> Path:
    libreoffice = resolve_tool(("libreoffice", "soffice"))
    with tempfile.TemporaryDirectory(prefix="styr-lo-profile-") as profile:
        profile_uri = Path(profile).resolve().as_uri()
        command = [
            libreoffice,
            "--headless",
            "--nologo",
            "--nodefault",
            "--nolockcheck",
            "--nofirststartwizard",
            f"-env:UserInstallation={profile_uri}",
            "--convert-to",
            "pdf:writer_pdf_Export",
            "--outdir",
            str(output_dir),
            str(odt),
        ]
        subprocess.run(command, check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    pdf = output_dir / f"{odt.stem}.pdf"
    if not pdf.exists() or pdf.stat().st_size < 1024:
        raise RuntimeError(f"LibreOffice did not produce a valid PDF: {pdf}")
    return pdf


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--version", required=True, help="Firmware release/tag version, e.g. 0.1.4 or v0.1.4")
    parser.add_argument("--locale", default=None, help="Locale key from docs/manual/manual.toml (default: source locale)")
    parser.add_argument("--output-dir", type=Path, default=Path("/tmp/styr-manual"), help="Generated artifact directory (must be outside repository)")
    parser.add_argument("--smoke", action="store_true", help="Build the toolchain smoke document instead of the maintained manual")
    parser.add_argument("--odt-only", action="store_true", help="Stop after the finalized ODT")
    parser.add_argument("--allow-font-substitution", action="store_true", help="Permit PDF smoke builds without Ubuntu fonts; never use for release artifacts")
    args = parser.parse_args()

    try:
        version = normalize_version(args.version)
        config = load_toml(CONFIG_PATH)
        manual = config["manual"]
        locale = args.locale or manual["source_locale"]
        locale_config = config["locales"].get(locale)
        if not locale_config or not locale_config.get("enabled", False):
            raise RuntimeError(f"manual locale is not enabled: {locale}")
        language_code = locale_config["language_code"]
        if not re.fullmatch(r"[a-z]{2}", language_code):
            raise RuntimeError(f"release language_code must be ISO 639-1 lower-case: {language_code!r}")

        output_dir = args.output_dir.resolve()
        ensure_outside_repo(output_dir)
        output_dir.mkdir(parents=True, exist_ok=True)

        reference = ROOT / manual["reference_document"]
        theme_path = ROOT / manual["theme_document"]
        if args.smoke:
            sources = [SMOKE_SOURCE]
        else:
            sources = [ROOT / source for source in locale_config["chapters"]]
        missing = [source for source in sources if not source.exists()]
        if missing:
            raise RuntimeError("manual source file(s) missing: " + ", ".join(str(path.relative_to(ROOT)) for path in missing))

        basename = f"{manual['output_basename']}.{language_code}.{version}"
        final_odt = output_dir / f"{basename}.odt"

        with tempfile.TemporaryDirectory(prefix="styr-manual-build-") as raw:
            workspace = Path(raw)
            themed_reference = workspace / "reference.odt"
            intermediate = workspace / "pandoc.odt"
            theme_reference(reference, theme_path, themed_reference)
            run_pandoc(sources, themed_reference, intermediate, locale)
            finalize_odt(
                intermediate,
                themed_reference,
                theme_path,
                final_odt,
                title=manual["title"],
                version=version,
                locale=locale,
                language_code=language_code,
                repository_url=manual["repository_url"],
            )

        print(f"ODT: {final_odt}")
        if args.odt_only:
            return 0

        theme = load_toml(theme_path)
        font_errors = font_preflight(list(theme["fonts"]["required_families"]))
        if font_errors and not args.allow_font_substitution:
            raise RuntimeError(
                "required release fonts are unavailable; run fetch_ubuntu_fonts.py or install Ubuntu/Ubuntu Light:\n - "
                + "\n - ".join(font_errors)
            )
        if font_errors:
            print("WARNING: PDF build is using font substitution; this is allowed only for smoke/debug runs.", file=sys.stderr)
            for error in font_errors:
                print(f" - {error}", file=sys.stderr)

        pdf = run_libreoffice(final_odt, output_dir)
        print(f"PDF: {pdf}")
        return 0
    except (KeyError, OSError, RuntimeError, ValueError, subprocess.CalledProcessError) as error:
        print(f"Manual build FAILED: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
