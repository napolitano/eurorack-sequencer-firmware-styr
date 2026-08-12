# Purpose: Provides Python automation for the simulator test support.
# Authors: Simon Kallweit — original PER|FORMER implementation lineage;
#          Axel Napolitano — Styr modifications, integration and modernization
# Copyright: 2017-2018 Simon Kallweit; 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
import sys
from pathlib import Path

repo_root = Path(__file__).resolve().parents[4]
module_path = repo_root / "build" / "simulator" / "release" / "python"
sys.path.append(str(module_path))

from testsim import *

from .controller import Controller

import unittest

class UiTest(unittest.TestCase):
    def setUp(self):
        self.env = Environment()
        self.controller = Controller(self.env.simulator)
        self.controller.wait(3000)

    def tearDown(self):
        self.controller = None
        self.env = None
