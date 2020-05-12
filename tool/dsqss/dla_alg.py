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

import argparse

from .algorithm import Algorithm
from .displacement import Displacement
from .hamiltonian import GraphedHamiltonian
from .lattice import Lattice
from .prob_kernel import (heat_bath, metropolis, reversible_suwa_todo,
                          suwa_todo)
from .util import ERROR
from .wavevector import Wavevector


def dla_alg(
    lat,
    ham=None,
    wv=None,
    latxml=None,
    algxml=None,
    wvxml=None,
    dispxml=None,
    prob_kernel=suwa_todo,
    ebase_extra=0.0,
    distance_only=False,
):
    if latxml is not None:
        lat.write_xml(latxml)
    if ham is not None and algxml is not None:
        alg = Algorithm(ham, prob_kernel=prob_kernel, ebase_extra=ebase_extra)
        alg.write_xml(algxml)
    if wv is not None and wvxml is not None:
        wv.write_xml(wvxml, lat)
    if dispxml is not None:
        disp = Displacement(lat, distance_only=distance_only)
        disp.write_xml(dispxml)


def main():

    parser = argparse.ArgumentParser(
        description="Generate algorithm and lattice XML files for DSQSS/DLA",
        conflict_handler="resolve",
        epilog=(
            "Note that the input lattice file is ALWAYS required "
            "even if the lattice XML file will not be generated "
            "(--without_lattice)."
        ),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=True,
    )

    parser.add_argument(
        "-l", "--lattice", dest="lat", default="lattice.dat", help="Input Lattice file"
    )
    parser.add_argument(
        "-h",
        "--hamiltonian",
        dest="ham",
        default="hamiltonian.toml",
        help="Input Hamiltonian file",
    )
    parser.add_argument(
        "-L",
        "--LatticeXML",
        dest="latxml",
        default="lattice.xml",
        help="Output Lattice XML file",
    )
    parser.add_argument(
        "-A",
        "--AlgorithmXML",
        dest="algxml",
        default="algorithm.xml",
        help="Output Algorithm XML file",
    )
    parser.add_argument(
        "--without_lattice",
        dest="wolat",
        action="store_true",
        help="Never output Lattice XML file",
    )
    parser.add_argument(
        "--without_algorithm",
        dest="woalg",
        action="store_true",
        help="Never output Algorithm XML file",
    )
    parser.add_argument(
        "-k", "--kpoint", dest="kpoint", default=None, help="kpoints data file"
    )
    parser.add_argument("--wv", dest="wv", default="wv.xml", help="wavevector XML file")
    parser.add_argument(
        "--disp", dest="disp", default=None, help="displacement XML file"
    )
    parser.add_argument(
        "--distance-only",
        dest="distance_only",
        action="store_true",
        help="use |r_ij| instead of r_ij",
    )
    parser.add_argument(
        "--kernel",
        dest="kernel",
        default="suwa todo",
        help=(
            "transition kernel of Monte Carlo: "
            '"suwa todo", "reversible suwa todo", '
            '"heat bath", or "metropolis".'
        ),
    )
    parser.add_argument(
        "--extra-shift",
        dest="extra_shift",
        type=float,
        default=0.0,
        help="extra energy shift",
    )

    args = parser.parse_args()

    if args.kernel == "suwa todo":
        kernel = suwa_todo
    elif args.kernel == "reversible suwa todo":
        kernel = reversible_suwa_todo
    elif args.kernel == "heat bath":
        kernel = heat_bath
    elif args.kernel == "metropolis":
        kernel = metropolis
    elif args.kernel == "metropolice":
        ERROR('kenel = "metropolice" is now an invalid option because this is a typographic error in old DSQSS. Use "metropolis".')
    else:
        ERROR("unknown kernel: {0}".format(args.kernel))

    lat = Lattice(args.lat)
    ham = GraphedHamiltonian(args.ham, lat) if not args.woalg else None
    wv = None
    if args.kpoint is not None:
        wv = Wavevector()
        wv.load(args.kpoint)

    dla_alg(
        lat,
        ham=ham,
        wv=wv,
        latxml=args.latxml,
        algxml=args.algxml,
        wvxml=args.wv,
        dispxml=args.disp,
        prob_kernel=kernel,
        ebase_extra=args.extra_shift,
        distance_only=args.distance_only,
    )

    # lat = Lattice(args.lat)
    # if not args.wolat:
    #     lat.write_xml(args.latxml)
    # if not args.woalg:
    #     ham = GraphedHamiltonian(args.ham, lat)
    #     alg = Algorithm(ham, prob_kernel=kernel, ebase_extra=args.extra_shift)
    #     alg.write_xml(args.algxml)
    # if args.kpoint is not None:
    #     wv = Wavevector()
    #     wv.load(args.kpoint)
    #     wv.write_xml(args.wv, lat)
    # if args.disp is not None:
    #     disp = Displacement(lat, distance_only=args.distance_only)
    #     disp.write_xml(args.disp)


if __name__ == "__main__":
    main()
