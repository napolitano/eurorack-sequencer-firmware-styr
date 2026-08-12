<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Bootloader size investigation

## Flash constraint

The bootloader occupies the fixed 32 KiB flash region beginning at `0x08000000`. The hardware-configuration image starts at `0x08008000`, so enlarging the bootloader region is not a valid migration fix.

During the PlatformIO migration the bootloader initially exceeded this region by 7,936 bytes. Restoring the historical size-oriented compiler profile reduced the remaining overflow to 2,800 bytes, but further compiler-flag changes did not remove it.

## Step 6.13 repair

The remaining size pressure was addressed inside the bootloader rather than by changing the flash map. The legacy bootloader used the bundled `stb_sprintf` implementation for both serial diagnostics and bounded status strings. Even with floating-point support disabled, that formatter implements substantially more conversion behavior than this bootloader uses.

Styr now uses `src/bootloader/Format.cpp` instead. The formatter intentionally supports only the active bootloader subset:

- `%%`
- `%s`
- `%d`
- `%u`
- `%x`
- the `l` length modifier used for 32-bit values on STM32F405
- decimal field width and zero padding such as `%02x` and `%08lx`

`stb_sprintf.c` is no longer compiled into the bootloader. The imported source remains under `third_party/embedded/bootloader/` as historical third-party material. Application and simulator formatting are unaffected.

This keeps the established bootloader behavior relevant to update handling, display messages, MD5 verification, flash programming and serial diagnostics while removing generic formatting code that the bootloader does not need.

## Verification

The formatter is covered by the checked-in `TestBootloaderFormat` host regression suite, including version strings, hexadecimal MD5 bytes, padded flash addresses, progress percentages, bounded-buffer truncation, return-value semantics and malformed trailing-percent handling. `toolchain/check_bootloader_formats.py` additionally rejects format conversions outside the deliberately small supported subset.

The authoritative size check remains the STM32 PlatformIO build with the pinned GCC ARM Embedded 6.3.1 toolchain. CI now treats this as a hard gate rather than a known/allowed failure:

```sh
pio run -e bootloader
```

The build must link without changing the 32 KiB region in `toolchain/linker/bootloader.ld`, after which an independent binary-size gate is run:

```sh
python toolchain/check_bootloader_size.py .pio/build/bootloader/bootloader.bin
```

After a successful target build, inspect:

```text
.pio/build/bootloader/bootloader.map
.pio/build/bootloader/bootloader.hex
```

No flash-layout relaxation is acceptable as a substitute for meeting the fixed limit.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
