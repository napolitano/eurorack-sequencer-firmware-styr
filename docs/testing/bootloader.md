<!-- SPDX-FileCopyrightText: 2026 Axel Napolitano -->
<!-- SPDX-License-Identifier: CC-BY-NC-4.0 -->

# Bootloader verification

The Styr bootloader is a release-critical component. It runs before the application, owns the SD-card update path and writes application flash, while remaining constrained to the fixed 32 KiB flash partition at `0x08000000`. Bootloader changes therefore use multiple independent gates rather than relying on a successful compile alone.

## Verification layers

| Layer | Gate | Purpose |
|---|---|---|
| PlatformIO + Unity native unit test | `TestBootloaderFormat` | Verifies the compact formatter, padding, truncation, return values, multiple conversions, boundary sizes, malformed/trailing `%` behavior and console CR/LF output. |
| PlatformIO + Unity native unit test | `TestBootloaderUpdatePolicy` | Rejects malformed/undersized/oversized `UPDATE.DAT` images, covers `size_t` extremes, verifies flash-word rounding at partition boundaries and checks the version-tag/update ABI, including compile-time size and field-offset assertions for the 32-byte `VersionTag`. |
| PlatformIO + Unity native integration test | `TestBootloaderUpdateFile` | Runs the production `UpdateFile.cpp` against a fake FatFs backend, covering minimum/maximum valid images, rewind, zero-length and partial payload reads, short metadata reads, mount/open/stat/seek/read failures, bounded error strings, invalid sizes and invalid/non-terminated version tags. |
| PlatformIO + Unity native unit test | `TestBootloaderFlashLayout` | Verifies all STM32F405 sector starts, rejects interior addresses as erase boundaries, checks ordering/flash extent and proves application erasure cannot select protected sectors 0–3. |
| PlatformIO + Unity native unit test | `TestBootloaderMd5` | Verifies RFC 1321 vectors, MD5 padding boundaries around 64 bytes, the bootloader's 1024-byte read boundary and chunking independence. |
| Static contract gate | `toolchain/check_bootloader_formats.py` | Rejects new printf-style conversions that the compact bootloader formatter does not support. |
| Static layout gate | `toolchain/check_bootloader_layout.py` | Cross-checks PlatformIO, linker scripts and bootloader configuration for the fixed bootloader/HW-config/application flash map. |
| Update ABI gate | `toolchain/check_update_abi.py` | Cross-checks the bootloader/application `VersionTag` ABI and, when binaries are supplied, verifies the embedded tag, raw MD5 trailer and byte-identical `UPDATE.DAT` payload. |
| Packaging tests | `toolchain/tests/test_bootloader_tooling.py` | Verifies deterministic `UPDATE.DAT` construction, raw MD5 trailer behavior and the size-gate implementation. |
| Test inventory gate | `toolchain/check_bootloader_test_coverage.py` | Prevents removal of critical Unity scenarios and enforces the current minimum of 43 bootloader regression cases across the five release-critical suites. |
| ARM build gate | `pio run -e bootloader` | Compiles and links the actual STM32F405 bootloader with the pinned ARM GCC toolchain. The 32 KiB linker region is a hard limit. |
| Binary-size gate | `toolchain/check_bootloader_size.py` | Independently checks the emitted binary against 32,768 bytes and reports remaining headroom. |

## Local verification

Run the bootloader product suite through PlatformIO from the repository root:

```sh
pio test -e test_bootloader_native
```

The tests execute on PlatformIO's native host platform using PlatformIO's built-in Unity framework and do not depend on the desktop simulator. Bootloader production support is linked through static archives, so a focused suite such as the MD5 tests does not also pull in `UpdateFile`, `Console`, or FatFs merely because those sources belong to the same bootloader target. They include `unity.h` directly and use normal Unity assertions/lifecycle functions. They are intentionally **not** registered in CTest: PlatformIO is the single owner for tests of bootloader/product code.

On Windows the native test adapter explicitly resolves MSYS2 UCRT64 (default `C:/msys64/ucrt64/bin`) and probes the compiler before building. Set `STYR_NATIVE_TOOLCHAIN_BIN` when GCC is installed elsewhere. This keeps the PlatformIO native test toolchain aligned with the Windows simulator toolchain and avoids silent SCons compiler-launch failures.

Run the bootloader-specific static and packaging gates from the repository root:

```sh
python toolchain/check_bootloader_layout.py
python toolchain/check_bootloader_formats.py
python toolchain/check_update_abi.py
python -m unittest toolchain.tests.test_bootloader_tooling -v
python toolchain/check_bootloader_test_coverage.py
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

The bootloader is a hard CI job. It does not use `continue-on-error`. Any failure in the PlatformIO bootloader test suites, layout contracts, update ABI compatibility, formatter compatibility, packaging tests, ARM linking or the 32 KiB binary-size gate makes CI fail.

From Munich with <img src="../manual/assets/blue-heart.svg" alt="blue heart" width="14">
