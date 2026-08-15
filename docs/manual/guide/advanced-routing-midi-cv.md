<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Advanced Routing, MIDI, and CV

## Layout first, then behavior

Use [Layout pages](../screens/layout/README.md) to decide where track outputs appear physically.
Then configure track behavior (Note/Curve/LFO/MIDI/CV) for those destinations.

## Routing matrix

Routing lets project or track targets be modulated by available sources.
Use [Routing](../screens/routing/routing.md) to pick routes and [Routing Edit](../screens/routing/routing-edit.md) for source/amount details.

If you use external controllers, MIDI learn can accelerate assignment workflows.

## MIDI/CV Track workflow

MIDI/CV Tracks convert incoming MIDI into CV/gate voice allocation:

1. choose MIDI source/channel
2. set voice count and allocation behavior
3. set note range and priority
4. configure retrigger/slide/arp behavior as needed

See [MIDI/CV Track reference](../screens/tracks/midi-cv-track.md).

## MIDI Output mappings

Use [MIDI Output](../screens/routing/midi-output.md) when Styr needs to send MIDI events based on internal sequencing and mapping decisions.

## User scales

Use [User Scale](../screens/routing/user-scale.md) to define custom pitch collections and then apply them in sequence editing and generation workflows.
