---
knowledge-id: styr.manual.change-catalog
status: canonical-index
locale: en-US
---

# User-visible change catalog

This file is an index for documentation impact analysis. The complete public wording remains in the root `README.md` under **Changes from PER|FORMER**. Do not maintain an independent duplicate bug ledger here.

During a documentation sync, inspect all four README tables:

1. **Fixed upstream bugs** — inherited PER|FORMER v0.1.42 defects whose user-visible behavior changed in Styr.
2. **Implemented upstream TODOs** — explicit inherited TODOs/gaps that Styr now implements.
3. **Improvements** — user-visible or workflow-quality improvements that are not presented as inherited defects.
4. **New features** — functionality added by Styr.

## Current documentation-impact areas

### Fixed inherited behavior

The current README groups verified fixes across:

- external clock acquisition, BPM estimation, swing, timeout, phase/rollover, transport state, source disable, and clock-output pulse timing;
- single-step sequence PingPong behavior;
- Note first-step live-record quantization and minimum representable gate/retrigger duration;
- Curve Free divisor changes, Next Pattern consistency, complete step edit comparison, and minimum gate duration.

### Implemented inherited TODO

- signed/negative Note Gate Offset with deterministic look-ahead and conservative boundary fallback.

### Current improvements

- external-clock robustness against isolated bad edges;
- generation-owned chronological Note microtiming events;
- explicit microtiming boundary policy;
- corrected simulator timer accounting;
- separated PlatformIO product tests and CMake/CTest simulator tests;
- structured source documentation and future Doxygen technical-reference output.

### Current Styr features

The README currently lists:

- LFO Track;
- Acid Bassline Generator;
- Overview mini map;
- Overview note labels;
- Advanced Settings.

## Agent rule

A documentation-sync task must compare the target state to the requested baseline and determine which catalog entries are new or changed in that interval. Do not blindly rewrite the manual from the whole catalog on every run.
