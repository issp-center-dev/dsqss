#!/usr/bin/env python3
"""Golden-file regression tests for the legacy generators (src/dla/generators).

Runs each generator with fixed arguments and compares its output
byte-for-byte with the golden files in data/. Exits nonzero on the
first set of mismatches.

Usage: run_generator_tests.py --bindir <dir with generator binaries>
                              --datadir <dir with golden files>
"""

import argparse
import filecmp
import os
import shutil
import subprocess
import sys
import tempfile

# (golden filename, tool, extra args); {golden} in args refers to a
# previously generated file in the working directory.
CASES = [
    ("lattice_C_1d8.xml", "lattgene_C", ["1", "8"]),
    ("lattice_C_2d44.xml", "lattgene_C", ["2", "4", "4"]),
    ("lattice_T_44.xml", "lattgene_T", ["4", "4"]),
    ("ham_H_M1.xml", "hamgen_H", ["1", "-1.0", "0.0"]),
    ("ham_H_M2.xml", "hamgen_H", ["2", "-1.0", "0.5"]),
    ("ham_B_M2.xml", "hamgen_B", ["2", "1.0", "1.0", "1.0", "0.5"]),
    ("cf_1d8.xml", "cfgene", ["1", "8", "10"]),
    ("sf_1d8.xml", "sfgene", ["1", "8", "10", "10", "0"]),
]

# dla_alg consumes hamiltonian files produced above (or, for
# hand-written inputs such as the mixed-spin case, copied from data/)
ALG_CASES = [
    ("algorithm_H_M1.xml", "ham_H_M1.xml"),
    ("algorithm_H_M2.xml", "ham_H_M2.xml"),
    ("algorithm_B_M2.xml", "ham_B_M2.xml"),
    ("algorithm_mixed.xml", "ham_mixed.xml"),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--bindir", required=True)
    ap.add_argument("--datadir", required=True)
    args = ap.parse_args()

    bindir = os.path.abspath(args.bindir)
    datadir = os.path.abspath(args.datadir)
    workdir = tempfile.mkdtemp(prefix="generator_tests_")
    os.chdir(workdir)

    failures = []

    def check(golden, out):
        if filecmp.cmp(out, os.path.join(datadir, golden), shallow=False):
            print(f"OK   {golden}")
        else:
            print(f"FAIL {golden}: differs from golden")
            failures.append(golden)

    for golden, tool, extra in CASES:
        out = golden
        cmd = [os.path.join(bindir, tool), "-o", out] + extra
        r = subprocess.run(cmd, stdout=subprocess.DEVNULL)
        if r.returncode != 0:
            print(f"FAIL {golden}: {tool} exited with {r.returncode}")
            failures.append(golden)
            continue
        check(golden, out)

    for golden, hamfile in ALG_CASES:
        if not os.path.exists(hamfile):
            shutil.copy(os.path.join(datadir, hamfile), hamfile)
        cmd = [os.path.join(bindir, "dla_alg_old"), hamfile, golden]
        r = subprocess.run(cmd, stdout=subprocess.DEVNULL)
        if r.returncode != 0:
            print(f"FAIL {golden}: dla_alg_old exited with {r.returncode}")
            failures.append(golden)
            continue
        check(golden, golden)

    shutil.rmtree(workdir, ignore_errors=True)
    if failures:
        print(f"{len(failures)} case(s) failed")
        return 1
    print("all generator golden tests passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
