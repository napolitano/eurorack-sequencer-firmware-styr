# Purpose: Provides Python automation for the PlatformIO build integration.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Explicit libopencm3 build adapter for the initial Styr migration.

The historical Westlicht fork is fetched from an immutable Git commit on first
use, cached inside the repository build workspace, and compiled explicitly as
part of the PlatformIO firmware graph. It intentionally bypasses PlatformIO's
Library Dependency Finder: STM32 platform and shared sources include
libopencm3 headers directly, so the dependency must be visible globally and
linked deterministically rather than discovered heuristically.
"""

from io import BytesIO
from pathlib import Path
import shutil
import tarfile
import time
import urllib.error
import urllib.request

Import("env")  # type: ignore[name-defined]

EXPECTED_COMMIT = "885e22205b229fa4c6fce9f9df5f288ae6adddee"
CACHE_FORMAT = "f4-generated-nvic-usb-private-v3"
EXPECTED_STAMP = EXPECTED_COMMIT + ":" + CACHE_FORMAT
ARCHIVE_URL = (
    "https://codeload.github.com/westlicht/libopencm3/tar.gz/"
    + EXPECTED_COMMIT
)

# PlatformIO executes library ``extraScript`` files through SCons/SConscript.
# In that execution context Python's ``__file__`` is not guaranteed to be
# defined. Resolve this compatibility package from PlatformIO's stable
# PROJECT_DIR instead; the path is part of Styr's repository layout.
PROJECT_ROOT = Path(env.subst("$PROJECT_DIR")).resolve()
PACKAGE_ROOT = PROJECT_ROOT / "third_party" / "embedded" / "libopencm3"
if not (PACKAGE_ROOT / "legacy-source.json").is_file():
    raise RuntimeError(
        "Cannot locate Styr's libopencm3 compatibility package below "
        f"PlatformIO PROJECT_DIR: {PROJECT_ROOT}"
    )

VENDOR_ROOT = PACKAGE_ROOT / "vendor"
LIB_DIR = VENDOR_ROOT / "lib"
INCLUDE_DIR = VENDOR_ROOT / "include"
STAMP = VENDOR_ROOT / ".styr-libopencm3-revision"
LOCK_DIR = VENDOR_ROOT / ".styr-libopencm3-fetch.lock"
LOCK_TIMEOUT_SECONDS = 90
LOCK_STALE_SECONDS = 300

# Exact source set produced by lib/stm32/f4/Makefile at EXPECTED_COMMIT.
# It is recorded here deliberately so the Styr build never invokes or parses
# libopencm3's historical Make build system.
SOURCES = [
    "stm32/f4/adc.c",
    "stm32/common/adc_common_v1.c",
    "stm32/can.c",
    "stm32/desig.c",
    "stm32/f4/gpio.c",
    "stm32/f4/pwr.c",
    "stm32/f4/rcc.c",
    "stm32/f4/rtc.c",
    "stm32/f4/crypto.c",
    "stm32/common/crc_common_all.c",
    "stm32/common/dac_common_all.c",
    "stm32/common/dma_common_f24.c",
    "stm32/common/gpio_common_all.c",
    "stm32/common/gpio_common_f0234.c",
    "stm32/common/i2c_common_v1.c",
    "stm32/common/iwdg_common_all.c",
    "stm32/common/pwr_common_v1.c",
    "stm32/common/rtc_common_l1f024.c",
    "stm32/common/spi_common_all.c",
    "stm32/common/spi_common_l1f124.c",
    "stm32/common/timer_common_all.c",
    "stm32/common/timer_common_f0234.c",
    "stm32/common/timer_common_f24.c",
    "stm32/common/usart_common_all.c",
    "stm32/common/usart_common_f124.c",
    "stm32/common/flash_common_f234.c",
    "stm32/common/flash_common_f24.c",
    "stm32/common/hash_common_f24.c",
    "stm32/common/crypto_common_f24.c",
    "stm32/common/exti_common_all.c",
    "stm32/common/rcc_common_all.c",
    "stm32/common/rng_common_v1.c",
    "usb/usb.c",
    "usb/usb_standard.c",
    "usb/usb_control.c",
    "usb/usb_fx07_common.c",
    "usb/usb_f107.c",
    "usb/usb_f207.c",
    "usb/usb_msc.c",
    "ethernet/mac.c",
    "ethernet/phy.c",
    "ethernet/mac_stm32fxx7.c",
    "ethernet/phy_ksz8051mll.c",
    "stm32/f4/fmc.c",
    "stm32/f4/ltdc.c",
    "cm3/vector.c",
    "cm3/systick.c",
    "cm3/scb.c",
    "cm3/nvic.c",
    "cm3/assert.c",
    "cm3/sync.c",
    "cm3/dwt.c",
]

# Files not compiled as standalone translation units but textually included by
# lib/cm3/vector.c. The historical libopencm3 build keeps the full source tree
# available, so these files must be present in Styr's selective vendor cache as
# well.
SUPPORT_FILES = [
    "dispatch/vector_chipset.c",
    "dispatch/vector_nvic.c",
    "stm32/f4/vector_chipset.c",
    # Private header included textually by lib/usb/*.c; the historical Make
    # build had the complete lib/usb directory on disk.
    "usb/usb_private.h",
    # Private companion header used by usb_fx07_common.c, usb_f107.c and
    # usb_f207.c. Like usb_private.h it lives below lib/usb in the historical
    # source tree and is therefore not part of the public include/ export.
    "usb/usb_fx07_common.h",
]

# The upstream build generates these files from the F4 interrupt description
# before compiling libopencm3. They are intentionally absent from Git.
GENERATED_FILES = [
    (INCLUDE_DIR, "libopencm3/stm32/f4/nvic.h"),
    (LIB_DIR, "stm32/f4/vector_nvic.c"),
]


def source_tree_complete() -> bool:
    if not STAMP.is_file() or STAMP.read_text(encoding="ascii").strip() != EXPECTED_STAMP:
        return False
    if not (INCLUDE_DIR / "libopencm3" / "stm32" / "gpio.h").is_file():
        return False
    if not all((LIB_DIR / source).is_file() for source in SOURCES):
        return False
    if not all((LIB_DIR / support).is_file() for support in SUPPORT_FILES):
        return False
    return all((root / relative).is_file() for root, relative in GENERATED_FILES)


def archive_member_path(member_name: str, subtree: str):
    # GitHub codeload archives contain one top-level directory whose exact name
    # is deliberately ignored. Only files below the requested subtree are
    # considered. Archive paths always use POSIX separators, including on
    # Windows hosts.
    from pathlib import PurePosixPath

    parts = PurePosixPath(member_name).parts
    if len(parts) < 3 or parts[1] != subtree:
        return None
    relative = PurePosixPath(*parts[2:])
    if relative.is_absolute() or ".." in relative.parts:
        raise RuntimeError(f"Unsafe path in libopencm3 archive: {member_name}")
    return relative


def _resolve_member_bytes(archive, members_by_name, member, depth=0):
    """Return a tar member's contents, materialising links as regular files.

    libopencm3 contains repository symlinks. GitHub preserves those in codeload
    tarballs, but Windows checkouts cannot be assumed to support them. We
    therefore resolve symlinks/hardlinks inside the archive and write ordinary
    files into Styr's dependency cache.
    """
    import posixpath

    if depth > 16:
        raise RuntimeError(f"Too many nested links in libopencm3 archive: {member.name}")

    if member.isfile():
        source = archive.extractfile(member)
        if source is None:
            raise RuntimeError(f"Could not read {member.name} from libopencm3 archive")
        with source:
            return source.read()

    if member.issym():
        target_name = posixpath.normpath(
            posixpath.join(posixpath.dirname(member.name), member.linkname)
        )
    elif member.islnk():
        # Tar hard-link names are normally archive-root relative. Be tolerant
        # of producers that encode them relative to the member directory.
        target_name = posixpath.normpath(member.linkname)
        if target_name not in members_by_name:
            target_name = posixpath.normpath(
                posixpath.join(posixpath.dirname(member.name), member.linkname)
            )
    else:
        raise RuntimeError(
            f"Unsupported libopencm3 archive entry type for {member.name}"
        )

    target = members_by_name.get(target_name)
    if target is None:
        raise RuntimeError(
            f"Broken link in libopencm3 archive: {member.name} -> {member.linkname}"
        )
    return _resolve_member_bytes(archive, members_by_name, target, depth + 1)


def generate_f4_interrupt_files() -> None:
    """Generate the F4 NVIC artifacts that upstream creates from irq.json."""
    import json

    irq_json = INCLUDE_DIR / "libopencm3" / "stm32" / "f4" / "irq.json"
    if not irq_json.is_file():
        raise RuntimeError("Pinned libopencm3 archive is missing STM32F4 irq.json")

    data = json.loads(irq_json.read_text(encoding="utf-8"))
    irqs = data.get("irqs")
    if not isinstance(irqs, list) or not all(isinstance(name, str) for name in irqs):
        raise RuntimeError("Unexpected STM32F4 irq.json format in pinned libopencm3")

    guard = data.get("includeguard")
    if not isinstance(guard, str) or not guard:
        raise RuntimeError("STM32F4 irq.json does not define an include guard")

    definitions = "\n".join(
        f"#define NVIC_{name.upper()}_IRQ {index}" for index, name in enumerate(irqs)
    )
    prototypes = "\n".join(f"void {name.lower()}_isr(void);" for name in irqs)
    pragmas = "\n".join(
        f"#pragma weak {name.lower()}_isr = blocking_handler" for name in irqs
    )
    vector_entries = ", \\\n    ".join(
        f"[NVIC_{name.upper()}_IRQ] = {name.lower()}_isr" for name in irqs
    )

    nvic_text = (
        "/* Generated from the pinned libopencm3 STM32F4 irq.json. */\n"
        f"#ifndef {guard}\n"
        f"#define {guard}\n\n"
        "#include <libopencm3/cm3/nvic.h>\n\n"
        f"{definitions}\n"
        f"#define NVIC_IRQ_COUNT {len(irqs)}\n\n"
        "BEGIN_DECLS\n\n"
        f"{prototypes}\n\n"
        "END_DECLS\n\n"
        f"#endif /* {guard} */\n"
    )
    nvic_path = INCLUDE_DIR / "libopencm3" / "stm32" / "f4" / "nvic.h"
    nvic_path.parent.mkdir(parents=True, exist_ok=True)
    nvic_path.write_text(nvic_text, encoding="utf-8", newline="\n")

    vector_text = (
        "/* Generated from the pinned libopencm3 STM32F4 irq.json. */\n\n"
        f"{pragmas}\n\n"
        "#define IRQ_HANDLERS \\\n    "
        f"{vector_entries}\n"
    )
    vector_path = LIB_DIR / "stm32" / "f4" / "vector_nvic.c"
    vector_path.parent.mkdir(parents=True, exist_ok=True)
    vector_path.write_text(vector_text, encoding="utf-8", newline="\n")


def install_archive(data: bytes) -> None:
    """Install only the F405 source set plus public headers into the cache.

    Do not unpack/move libopencm3's complete repository tree. Apart from being
    unnecessary, that used to make the first build depend on Windows handling
    every foreign architecture's symlinks correctly. A failed or interrupted
    install leaves no valid revision stamp, so the next build safely retries.
    """
    from pathlib import PurePosixPath

    shutil.rmtree(LIB_DIR, ignore_errors=True)
    shutil.rmtree(INCLUDE_DIR, ignore_errors=True)
    STAMP.unlink(missing_ok=True)
    LIB_DIR.mkdir(parents=True, exist_ok=True)
    INCLUDE_DIR.mkdir(parents=True, exist_ok=True)

    try:
        with tarfile.open(fileobj=BytesIO(data), mode="r:gz") as archive:
            members = archive.getmembers()
            members_by_name = {member.name: member for member in members}

            # Determine the codeload archive's single top-level directory.
            roots = {
                PurePosixPath(member.name).parts[0]
                for member in members
                if PurePosixPath(member.name).parts
            }
            if len(roots) != 1:
                raise RuntimeError(
                    "Downloaded libopencm3 archive has an unexpected directory layout"
                )
            archive_root = next(iter(roots))

            # Public headers: materialise regular files and repository links.
            header_count = 0
            for member in members:
                relative = archive_member_path(member.name, "include")
                if relative is None or member.isdir():
                    continue
                if not (member.isfile() or member.issym() or member.islnk()):
                    continue
                destination = INCLUDE_DIR.joinpath(*relative.parts)
                destination.parent.mkdir(parents=True, exist_ok=True)
                destination.write_bytes(
                    _resolve_member_bytes(archive, members_by_name, member)
                )
                header_count += 1

            # Firmware library: only the exact F4 source manifest is installed.
            for source_name in SOURCES:
                member_name = f"{archive_root}/lib/{source_name}"
                member = members_by_name.get(member_name)
                if member is None:
                    raise RuntimeError(
                        f"Pinned libopencm3 archive is missing required source: {source_name}"
                    )
                destination = LIB_DIR.joinpath(*PurePosixPath(source_name).parts)
                destination.parent.mkdir(parents=True, exist_ok=True)
                destination.write_bytes(
                    _resolve_member_bytes(archive, members_by_name, member)
                )

            # cm3/vector.c textually includes these support files. They are not
            # independent objects and therefore do not appear in SOURCES.
            for support_name in SUPPORT_FILES:
                member_name = f"{archive_root}/lib/{support_name}"
                member = members_by_name.get(member_name)
                if member is None:
                    raise RuntimeError(
                        f"Pinned libopencm3 archive is missing support file: {support_name}"
                    )
                destination = LIB_DIR.joinpath(*PurePosixPath(support_name).parts)
                destination.parent.mkdir(parents=True, exist_ok=True)
                destination.write_bytes(
                    _resolve_member_bytes(archive, members_by_name, member)
                )

            if header_count == 0:
                raise RuntimeError(
                    "Downloaded libopencm3 archive did not contain public headers"
                )

        generate_f4_interrupt_files()

        # Stamp only after the complete required tree has been validated.
        missing_sources = [source for source in SOURCES if not (LIB_DIR / source).is_file()]
        missing_support = [support for support in SUPPORT_FILES if not (LIB_DIR / support).is_file()]
        missing_generated = [
            relative for root, relative in GENERATED_FILES if not (root / relative).is_file()
        ]
        if (
            missing_sources
            or missing_support
            or missing_generated
            or not (INCLUDE_DIR / "libopencm3" / "stm32" / "gpio.h").is_file()
        ):
            details = []
            if missing_sources:
                details.append("sources: " + ", ".join(missing_sources))
            if missing_support:
                details.append("support: " + ", ".join(missing_support))
            if missing_generated:
                details.append("generated: " + ", ".join(missing_generated))
            if not (INCLUDE_DIR / "libopencm3" / "stm32" / "gpio.h").is_file():
                details.append("public headers")
            raise RuntimeError("Installed libopencm3 cache is incomplete: " + "; ".join(details))
        STAMP.write_text(EXPECTED_STAMP + "\n", encoding="ascii")
    except Exception:
        # Never leave a half-populated tree that looks reusable on the next run.
        STAMP.unlink(missing_ok=True)
        shutil.rmtree(LIB_DIR, ignore_errors=True)
        shutil.rmtree(INCLUDE_DIR, ignore_errors=True)
        raise

def acquire_fetch_lock() -> bool:
    """Serialize first-use cache population across concurrent PlatformIO jobs.

    VSCodium, ``compiledb`` and a manual build can otherwise enter this library
    script at the same time. Directory creation is atomic on Windows and POSIX.
    A stale lock from an interrupted process is discarded after five minutes.
    """
    VENDOR_ROOT.mkdir(parents=True, exist_ok=True)
    deadline = time.monotonic() + LOCK_TIMEOUT_SECONDS

    while True:
        if source_tree_complete():
            return False
        try:
            LOCK_DIR.mkdir()
            (LOCK_DIR / "created").write_text(str(time.time()), encoding="ascii")
            return True
        except FileExistsError:
            try:
                created = float((LOCK_DIR / "created").read_text(encoding="ascii"))
            except (OSError, ValueError):
                created = time.time()
            if time.time() - created > LOCK_STALE_SECONDS:
                shutil.rmtree(LOCK_DIR, ignore_errors=True)
                continue
            if time.monotonic() >= deadline:
                raise RuntimeError(
                    "Timed out waiting for another PlatformIO process to populate "
                    "the pinned libopencm3 cache. Close duplicate build/index jobs "
                    "and retry."
                )
            time.sleep(0.1)


def fetch_sources() -> None:
    if source_tree_complete():
        return

    owns_lock = acquire_fetch_lock()
    if not owns_lock:
        return

    try:
        # Another process may have completed the cache immediately before we
        # acquired the lock. Avoid a needless network request in that case.
        if source_tree_complete():
            return

        print(f"Fetching pinned Westlicht libopencm3 {EXPECTED_COMMIT[:12]}...")
        request = urllib.request.Request(
            ARCHIVE_URL,
            headers={"User-Agent": "Styr-PlatformIO-build"},
        )
        try:
            with urllib.request.urlopen(request, timeout=60) as response:
                data = response.read()
        except (OSError, urllib.error.URLError) as exc:
            raise RuntimeError(
                "Unable to fetch the pinned libopencm3 source required for the first build. "
                f"URL: {ARCHIVE_URL}. Network access is only required while populating the "
                "local dependency cache."
            ) from exc

        install_archive(data)
        if not source_tree_complete():
            missing = [source for source in SOURCES if not (LIB_DIR / source).is_file()]
            raise RuntimeError(
                "Fetched libopencm3 source is incomplete. Missing: " + ", ".join(missing)
            )
    finally:
        shutil.rmtree(LOCK_DIR, ignore_errors=True)


def libopencm3_size_profile(env, node):
    """Compile libopencm3 with the historical STM32F4 size profile.

    @brief Preserve the compiler contract of the pinned Westlicht library.
    @note The original lib/stm32/f4/Makefile compiles libopencm3 with -Os and
    without Performer's application-level -funroll-loops, -fshort-enums or
    -mthumb-interwork flags.  Reusing those application flags for the library
    increases the fixed 32 KiB bootloader image and also diverges from the
    ABI/build assumptions of the pinned dependency.
    """
    try:
        path = Path(node.get_abspath()).resolve()
        path.relative_to(LIB_DIR.resolve())
    except (AttributeError, ValueError):
        return node

    flags = []
    removed = {
        "-O0",
        "-O1",
        "-O2",
        "-O3",
        "-Og",
        "-Os",
        "-Ofast",
        "-funroll-loops",
        "-fshort-enums",
        "-mthumb-interwork",
    }
    for flag in env.get("CCFLAGS", []):
        if str(flag) not in removed:
            flags.append(flag)

    flags.extend(["-Os", "-fno-common"])
    return env.Object(node, CCFLAGS=flags)

def configure_build() -> None:
    # HWCONFIG is pure data placed at 0x08008000 and has no libopencm3
    # dependency. Avoid a network/cache dependency for those tiny targets.
    if env.subst("$PIOENV").startswith("hwconfig_"):
        return

    fetch_sources()

    # libopencm3 has its own historical compiler profile. Build middleware is
    # the PlatformIO-supported way to override flags for individual source
    # nodes without cloning the PRE construction environment (which would
    # freeze the host compiler before ST STM32 installs arm-none-eabi-gcc).
    env.AddBuildMiddleware(libopencm3_size_profile)

    # The application/runtime sources are added explicitly with BuildSources()
    # from another PRE script. PlatformIO's LDF cannot see that relationship
    # reliably, so export libopencm3's public include root to the project env.
    env.PrependUnique(CPPPATH=[str(INCLUDE_DIR)])

    # Preserve the historical link semantics: Westlicht built the complete F4
    # source set into libopencm3_stm32f4.a and linked that archive into each
    # application. A static archive matters especially for the 32 KiB
    # bootloader because the linker only extracts translation units that satisfy
    # unresolved symbols. Feeding all 52 objects directly into the final link
    # changes that behaviour and can make the bootloader exceed its fixed flash
    # region even with --gc-sections enabled.
    #
    # BuildLibrary() is intentionally called on the *main* PlatformIO
    # construction environment. As with BuildSources(), this lets the ST STM32
    # platform attach arm-none-eabi-gcc after PRE scripts have run; cloning the
    # environment here would freeze the host compiler instead.
    library = env.BuildLibrary(
        str(Path(env.subst("$BUILD_DIR")) / "external" / "libopencm3"),
        str(LIB_DIR),
        [f"+<{source}>" for source in SOURCES],
    )
    env.Prepend(LIBS=[library])


configure_build()
