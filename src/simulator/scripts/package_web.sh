#!/bin/sh
# Purpose: Provides shell automation for the desktop simulator.
# Authors: Simon Kallweit — original PER|FORMER implementation lineage;
#          Axel Napolitano — Styr modifications, integration and modernization
# Copyright: 2017-2018 Simon Kallweit; 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
set -eu

REPO_ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/../../.." && pwd)
BUILD_DIR="$REPO_ROOT/build/simulator/www"
DEPLOY_DIR="$REPO_ROOT/dist/simulator-web"

rm -rf "$DEPLOY_DIR"
mkdir -p "$DEPLOY_DIR"
cp "$BUILD_DIR"/styr_simulator.* "$DEPLOY_DIR"/
