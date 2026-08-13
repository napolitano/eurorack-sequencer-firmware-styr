---
knowledge-id: styr.manual.documentation-map
status: canonical
locale: en-US
---

# Documentation map

Use this map to locate all mutually affected manual areas before editing.

| Concept | Primary manual source | Related screen family |
|---|---|---|
| Manual entry point | `docs/manual/README.md` | all |
| Display anatomy | `docs/manual/screens/interface/README.md` | `interface/*` |
| Tempo / clock / project / patterns / performance | `docs/manual/screens/global/README.md` | `global/*` |
| Note Track | `docs/manual/screens/note/README.md` | `note/*` |
| Curve Track | `docs/manual/screens/curve/README.md` | `curve/*` |
| LFO Track workflow | `docs/manual/features/lfo/README.md` | `lfo/*` |
| LFO screen reference | `docs/manual/screens/lfo/README.md` | `lfo/*` |
| MIDI/CV Track | `docs/manual/screens/tracks/README.md` | `tracks/*` |
| Generators | `docs/manual/screens/generators/README.md` | `generators/*` |
| Acid Bassline workflow | `docs/manual/features/acid-bassline/README.md` | `generators/acid-bassline-*` |
| Routing / MIDI output / user scale | `docs/manual/screens/routing/README.md` | `routing/*` |
| Hardware/output layout | `docs/manual/screens/layout/README.md` | `layout/*` |
| Monitoring | `docs/manual/screens/monitor/README.md` | `monitor/*` |
| Song mode | `docs/manual/screens/song/README.md` | `song/*` |
| System / calibration / update | `docs/manual/screens/system/README.md` | `system/*` |

## Screenshot ownership

- Canonical generated images: `docs/manual/assets/`.
- Canonical capture definitions: `src/simulator/tools/manual_screenshots.cpp`.
- Capture/Markdown/asset validator: `toolchain/check_manual_screenshots.py`.
- Screenshot build instructions: `README_SCREENSHOTS.md`.

Every maintained capture has a stable semantic name, one expected PNG asset, and one corresponding screen Markdown page.

## Impact rule

When a behavior affects both a conceptual workflow and a concrete screen, update the feature/category guide and the individual screen reference together. A screenshot-only change is not sufficient when the user workflow changed; a prose-only change is not sufficient when the visible UI state changed materially.
