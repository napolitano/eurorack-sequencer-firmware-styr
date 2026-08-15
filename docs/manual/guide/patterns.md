<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Patterns

Styr provides 16 patterns per project. Pattern workflow separates:

- the pattern currently playing
- the pattern you are editing

Open [Pattern](../screens/global/pattern.md) to switch patterns and manage requests.

## Pattern change timing

Pattern requests can run as:

- **Immediate**
- **Latched** (stage several changes, commit together)
- **Sync** (execute on the configured sync boundary)

Track keys let you scope a request to selected tracks instead of all tracks.

## Snapshot workflow

The Pattern page supports snapshot-based experimentation:

- **Snap** creates a temporary snapshot
- **Revert** returns to the snapshot state
- **Commit** accepts the current result

Use this when testing edits or generator results during playback.

## Pattern context menu

From Pattern page context menu (`SHIFT` + `PAGE`), use:

- Init
- Copy
- Paste
- Duplicate

These operations affect the selected pattern context, so confirm play/edit pattern state in the header before executing destructive actions.
