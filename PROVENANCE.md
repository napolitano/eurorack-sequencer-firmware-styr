# Code provenance

Styr is derived from the MIT-licensed Westlicht Performer firmware and currently remains in an active migration phase.

The project tracks source provenance using four conceptual states:

- **UPSTREAM** — substantially original upstream implementation;
- **DERIVED** — upstream implementation with Styr modifications;
- **STYR** — independently implemented Styr code without material source-code carry-over;
- **THIRD_PARTY** — external code governed by its own license.

The current structural migration does not claim that existing derived files have become Styr-native merely because they were moved or renamed. File movement, formatting, renaming and mechanical refactoring do not change provenance.

The repository currently retains the MIT license as the baseline license for inherited code. A future PolyForm Noncommercial licensing layer for Styr-native contributions must be introduced with file-level provenance and license metadata rather than by removing upstream notices.

`LICENSES/MIT.txt` contains the inherited MIT license text. Third-party license inventory and machine-readable SPDX/REUSE metadata will be added as dependencies are audited during the modernization work.


## File-level source headers

First-party source files record purpose, implementation lineage and the current MIT software license in a short source header. The role wording distinguishes Simon Kallweit's original PER|FORMER implementation from Axel Napolitano's Styr modifications and modernization work. See `docs/development/SOURCE_HEADERS.md`.

This metadata is deliberately not applied to `third_party/`. Imported files retain their own copyright and license notices. The public-domain MD5 implementation used by the bootloader is treated as third-party source even though historical Performer releases stored it inside the bootloader application directory.

## Documentation

New Styr documentation under `docs/` is licensed separately under CC BY-NC 4.0 unless a file says otherwise. Inherited PER|FORMER documentation is isolated under `docs/legacy/performer/` and retains its upstream MIT provenance.
