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

import typing
from typing import Dict, Any

import sys
from dsqss import __version__
from dsqss import lattice
from dsqss import util
from dsqss.lattice_factory import honeycomb, hypercubic, kagome, triangular


def std_lattice(param):
    if "lattice" in param and not isinstance(param["lattice"], str):
        param = param["lattice"]
    if "unitcell" in param:
        latticedict = param
    else:
        latname = param["lattice"].lower()
        if latname == "hypercubic":
            latticedict = hypercubic.generate(param)
        elif latname == "triangular":
            latticedict = triangular.generate(param)
        elif latname == "honeycomb":
            latticedict = honeycomb.generate(param)
        elif latname == "kagome":
            latticedict = kagome.generate(param)
        else:
            util.ERROR(
                'Unknown lattice: param["lattice"] = {0}'.format(
                    param["lattice"].lower()
                )
            )
            sys.exit(1)

    lat = lattice.Lattice()
    lat.load_dict(latticedict)
    return lat, latticedict


def main():
    import argparse
    import toml

    parser = argparse.ArgumentParser(
        description="Generate lattice file for DSQSS/DLA",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=True,
    )

    parser.add_argument("input", help="Input filename")
    parser.add_argument(
        "-o", "--output", dest="out", default="lattice.dat", help="Output data filename"
    )
    parser.add_argument(
        "-t", "--toml", dest="toml", default="", help="Output TOML filename"
    )
    parser.add_argument(
        "-g", "--gnuplot", dest="gnuplot", default="", help="Output Gnuplot filename"
    )
    parser.add_argument("--version", action="version", version=__version__)

    args = parser.parse_args()
    inp: Dict[str, Any]
    try:
        inp = typing.cast(Dict, toml.load(args.input))
        istoml = True
    except toml.TomlDecodeError:
        istoml = False
        inp = {}
        print("Input file seems not to be TOML file.")

    if istoml:
        lat, lattice_dict = std_lattice(inp)
    else:
        lattice_dict: Dict = {}
        lat = lattice.Lattice(args.input)

    if len(args.out) > 0:
        lat.save_dat(args.out)
    if len(args.toml) > 0:
        if istoml:
            with open(args.toml, "w", encoding="utf-8") as f:
                toml.dump(lattice_dict, f)
        else:
            util.ERROR("Cannot generate a lattice TOML file from a lattice data file.")

    if len(args.gnuplot) > 0:
        lat.write_gnuplot(args.gnuplot)


if __name__ == "__main__":
    main()
