#!/bin/sh
# Purpose: Provides shell automation for the font conversion tooling.
# Author: Axel Napolitano — Styr implementation and modernization
# Copyright: 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
FONT_DIR="$ROOT_DIR/assets/fonts/ttf"
OUTPUT_DIR="$ROOT_DIR/assets/fonts/header"
FONTCONVERT=${FONTCONVERT:-fontconvert}

# Legacy helper retained for the historical TTF -> bitmap-font conversion.
# FONTCONVERT may be overridden with an absolute path when the converter is
# not available on PATH.
"$FONTCONVERT" -s 8 -b 1 "$FONT_DIR/tiny5x5.ttf" "$OUTPUT_DIR/tiny5x5"
"$FONTCONVERT" -s 8 -b 1 -f 16 "$FONT_DIR/ati8x8.ttf" "$OUTPUT_DIR/ati8x8"
