<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Generator Screen Reference

Sequence generators create or transform step data from a small parameter set. Generator pages are preview-oriented: parameter changes update the candidate result, while the generator context menu controls whether that result is initialized, reverted or committed.

## Common workflow

1. Open the Steps page for the sequence/layer you want to modify.
2. Hold `SHIFT` + `PAGE` and open the generator selection.
3. Select a generator and confirm it.
4. Hold the labeled function key while turning the encoder to edit its parameter.
5. Hold `SHIFT` + `PAGE` on the generator page to **Init**, **Revert** or **Commit**.
6. **Commit** returns the generated values to the normal editable sequence; **Revert** restores the pre-generator state.

**Init Layer** is intentionally different: it acts immediately on the current layer and therefore has no preview/Commit page.

## Generator selection

- [Init Layer](generator-select-init-layer.md)
- [Euclidean](generator-select-euclidean.md)
- [Random](generator-select-random.md)
- [Acid Bassline](generator-select-acid-bassline.md)

## Euclidean

- [Overview](generator-euclidean.md)
- [Steps](generator-euclidean-steps.md)
- [Beats](generator-euclidean-beats.md)
- [Offset](generator-euclidean-offset.md)
- [Commit menu](generator-euclidean-commit-menu.md)
- [Committed result](generator-euclidean-committed.md)

## Random

- [Overview](generator-random.md)
- [Seed](generator-random-seed.md)
- [Smooth](generator-random-smooth.md)
- [Bias](generator-random-bias.md)
- [Scale](generator-random-scale.md)
- [Commit menu](generator-random-commit-menu.md)
- [Committed result](generator-random-committed.md)

## Acid Bassline

- [Overview](acid-bassline-generator.md)
- [Seed](acid-bassline-seed.md)
- [Root](acid-bassline-root.md)
- [Length](acid-bassline-length.md)
- [Density](acid-bassline-density.md)
- [Legato](acid-bassline-legato.md)
- [Commit menu](acid-bassline-commit-menu.md)
- [Committed result](acid-bassline-committed.md)

For the feature-level explanation, see the [Acid Bassline guide](../../features/acid-bassline/README.md).

From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
