# Purpose: Provides Python automation for the simulator test support.
# Authors: Simon Kallweit — original PER|FORMER implementation lineage;
#          Axel Napolitano — Styr modifications, integration and modernization
# Copyright: 2017-2018 Simon Kallweit; 2026 Axel Napolitano
# License: MIT; see LICENSES/MIT.txt.
#
# SPDX-FileCopyrightText: 2017-2018 Simon Kallweit
# SPDX-FileCopyrightText: 2026 Axel Napolitano
# SPDX-License-Identifier: MIT
import os
import unittest

if __name__ == "__main__":
    loader = unittest.TestLoader()
    tests = loader.discover(os.path.dirname(__file__), "*.py")
    # print(tests)
    runner = unittest.runner.TextTestRunner(verbosity=2)
    result = runner.run(tests)
    # print(loader)
    # unittest.main(testLoader=loader)
