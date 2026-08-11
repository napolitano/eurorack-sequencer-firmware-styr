<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Bootloader size investigation

## Current state

The PlatformIO bootloader target preserves the historical 32 KiB flash region at `0x08000000`. The target currently links above that limit and therefore remains a migration blocker even though the main `application` environment builds successfully.

Observed migration results:

- initial PlatformIO overflow: 7,936 bytes;
- after restoring bootloader-oriented size flags: 2,800 bytes;
- static `libopencm3` archive linkage and the subsequent library size-profile adjustment did not remove the remaining 2,800 bytes.

The flash boundary must not be enlarged simply to make the port link: the hardware configuration image begins at `0x08008000`, so changing the boundary changes the established flash layout.

## Next diagnostic step

Do not continue with speculative compiler flags. Build the bootloader once and inspect:

```text
.pio/build/bootloader/bootloader.map
```

The map file should be used to rank the largest linked sections and symbols, compare them with the historical bootloader build, and identify the actual source of the remaining size difference.

Useful questions for the map review:

1. Which bootloader translation units dominate `.text` and `.rodata`?
2. Which `libopencm3`, FatFs or formatting symbols are actually retained?
3. Are exception/unwind, RTTI, C library, floating-point or formatting helpers unexpectedly linked?
4. Is section garbage collection operating on all bootloader inputs?
5. Does the current bootloader contain functionality or data absent from the historical 32 KiB image?

Any size fix should be traceable to one of those findings and should preserve update-file compatibility and the existing flash map.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
