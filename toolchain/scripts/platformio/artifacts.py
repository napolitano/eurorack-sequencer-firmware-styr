# Purpose: Provides Python automation for the PlatformIO build integration.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
"""Generate stable release artifacts and the SD-card UPDATE.DAT image."""

from pathlib import Path
import importlib.util
import os
import shutil
import subprocess

Import("env")  # type: ignore[name-defined]

ROOT = Path(env.subst("$PROJECT_DIR")).resolve()
BUILD = Path(env.subst("$BUILD_DIR"))
DIST = ROOT / "dist" / env.subst("$PIOENV")
PROGNAME = env.subst("$PROGNAME")
ELF = BUILD / f"{PROGNAME}.elf"
BIN = BUILD / f"{PROGNAME}.bin"
HEX = BUILD / f"{PROGNAME}.hex"
SREC = BUILD / f"{PROGNAME}.srec"
LIST = BUILD / f"{PROGNAME}.list"
SIZE = BUILD / f"{PROGNAME}.size"
MAP = BUILD / f"{PROGNAME}.map"
UPDATE = DIST / "UPDATE.DAT"


def load_update_packager():
    module_path = ROOT / "toolchain" / "scripts" / "package_update.py"
    spec = importlib.util.spec_from_file_location("styr_package_update", module_path)
    module = importlib.util.module_from_spec(spec)
    assert spec and spec.loader
    spec.loader.exec_module(module)
    return module.package_update


package_update = load_update_packager()


def process_env(build_env):
    result = dict(os.environ)
    result.update({str(k): str(v) for k, v in build_env.get("ENV", {}).items()})
    return result


def run_tool(build_env, *args, stdout=None):
    command = [build_env.subst(str(args[0])), *[str(arg) for arg in args[1:]]]
    subprocess.run(command, check=True, env=process_env(build_env), stdout=stdout)


def generate_secondary_artifacts(target, source, env):
    """SCons post-action; parameter names must match target/source/env keywords."""
    DIST.mkdir(parents=True, exist_ok=True)

    run_tool(env, "$OBJCOPY", "-O", "ihex", ELF, HEX)
    run_tool(env, "$OBJCOPY", "-O", "srec", ELF, SREC)
    with LIST.open("wb") as out:
        run_tool(env, "$OBJDUMP", "-dhS", ELF, stdout=out)
    with SIZE.open("wb") as out:
        run_tool(env, "$SIZETOOL", "-A", ELF, stdout=out)

    for path in (ELF, BIN, HEX, SREC, LIST, SIZE, MAP):
        if path.exists():
            shutil.copy2(path, DIST / path.name)
    return 0


def package_sd_update(target, source, env):
    """SCons custom-target action using the standard target/source/env contract."""
    if not BIN.exists():
        raise RuntimeError(f"Firmware binary not found: {BIN}")
    size, digest = package_update(BIN, UPDATE)
    print(f"SD update: {UPDATE} ({size} byte firmware + 16 byte MD5)")
    print(f"MD5: {digest.hex()}")
    return 0


env.AddPostAction(str(BIN), generate_secondary_artifacts)

if env.GetProjectOption("custom_styr_sd_update", "no").strip().lower() in ("1", "yes", "true", "on"):
    env.AddCustomTarget(
        name="sd-update",
        dependencies=str(BIN),
        actions=[package_sd_update],
        title="SD Update",
        description="Build dist/application/UPDATE.DAT for installation through the bootloader",
        always_build=True,
    )
