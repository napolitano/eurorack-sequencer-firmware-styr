---
knowledge-id: styr.manual.interaction-model
status: canonical
locale: en-US
---

# Interaction model

This module records interaction rules that occur across several manual sections. Exact page-specific mappings still belong to the maintained screen reference.

## Function keys and layers

The footer labels five physical function keys contextually. A function key may select a page, cycle a layer group, edit a parameter, or become a context-menu action.

### Note Steps

The current maintained Note screen reference defines these layer groups:

| Key | Layer group |
|---|---|
| `F1` | Gate → Gate Probability → Gate Offset → Slide |
| `F2` | Retrigger → Retrigger Probability |
| `F3` | Length → Length Variation Range → Length Variation Probability |
| `F4` | Note → Note Variation Range → Note Variation Probability |
| `F5` | Condition |

`SHIFT` + the same function key jumps directly to the group's primary layer.

Source: `docs/manual/screens/note/README.md`.

### Curve Steps

| Key | Layer group |
|---|---|
| `F1` | Shape → Shape Variation → Shape Variation Probability |
| `F2` | Minimum |
| `F3` | Maximum |
| `F4` | Gate → Gate Probability |
| `F5` | unused in the current Curve step-layer map |

Source: `docs/manual/screens/curve/README.md`.

### LFO

The LFO is not a conventional 64-step editor. Its five function keys expose waveform/rate/mode/range controls directly. The exact normal and Shift-layer mapping is maintained in `docs/manual/screens/lfo/README.md` and must be checked there before editing user-facing instructions.

## Quick Edit

On Note and Curve Steps pages, holding `PAGE` with specific step keys overlays sequence-level parameters without leaving the Steps workflow. Releasing the chord returns to normal step editing.

Do not assume that every track type supports the same Quick Edit mapping. Use the track-specific screen reference.

## Context menus

The Note and Curve Steps context menu is opened with `SHIFT` + `PAGE`. Available actions depend on the current context. Generator access is documented only where the current UI exposes it.

## Generator workflow

The maintained generator workflow is:

1. open the relevant Steps page;
2. open generator selection through the Steps context menu;
3. select and confirm a generator;
4. edit generator parameters using its labeled function controls;
5. use the generator context menu for **Init**, **Revert**, or **Commit** where the generator supports preview;
6. **Commit** accepts the generated candidate into the normal editable sequence; **Revert** restores the pre-generator state.

`Init Layer` is intentionally different: it acts immediately and has no preview/Commit page.

Source: `docs/manual/screens/generators/README.md`.

## Documentation rule

Navigation instructions must be checked against the current screen reference and deterministic capture state. Do not infer a chord or button path from a similarly named page.
