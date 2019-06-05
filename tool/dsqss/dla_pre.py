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

from __future__ import print_function

import codecs

from .algorithm import Algorithm
from .displacement import Displacement
from .hamiltonian import GraphedHamiltonian
from .lattice import Lattice
from .parameter import Parameter
from .prob_kernel import (heat_bath, metropolice, reversible_suwa_todo,
                          suwa_todo)
from .std_lattice import std_lattice
from .std_model import std_model
from .util import ERROR, INFO
from .wavevector import Wavevector


def dla_pre(param, pfile):

    p = Parameter(param)

    lat = std_lattice(param)
    lat.write_xml(p["latfile"])

    h = std_model(param["hamiltonian"])
    ham = GraphedHamiltonian(h, lat)

    palg = param.get("algorithm", {})
    extra_shift = palg.get("extra_shift", 0.0)
    kernel = palg.get("kernel", "suwa todo")

    if kernel == "suwa todo":
        kernel = suwa_todo
    elif kernel == "reversible suwa todo":
        kernel = reversible_suwa_todo
    elif kernel == "heat bath":
        kernel = heat_bath
    elif kernel == "metropolice":
        kernel = metropolice
    else:
        ERROR("unknown kernel: {0}".format(kernel))

    alg = Algorithm(ham, prob_kernel=kernel, ebase_extra=extra_shift)
    alg.write_xml(p["algfile"])

    if p["wvfile"] != "":
        wv = Wavevector()
        wv.generate(param.get("kpoints", {}), size=lat.size)
        wv.write_xml(p["wvfile"], lat)

    if p["dispfile"] != "":
        pdisp = param.get("displacement", {})
        disp = Displacement(lat, pdisp.get("distance_only", False))
        disp.write_xml(p["dispfile"])

    p.write_param(pfile)


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
        "-p", "--paramfile", dest="pfile", default="param.in", help="Parameter file"
    )

    args = parser.parse_args()
    if args.input is sys.stdin:
        INFO("waiting for standard input...")

    dla_pre(toml.load(args.input), args.pfile)


if __name__ == "__main__":
    main()
