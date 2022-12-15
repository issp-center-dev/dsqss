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

import sys

from dsqss import __version__
from dsqss import util
from dsqss import bosehubbard
from dsqss import xxz
from dsqss import hamiltonian


def std_model(param) -> hamiltonian.Hamiltonian:
    if "hamiltonian" in param:
        param = param["hamiltonian"]
    if "model" in param:
        ham: hamiltonian.Hamiltonian
        if param["model"].lower() == "spin":
            ham = xxz.XXZ_hamiltonian(param)
        elif param["model"].lower() == "boson":
            ham = bosehubbard.BoseHubbard_hamiltonian(param)
        else:
            util.ERROR(
                'Unknown model: param["model"] = {0}'.format(param["model"].lower())
            )
            sys.exit(1)
    else:
        ham = hamiltonian.Hamiltonian(param)
    return ham


def main():
    import argparse
    import toml

    parser = argparse.ArgumentParser(
        description="Generate hamiltonian TOML file for DSQSS/DLA",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=True,
    )

    parser.add_argument("input", help="Input filename")
    parser.add_argument(
        "-o", "--output", dest="out", default="hamiltonian.toml", help="Output filename"
    )
    parser.add_argument("--version", action="version", version=__version__)
    args = parser.parse_args()

    inp = toml.load(args.input)
    if "hamiltonian" in inp:
        inp = inp["hamiltonian"]
    ham = std_model(inp)
    ham.write_toml(args.out)


if __name__ == "__main__":
    main()
