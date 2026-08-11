<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Display Layout

The 256×64 display is organized into a **header**, a page-specific **body**, and a **footer**. The body changes with the active page; the outer regions provide stable context and dynamic control labels.

## Header

The header summarizes global and editing context: clock/recording state and tempo on the left, selected track and play/edit pattern state in the centre, and the active page plus optional sub-mode on the right.

- [Header reference](header.md)

## Body

The body is the working area. Depending on the page it can be a list editor, a step/layer view, a waveform display, a routing editor, a generator preview, a song list or a system dialog.

## Footer

The footer labels the five physical function keys. Labels are contextual: a function key may select a sub-page, cycle a layer group, edit a parameter, or become a context-menu action while `SHIFT` + `PAGE` is held.

- [Footer reference](footer.md)

The display structure follows the inherited PER|FORMER interaction model; Styr-specific pages keep the same principle so the current context and available function-key actions remain visible.

From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
