<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Styr Firmware Update

For normal end-user updates, use the **SD-card update path**. It uses the existing bootloader, validates the update image before writing it, verifies the programmed application afterwards, and does not require opening the rack or attaching a debug probe.

## Recommended: SD-card update

A release update is distributed as:

```text
UPDATE.DAT
```

Copy `UPDATE.DAT` to the **root directory of the SD card** and insert the card into the sequencer.

> [!IMPORTANT]
> `UPDATE.DAT` is an application update for systems that already have a compatible working bootloader. It is the recommended installation method for end users.

Enter the bootloader using either method:

1. **From a powered-off module:** hold the encoder while powering the sequencer on.
2. **From Styr:** open `SYSTEM` → `UPDATE`, then use the encoder action shown on that page to reset into the bootloader.

The bootloader reads `UPDATE.DAT`, checks its MD5 checksum and displays the installed and proposed firmware versions. When verification succeeds:

1. rotate the encoder until **YES** is selected;
2. press the encoder to start writing;
3. wait while the image is programmed and verified;
4. when the bootloader reports completion, press the encoder to reset.

> [!WARNING]
> **Do not remove Eurorack power or the SD card while the bootloader is writing or verifying the firmware.** An interrupted write can leave the application invalid. A working bootloader can normally recover from an invalid application by accepting another valid `UPDATE.DAT`.

### Building `UPDATE.DAT`

Developers can build the same update artifact from the repository root:

```sh
pio run -e application -t sd-update
```

The result is written to:

```text
dist/application/UPDATE.DAT
```

The PlatformIO packaging step appends the MD5 data expected by the bootloader. Do not substitute a raw `.bin` file for `UPDATE.DAT`.

## ST-Link: development and recovery only

ST-Link is useful for development, debugging and recovery, but it is **not** the preferred end-user update method.

> [!WARNING]
> **The ST-Link/debug header is not a protected user interface. Its signals connect directly to MCU/debug circuitry without the protection expected on an external hot-plug interface. Treat it as a development header.**

> [!WARNING]
> **When attaching ST-Link, use this connection/power order:**
>
> 1. power the module from the **Eurorack supply**;
> 2. connect the **ST-Link header** to the module;
> 3. only then connect the **ST-Link to the computer/USB host**.
>
> Do not use “ST-Link already powered from USB, then attach it to an unpowered module” as the normal connection sequence.

For normal application flashing through ST-Link:

```sh
pio run -e application -t upload
```

The normal application environment is linked/uploaded at `0x08010000`, leaving the bootloader region at the start of flash intact.

> [!WARNING]
> Do **not** use `application_standalone` for a routine firmware update. The standalone image starts at `0x08000000` and is intended for development/recovery scenarios where running without the normal bootloader layout is deliberate.

> [!WARNING]
> Do not flash the bootloader merely because an application update is available. A damaged bootloader removes the normal SD-card recovery/update path. Bootloader work should be treated as a separate development/recovery operation.

## Flash layout relevant to updates

The maintained layout is intentionally separated:

```text
0x08000000  bootloader
0x08008000  hardware configuration
0x08010000  normal Styr application
```

The SD-card updater writes the normal application region beginning at `0x08010000`; it is not a bootloader or hardware-configuration updater.

## Troubleshooting SD updates

If the bootloader does not find an update, check that the file is named exactly `UPDATE.DAT` and is in the SD-card root. If checksum verification fails, replace the file with a known-good release artifact rather than forcing a write.

If the normal application is invalid, the bootloader is designed to remain available on startup so another valid application image can be installed.

From Munich with <img src="docs/manual/assets/blue-heart.svg" alt="blue heart" width="14">
