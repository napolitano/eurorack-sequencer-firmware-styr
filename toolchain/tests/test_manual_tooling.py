# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT

from __future__ import annotations

import importlib.util
from pathlib import Path
import struct
import tempfile
import unittest
from zipfile import ZipFile

ROOT = Path(__file__).resolve().parents[2]


def load_module(name: str, path: Path):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    assert spec.loader is not None
    spec.loader.exec_module(module)
    return module


build = load_module("styr_manual_build", ROOT / "toolchain" / "manual" / "build_manual.py")
artifact = load_module("styr_manual_artifact", ROOT / "toolchain" / "manual" / "check_manual_artifact.py")
fonts = load_module("styr_manual_fonts", ROOT / "toolchain" / "manual" / "fetch_ubuntu_fonts.py")
screenshots = load_module("styr_manual_screenshots_check", ROOT / "toolchain" / "check_manual_screenshots.py")
regenerator = load_module("styr_manual_screenshot_regenerator", ROOT / "toolchain" / "regenerate_manual_screenshots.py")
architecture = load_module("styr_manual_screenshot_architecture", ROOT / "toolchain" / "check_manual_screenshot_architecture.py")


class ManualToolingTests(unittest.TestCase):
    def test_version_normalization(self):
        self.assertEqual(build.normalize_version("0.1.4"), "0.1.4")
        self.assertEqual(build.normalize_version("v0.1.4"), "0.1.4")
        self.assertEqual(build.normalize_version("0.1.4-rc.1"), "0.1.4-rc.1")

    def test_invalid_version_is_rejected(self):
        for value in ("1.4", "release-0.1.4", "0.1.4/evil", ""):
            with self.subTest(value=value), self.assertRaises(ValueError):
                build.normalize_version(value)

    def test_release_artifact_name(self):
        self.assertEqual(artifact.expected_stem("en", "0.1.4"), "styr-user-manual.en.0.1.4")

    def test_reference_odt_contains_no_font_binaries(self):
        reference = ROOT / "docs" / "manual" / "style" / "styr-reference.odt"
        with ZipFile(reference) as archive:
            embedded = [
                name for name in archive.namelist()
                if Path(name).suffix.lower() in {".ttf", ".otf", ".woff", ".woff2", ".eot"}
            ]
        self.assertEqual(embedded, [])

    def test_pinned_ubuntu_font_source(self):
        self.assertEqual(fonts.VERSION, "0.83")
        self.assertTrue(fonts.SOURCE_URL.startswith("https://archive.ubuntu.com/ubuntu/"))
        self.assertEqual(
            fonts.SOURCE_SHA256,
            "1a96df0ea32bf88278ce9d24c3a31e4c3d31f4397ff7a3af6622dc488b0b830a",
        )

    def test_generated_output_must_be_outside_repository(self):
        with self.assertRaises(RuntimeError):
            build.ensure_outside_repo(ROOT / "dist" / "manual")
        with tempfile.TemporaryDirectory() as raw:
            build.ensure_outside_repo(Path(raw))

    def test_screenshot_asset_validation_accepts_consistent_integer_scaling(self):
        with tempfile.TemporaryDirectory() as raw:
            assets = Path(raw)
            old_assets = screenshots.ASSETS
            screenshots.ASSETS = assets
            try:
                for name, height in (("alpha", 192), ("header", 30)):
                    payload = (
                        b"\x89PNG\r\n\x1a\n"
                        + struct.pack(">I", 13)
                        + b"IHDR"
                        + struct.pack(">II", 768, height)
                        + b"\x08\x00\x00\x00\x00"
                    )
                    (assets / f"{name}.png").write_bytes(payload)
                errors = []
                screenshots.validate_assets({"alpha", "header"}, errors)
                self.assertEqual(errors, [])
            finally:
                screenshots.ASSETS = old_assets

    def test_screenshot_asset_validation_rejects_missing_asset(self):
        with tempfile.TemporaryDirectory() as raw:
            old_assets = screenshots.ASSETS
            screenshots.ASSETS = Path(raw)
            try:
                errors = []
                screenshots.validate_assets({"missing"}, errors)
                self.assertTrue(any("generated screenshot assets are missing" in error for error in errors))
            finally:
                screenshots.ASSETS = old_assets

    def test_screenshot_regenerator_selects_repository_platform_presets(self):
        self.assertEqual(regenerator.default_preset("Linux"), "manual-screenshots")
        self.assertEqual(regenerator.default_preset("Darwin"), "manual-screenshots")
        self.assertEqual(regenerator.default_preset("Windows"), "windows-ucrt64-manual-screenshots")

    def test_manual_screenshot_architecture_is_headless(self):
        self.assertEqual(architecture.static_errors(), [])


if __name__ == "__main__":
    unittest.main()
