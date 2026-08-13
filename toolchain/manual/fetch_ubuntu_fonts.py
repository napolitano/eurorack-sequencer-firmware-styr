#!/usr/bin/env python3
# Purpose: Fetches the pinned Ubuntu font family for ephemeral manual builds.
# Author: Axel Napolitano — Styr documentation toolchain
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

"""Fetch and optionally install the pinned Ubuntu classic fonts for a build.

No font binary is written to the Styr repository. The source tarball is fetched
from the official Ubuntu archive and verified before extraction.
"""

from __future__ import annotations

import argparse
import hashlib
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile
import tempfile
import urllib.request

ROOT = Path(__file__).resolve().parents[2]
VERSION = "0.83"
SOURCE_PACKAGE_VERSION = "0.83-6ubuntu2"
SOURCE_URL = (
    "https://archive.ubuntu.com/ubuntu/pool/universe/f/fonts-ubuntu-classic/"
    f"fonts-ubuntu-classic_{VERSION}.orig.tar.gz"
)
SOURCE_SHA256 = "1a96df0ea32bf88278ce9d24c3a31e4c3d31f4397ff7a3af6622dc488b0b830a"


def refuse_repo_path(path: Path) -> None:
    try:
        path.resolve().relative_to(ROOT.resolve())
    except ValueError:
        return
    raise RuntimeError(f"font destination must be outside the repository: {path}")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--destination",
        type=Path,
        default=Path.home() / ".local" / "share" / "fonts" / f"styr-ubuntu-classic-{VERSION}",
        help="Font installation/extraction directory; must remain outside the repository",
    )
    parser.add_argument("--no-cache-refresh", action="store_true", help="Do not run fc-cache after extraction")
    args = parser.parse_args()

    try:
        destination = args.destination.expanduser().resolve()
        refuse_repo_path(destination)
        destination.mkdir(parents=True, exist_ok=True)

        with tempfile.TemporaryDirectory(prefix="styr-ubuntu-fonts-") as raw:
            archive = Path(raw) / "ubuntu-fonts.tar.gz"
            print(f"Fetching Ubuntu font family {VERSION} from the official Ubuntu archive")
            urllib.request.urlretrieve(SOURCE_URL, archive)
            actual = sha256(archive)
            if actual != SOURCE_SHA256:
                raise RuntimeError(f"Ubuntu font archive checksum mismatch: {actual}")

            with tarfile.open(archive, "r:gz") as tar:
                members = [member for member in tar.getmembers() if member.isfile() and member.name.lower().endswith(".ttf")]
                if not members:
                    raise RuntimeError("verified Ubuntu font archive contains no TTF files")
                for member in members:
                    source = tar.extractfile(member)
                    if source is None:
                        continue
                    target = destination / Path(member.name).name
                    with target.open("wb") as handle:
                        shutil.copyfileobj(source, handle)

        if not args.no_cache_refresh:
            fc_cache = shutil.which("fc-cache")
            if not fc_cache:
                raise RuntimeError("fc-cache is required to activate fetched fonts")
            subprocess.run([fc_cache, "-f", str(destination)], check=True)

        print(f"Ubuntu font family installed for this user/build: {destination}")
        print(f"Source SHA-256: {SOURCE_SHA256}")
        return 0
    except (OSError, RuntimeError, tarfile.TarError, urllib.error.URLError, subprocess.CalledProcessError) as error:
        print(f"Ubuntu font preparation FAILED: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
