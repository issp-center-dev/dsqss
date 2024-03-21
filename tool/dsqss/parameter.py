#!python

# DSQSS (Discrete Space Quantum Systems Solver)
# Copyright (C) 2018- The University of Tokyo
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from typing import MutableMapping

from copy import deepcopy

import dsqss
import dsqss.util


def set_default_values(param: MutableMapping):
    for name, val in (
        ("npre", 1000),
        ("ntherm", 1000),
        ("ndecor", 1000),
        ("nmcs", 1000),
        ("nset", 10),
        ("simulationtime", 0.0),
        ("ntau", 10),
        ("seed", 198212240),
        ("nvermax", 100000),
        ("nsegmax", 100000),
        ("algfile", "algorithm.xml"),
        ("latfile", "lattice.xml"),
        ("wvfile", ""),
        ("dispfile", ""),
        ("outfile", "sample.log"),
        ("sfoutfile", "sf.dat"),
        ("cfoutfile", "cf.dat"),
        ("ckoutfile", "ck.dat"),
    ):
        if name not in param:
            param[name] = val


def check_mandatories(param):
    for name in ("beta",):
        if name not in param:
            dsqss.util.ERROR(f"parameter {name} is not specified.")


class Parameter(dict):
    def __init__(self, param):
        super().__init__()
        if "parameter" in param:
            p = deepcopy(param["parameter"])
        else:
            p = deepcopy(param)
        set_default_values(p)
        check_mandatories(p)
        self.update(p)

    def write_param(self, pfile: dsqss.util.Filename):
        with open(pfile, "w", encoding="utf-8") as f:
            for k in sorted(self.keys()):
                v = self.get(k)
                f.write(f"{k} = {v}\n")


def main():
    import argparse
    import sys
    import toml

    parser = argparse.ArgumentParser(
        description="Generate input files for dsqss/dla", add_help=True
    )

    parser.add_argument(
        "input",
        nargs="?",
        default=sys.stdin,
        type=argparse.FileType("r"),
        help="Input TOML file",
    )
    parser.add_argument(
        "-o", "--output", dest="pfile", default="param.in", help="Parameter file"
    )
    parser.add_argument("--version", action="version", version=dsqss.__version__)

    args = parser.parse_args()
    if args.input is sys.stdin:
        dsqss.util.INFO("waiting for standard input...")

    p = Parameter(toml.load(args.input))
    p.write_param(args.pfile)


if __name__ == "__main__":
    main()
