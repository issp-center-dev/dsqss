from __future__ import print_function
import unittest
import platform
import numpy
import filecmp
import os
from common import * 

is_python3 = int(platform.python_version_tuple()[0]) == 3

class Test_PMWA_RESTART(unittest.TestCase):
        def setUp(self):
            self.cwd = os.getcwd()
            self.path_to_exe = "../../../../src/pmwa/pmwa_H"
            self.path_to_test = "/data/pmwa_restart"
        
        def test_pmwa_restart_success(self):
            os.chdir(self.cwd+self.path_to_test)
            self.assertIs(0, os.system(self.path_to_exe + " param.in"))
            self.assertIs(0, os.system(self.path_to_exe + " param.restart.in"))
            os.chdir(self.cwd)

        def test_pmwa_restart_fail(self):
            os.chdir(self.cwd+self.path_to_test)
            self.assertIs(0, os.system(self.path_to_exe + " param.in"))
            self.assertIsNot(0, os.system(self.path_to_exe + " param.restart.fail.in"))
            os.chdir(self.cwd)

            
class Test_PMWA_ANNEAL(unittest.TestCase):
        def setUp(self):
            self.cwd = os.getcwd()
            self.path_to_exe = "../../../../src/pmwa/pmwa_H"
            self.path_to_test = "/data/pmwa_annealing"

        def test_pmwa_anneal_success(self):
            cwd = os.getcwd()
            os.chdir(self.cwd+self.path_to_test)
            self.assertIs(0, os.system(self.path_to_exe + " param.in"))
            self.assertIs(0, os.system(self.path_to_exe + " param.anneal.in"))
            os.chdir(self.cwd)            

        def test_pmwa_anneal_fail(self):
            cwd = os.getcwd()
            os.chdir(self.cwd+self.path_to_test)
            self.assertIs(0, os.system(self.path_to_exe + " param.in"))
            self.assertIsNot(0, os.system(self.path_to_exe + " param.anneal.fail.in"))
            os.chdir(self.cwd)            
            
if __name__ == '__main__':
    unittest.main()

