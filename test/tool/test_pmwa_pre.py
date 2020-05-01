from __future__ import print_function

import filecmp
import platform
import unittest
import os

from subprocess import call

from common import copy_if_not_exists

is_python3 = int(platform.python_version_tuple()[0]) == 3


class Test_DSQSSPRE_PMWA(unittest.TestCase):
    def test_dsqss_pre_pmwa_1D_Heisenberg(self):
        copy_if_not_exists("lattgene_P", "../../src/pmwa")
        for p in ("dsqss_pre_pmwa_1D_Heisenberg", "dsqss_pre_pmwa_2D_Heisenberg"):
            cmds = ['./pmwa_pre', os.path.join('data', p, 'std.in')]
            self.assertIs(
                0,
                call(cmds),
            )
            path = os.path.join("data", p)
            for _file in ["param.in", "lattice.xml"]:
                self.assertIs(True, filecmp.cmp(_file, os.path.join(path, _file)))


if __name__ == "__main__":
    unittest.main()
