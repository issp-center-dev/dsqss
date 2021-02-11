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

import sys

import dsqss
import dsqss.util
import dsqss.algorithm
import dsqss.hamiltonian
import dsqss.displacement
import dsqss.lattice
import dsqss.wavevector
import dsqss.prob_kernel


def dla_alg(
    lat,
    ham: dsqss.hamiltonian.GraphedHamiltonian = None,
    wv: dsqss.wavevector.Wavevector = None,
    latxml: str = None,
    algxml: str = None,
    wvxml: str = None,
    dispxml: str = None,
    prob_kernel: dsqss.prob_kernel.KernelCallBack = dsqss.prob_kernel.suwa_todo,
    ebase_extra: float = 0.0,
    distance_only: bool = False,
):
    if latxml is not None:
        lat.write_xml(latxml)
    if ham is not None and algxml is not None:
        alg = dsqss.algorithm.Algorithm(
            ham, prob_kernel=prob_kernel, ebase_extra=ebase_extra
        )
        alg.write_xml(algxml)
    if wv is not None and wvxml is not None:
        wv.write_xml(wvxml, lat)
    if dispxml is not None:
        disp = dsqss.displacement.Displacement(lat, distance_only=distance_only)
        disp.write_xml(dispxml)


def main():
    import argparse

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
        kernel = dsqss.prob_kernel.suwa_todo
    elif args.kernel == "reversible suwa todo":
        kernel = dsqss.prob_kernel.reversible_suwa_todo
    elif args.kernel == "heat bath":
        kernel = dsqss.prob_kernel.heat_bath
    elif args.kernel == "metropolis":
        kernel = dsqss.prob_kernel.metropolis
    elif args.kernel == "metropolice":
        dsqss.util.ERROR(
            'kernel = "metropolice" is now an invalid option'
            ' because this is a typographic error in old DSQSS.'
            ' Use "metropolis".'
        )
        sys.exit(1)
    else:
        dsqss.util.ERROR(f"unknown kernel: {args.kernel}")
        sys.exit(1)

    lat = dsqss.lattice.Lattice(args.lat)
    ham = (
        dsqss.hamiltonian.GraphedHamiltonian(args.ham, lat) if not args.woalg else None
    )
    wv = None
    if args.kpoint is not None:
        wv = dsqss.wavevector.Wavevector()
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


if __name__ == "__main__":
    main()
