<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Clock Setup

![Clock Setup](../../assets/clock.png)

## Function

Configures whether Styr runs from its own clock or follows an external clock, along with the related input/output behavior.

## Access

Press `PAGE` + `TEMP`/Clock to open the Clock page.

## Operation

Use the encoder to select a row, press it to edit, then turn it to change the value.

- **Mode** — `Auto`, `Master`, or `Slave`.
- **Shift Mode** — what `SHIFT` + `PLAY` does: `Restart` starts again from the beginning, while `Pause` toggles Stop/Continue.
- **Input Divisor** — expected external clock rate, adjustable from `1` to `192`.
- **Input Mode** — how the external clock/reset jacks are interpreted: `Reset`, `Run`, or `StartStop`.
- **Output Divisor** — clock-output rate, adjustable from `2` to `192`.
- **Output Swing** — includes or ignores swing on the clock output.
- **Output Pulse** — clock pulse width, `1` to `20` ms.
- **Output Mode** — whether the clock output uses `Reset` or `Run` behavior.
- **MIDI RX / TX** and **USB RX / TX** — enable or disable clock receive/transmit over DIN MIDI and USB.

When Styr is following an external clock, the detected slave tempo is derived from the incoming pulse stream and does not overwrite the stored project tempo. In `Auto`, Styr locks to a valid external clock when one is present and releases it again if that clock disappears long enough.


From Munich with <img src="../../assets/blue-heart.svg" alt="blue heart" width="14">
