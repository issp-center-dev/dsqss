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

# from .hypercubic import HyperCubicLattice
import codecs

from .lattice import Lattice
from .lattice_factory import honeycomb, hypercubic, kagome, triangular
from .util import ERROR


def std_lattice(param, ret_dict=False):
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
            ERROR(
                'Unknown lattice: param["lattice"] = {0}'.format(
                    param["lattice"].lower()
                )
            )

    lat = Lattice()
    lat.load_dict(latticedict)
    if ret_dict:
        return lat, latticedict
    else:
        return lat


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

    args = parser.parse_args()
    istoml = True
    try:
        inp = toml.load(args.input)
    except toml.TomlDecodeError:
        istoml = False
        print("Input file seems not to be TOML file.")

    if istoml:
        lat, lattice_dict = std_lattice(inp, True)
    else:
        lat = Lattice(args.input)

    if len(args.out) > 0:
        lat.save_dat(args.out)
    if len(args.toml) > 0:
        if istoml:
            with codecs.open(args.toml, "w", "utf-8") as f:
                toml.dump(lattice_dict, f)
        else:
            ERROR("Cannot generate a lattice TOML file from a lattice data file.")

    if len(args.gnuplot) > 0:
        lat.write_gnuplot(args.gnuplot)


if __name__ == "__main__":
    main()
