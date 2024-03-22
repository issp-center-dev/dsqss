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

from dsqss import (
    algorithm,
    displacement,
    hamiltonian,
    parameter,
    prob_kernel,
    std_lattice,
    std_model,
    util,
    wavevector,
    __version__,
)


def dla_pre(param, pfile: str) -> None:

    p = parameter.Parameter(param)

    lat, _ = std_lattice.std_lattice(param)
    lat.write_xml(p["latfile"])

    h = std_model.std_model(param["hamiltonian"])
    ham = hamiltonian.GraphedHamiltonian(h, lat)

    palg = param.get("algorithm", {})
    extra_shift = palg.get("extra_shift", 0.0)
    kernel = palg.get("kernel", "suwa todo")

    if kernel == "suwa todo":
        kernel = prob_kernel.suwa_todo
    elif kernel == "reversible suwa todo":
        kernel = prob_kernel.reversible_suwa_todo
    elif kernel == "heat bath":
        kernel = prob_kernel.heat_bath
    elif kernel == "metropolis":
        kernel = prob_kernel.metropolis
    elif kernel == "metropolice":
        util.ERROR(
            'kernel = "metropolice" is now an invalid option'
            " because this is a typographic error in old DSQSS."
            ' Use "metropolis".'
        )
        sys.exit(1)
    else:
        util.ERROR("unknown kernel: {0}".format(kernel))
        sys.exit(1)

    alg = algorithm.Algorithm(ham, prob_kernel=kernel, ebase_extra=extra_shift)
    alg.write_xml(p["algfile"])

    if p["wvfile"] != "":
        wv = wavevector.Wavevector()
        wv.generate(param.get("kpoints", {}), size=lat.size)
        wv.write_xml(p["wvfile"], lat)

    if p["dispfile"] != "":
        pdisp = param.get("displacement", {})
        disp = displacement.Displacement(lat, pdisp.get("distance_only", False))
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
    parser.add_argument("--version", action="version", version=__version__)

    args = parser.parse_args()
    if args.input is sys.stdin:
        util.INFO("waiting for standard input...")

    dla_pre(toml.load(args.input), args.pfile)


if __name__ == "__main__":
    main()
