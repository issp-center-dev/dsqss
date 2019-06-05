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

from .bosehubbard import BoseHubbard_hamiltonian
from .util import ERROR, dictkey_tolower
from .xxz import XXZ_hamiltonian
from .hamiltonian import Hamiltonian


def std_model(param):
    dictkey_tolower(param)
    if "hamiltonian" in param:
        param = param["hamiltonian"]
    if "model" in param:
        if param["model"].lower() == "spin":
            ham = XXZ_hamiltonian(param)
        elif param["model"].lower() == "boson":
            ham = BoseHubbard_hamiltonian(param)
        else:
            ERROR('Unknown model: param["model"] = {0}'.format(param["model"].lower()))
    else:
        ham = Hamiltonian(param)

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
    args = parser.parse_args()

    inp = toml.load(args.input)
    if "hamiltonian" in inp:
        inp = inp["hamiltonian"]
    ham = std_model(inp)
    ham.write_toml(args.out)


if __name__ == "__main__":
    main()
