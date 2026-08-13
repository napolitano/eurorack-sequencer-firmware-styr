<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Styr manual document style

`styr-reference.odt` is the canonical Writer reference document for deterministic Styr user-manual builds. It is derived from the maintained Styr ODT design template and supplies page geometry, master pages/footer, paragraph and character styles, the static cover artwork, and the compact table/caption typography used by the manual.

The reference document names **Ubuntu** and **Ubuntu Light**, but deliberately embeds no font binaries. Font files are not stored in this repository. Release/CI builds obtain the pinned Ubuntu font family as an ephemeral build dependency, verify it, install it only in the build environment, and then discard that environment after the run. A strict PDF build fails instead of silently accepting a substituted font.

The current Styr accent is black. Configurable document colors live in `theme.toml`; Markdown contains semantic content only and must not carry layout colors, font sizes, margins, or other publication-format instructions.

The intended publication path is:

```text
Markdown -> Pandoc ODT -> Styr ODT post-processing -> LibreOffice PDF
```

The ODT is retained as an intermediate build artifact because it makes layout failures diagnosable before PDF export. The PDF is the primary user-facing release artifact.

The cover artwork is currently supplied by the reference document. Firmware version and locale are injected into ODT metadata and the release filename by the build tool rather than hard-coded into Markdown.

## Semantic Markdown mapping

Publication-specific layout stays outside normal prose. Top-level Markdown headings become numbered Writer chapters with the template's black rule; level-2/3 headings use the corresponding compact heading treatment. Markdown figures are mapped to the centered grey italic caption style used by the supplied template.

For note/warning callouts, use the semantic Writer custom style rather than inline colors:

```markdown
::: {custom-style="StyrNote"}
**NOTE**

Callout text.
:::
```

The publication tool maps that block to the template-derived beige/orange note treatment and keeps the label with the following callout text across page breaks.
