<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Bootloader verification

The Styr bootloader is a release-critical component. It runs before the application, owns the SD-card update path and writes application flash, while remaining constrained to the fixed 32 KiB flash partition at `0x08000000`. Bootloader changes therefore use multiple independent gates rather than relying on a successful compile alone.

## Verification layers

| Layer | Gate | Purpose |
|---|---|---|
| Host unit tests | `TestBootloaderFormat` | Verifies the compact formatter, padding, truncation, return values, boundary sizes and console output. |
| Host unit tests | `TestBootloaderUpdatePolicy` | Rejects malformed/undersized/oversized `UPDATE.DAT` images and verifies the version-tag/update ABI boundaries, including compile-time size and field-offset assertions for the 32-byte `VersionTag`. |
| Host integration test | `TestBootloaderUpdateFile` | Runs the production `UpdateFile.cpp` against a fake FatFs backend, covering valid parsing, rewind, short reads, mount/open/stat/seek/read failures, invalid sizes and invalid/non-terminated version tags. |
| Host unit tests | `TestBootloaderFlashLayout` | Verifies STM32F405 sector boundaries and that application erasure starts at sector 4 rather than protected sectors 0–3. |
| Host unit tests | `TestBootloaderMd5` | Verifies the MD5 implementation against known vectors and incremental updates. |
| Static contract gate | `toolchain/check_bootloader_formats.py` | Rejects new printf-style conversions that the compact bootloader formatter does not support. |
| Static layout gate | `toolchain/check_bootloader_layout.py` | Cross-checks PlatformIO, linker scripts and bootloader configuration for the fixed bootloader/HW-config/application flash map. |
| Update ABI gate | `toolchain/check_update_abi.py` | Cross-checks the bootloader/application `VersionTag` ABI and, when binaries are supplied, verifies the embedded tag, raw MD5 trailer and byte-identical `UPDATE.DAT` payload. |
| Packaging tests | `toolchain/tests/test_bootloader_tooling.py` | Verifies deterministic `UPDATE.DAT` construction, raw MD5 trailer behavior and the size-gate implementation. |
| ARM build gate | `pio run -e bootloader` | Compiles and links the actual STM32F405 bootloader with the pinned ARM GCC toolchain. The 32 KiB linker region is a hard limit. |
| Binary-size gate | `toolchain/check_bootloader_size.py` | Independently checks the emitted binary against 32,768 bytes and reports remaining headroom. |

## Local verification

Run the bootloader host suite without simulator dependencies:

```sh
cmake -S tests/bootloader -B build/bootloader-host-tests -DCMAKE_BUILD_TYPE=Release
cmake --build build/bootloader-host-tests --config Release
ctest --test-dir build/bootloader-host-tests -C Release --output-on-failure
```

The same bootloader tests are also registered in the full simulator CTest graph, providing a second integration path.

Run the bootloader-specific static and packaging gates from the repository root:

```sh
python toolchain/check_bootloader_layout.py
python toolchain/check_bootloader_formats.py
python toolchain/check_update_abi.py
python -m unittest toolchain.tests.test_bootloader_tooling -v
```

Build the real target and apply the explicit size gate:

```sh
pio run -e bootloader
python toolchain/check_bootloader_size.py .pio/build/bootloader/bootloader.bin
```

The linker gate and explicit binary-size gate are intentionally redundant. A release-critical bootloader must fit the established partition; changing `toolchain/linker/bootloader.ld` or moving the HW-configuration/application regions is not an acceptable way to make the gate pass.

> [!WARNING]
> A passing host test suite does not prove flash programming or SD-card behavior on physical hardware. Hardware-in-the-loop update/recovery tests remain a separate release-verification layer and should be added before Styr reaches a production release process.

## Update-image safety invariants

`UPDATE.DAT` consists of the raw application binary followed by the 16-byte MD5 digest used by the bootloader. The bootloader now rejects files before subtraction if they are too small, rejects payloads that cannot contain the complete version tag, rejects payloads larger than the application partition, and rejects an invalid version-tag magic value and rejects a non-NUL-terminated version name before any update can be accepted or formatted as a string.

The application payload is constrained to `CONFIG_APPLICATION_SIZE` and begins at `CONFIG_APPLICATION_ADDR`. `toolchain/check_update_abi.py` also verifies that the bootloader and application agree on the fixed `VersionTag` magic/offset/layout and can validate a built application plus `UPDATE.DAT` as one byte-for-byte release contract. Flash erase boundaries are tested so the update path cannot treat bootloader/HW-configuration sectors as application-sector starts.

## CI policy

The bootloader is a hard CI job. It does not use `continue-on-error`. Any failure in layout contracts, update ABI compatibility, formatter compatibility, packaging tests, ARM linking or the 32 KiB binary-size gate makes CI fail.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
