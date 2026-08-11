<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Source header and provenance policy

First-party source files carry a short file header that states the file purpose, the relevant implementation lineage and the software license. The header is documentation of provenance; it does not replace Git history or the license texts in `LICENSES/`.

## Attribution roles

- **Simon Kallweit — original PER|FORMER implementation** identifies source that descends from the upstream PER|FORMER firmware.
- **Axel Napolitano — Styr modifications and modernization** identifies Styr-side changes, maintenance or new implementation work.
- A mechanically moved upstream file does not become independently authored Styr code merely because its path changed.
- During the current migration, inherited first-party files may still list Axel Napolitano for Styr integration and modernization when their build integration, surrounding interfaces, or implementation has changed. This does not claim authorship of Simon Kallweit's original algorithms.
- Styr-native files that do not carry material PER|FORMER source list Axel Napolitano as the implementation author.

Files may contain additional upstream authors or license notices. Existing third-party notices always take precedence and must not be replaced by a Styr header.

## Licensing

The current first-party software baseline remains MIT-licensed. Source headers therefore use `SPDX-License-Identifier: MIT` and refer to `LICENSES/MIT.txt`.

The documentation license under `docs/` is separate and is described in `docs/LICENSE`.

## Third-party boundary

Files under `third_party/` retain their upstream notices and licenses. Imported code that is discovered inside a first-party directory should be moved to the appropriate `third_party/` location rather than relabeled as Styr source.

## Comment-free formats

Some machine-readable configuration formats used by the repository, notably JSON (`CMakePresets.json`, PlatformIO board definitions and VSCodium workspace JSON), do not permit comments. They are not modified with invented metadata fields merely to carry a header. Their provenance is documented by the surrounding first-party build files, Git history and this policy.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
