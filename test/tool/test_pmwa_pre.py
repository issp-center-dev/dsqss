from __future__ import print_function

import filecmp
import platform
import unittest
from subprocess import call

from common import copy_if_not_exists

is_python3 = int(platform.python_version_tuple()[0]) == 3


class Test_DSQSSPRE_DLA(unittest.TestCase):
    def test_dsqss_pre_pmwa_1D_Heisenberg(self):
        copy_if_not_exists("lattgene_P", "../../src/pmwa")
        cmds = ['./pmwa_pre', '-i', 'data/dsqss_pre_pmwa_1D_Heisenberg/std.in']
        self.assertIs(
            0,
            call(cmds),
        )
        path = "data/dsqss_pre_pmwa_1D_Heisenberg/"
        for _file in ["param.in", "lattice.xml"]:
            self.assertIs(True, filecmp.cmp(_file, path + _file))


if __name__ == "__main__":
    unittest.main()
