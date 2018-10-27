from __future__ import print_function
import unittest
import platform
import numpy
import filecmp
from common import * 

is_python3 = int(platform.python_version_tuple()[0]) == 3

class Test_DSQSSPRE_DLA(unittest.TestCase):
        def test_dsqss_pre_dla_1D_Heisenberg(self):
            copy_if_not_exists("hamgen_H", "../../src/dla/generators")
            copy_if_not_exists("dla_alg", "../../src/dla/generators")
            copy_if_not_exists("lattgene_C", "../../src/dla/generators")
            self.assertIs(0, os.system("python dsqss_pre.py -i data/dsqss_pre_dla_1D_Heisenberg/std.in"))
            path = "data/dsqss_pre_dla_1D_Heisenberg/" 
            for _file in ["param.in", "lattice.xml", "hamiltonian.xml", "algorithm.xml"]:
                    self.assertIs(True, filecmp.cmp(_file, path+_file))

        def test_dsqss_pre_dla_2D_Heisenberg(self):
            copy_if_not_exists("hamgen_H", "../../src/dla/generators")
            copy_if_not_exists("dla_alg", "../../src/dla/generators")
            copy_if_not_exists("lattgene_C", "../../src/dla/generators")
            self.assertIs(0, os.system("python dsqss_pre.py -i data/dsqss_pre_dla_2D_Heisenberg/std.in"))
            path = "data/dsqss_pre_dla_2D_Heisenberg/" 
            for _file in ["param.in", "lattice.xml", "hamiltonian.xml", "algorithm.xml"]:
                    self.assertIs(True, filecmp.cmp(_file, path+_file))

            
if __name__ == '__main__':
    unittest.main()

