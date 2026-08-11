<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Display Header

![Display Header](../../assets/header.png)

## Function

The header keeps global sequencer state and the active editing context visible while the body of the display changes from page to page.

## Information shown

- **Top left:** clock mode and tempo. The inherited clock indicators are Auto, Master and Slave; recording replaces the normal clock-mode indicator while recording is active.
- **Centre:** selected track plus playing and editing pattern context. Playing and editing patterns are intentionally distinct because Styr can edit a pattern other than the one currently playing.
- **Top right:** active page and, where relevant, a sub-page, layer or mode label.

## Operation

The header itself is not edited. Its fields follow the current transport, track, pattern and page state automatically. When a step layer or other sub-mode changes, the header is one of the main places to confirm the new context.

From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
