#!python
from __future__ import print_function

import codecs

import toml

from .algorithm import Algorithm
from .displacement import CF
from .hamiltonian import GraphedHamiltonian
from .prob_kernel import (heat_bath, metropolice, reversible_suwa_todo,
                          suwa_todo)
from .std_lattice import std_lattice
from .std_model import std_model
from .util import ERROR, INFO
from .wavevector import Wavevector


def set_default_values(param):
    for name, val in (
        ("npre", 1000),
        ("ntherm", 1000),
        ("ndecor", 1000),
        ("nmcs", 1000),
        ("nset", 10),
        ("simulationtime", 0.0),
        ("ntau", 10),
        ("taucutoff", None),
        ("seed", 198212240),
        ("nvermax", 10000),
        ("nsegmax", 10000),
        ("algfile", "algorithm.xml"),
        ("latfile", "lattice.xml"),
        ("sfinpfile", ""),
        ("cfinpfile", ""),
        ("ckinpfile", ""),
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
            ERROR("parameter {0} is not specified.".format(name))


def dla_pre(param, pfile):
    p = param["parameter"]
    set_default_values(p)
    check_mandatories(p)

    lat = std_lattice(param["lattice"])
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

    if p["sfinpfile"] != "" or p["ckinpfile"] != "":
        sf = Wavevector()
        sf.generate(param.get("kpoints", {}), size=lat.size)
        if p["sfinpfile"] != "":
            sf.write_xml(p["sfinpfile"], lat, p["ntau"], p["taucutoff"])
        if p["ckinpfile"] != p["sfinpfile"]:
            sf.write_xml(p["ckinpfile"], lat, p["ntau"], p["taucutoff"])

    if p["cfinpfile"] != "":
        pdisp = param.get("displacement", {})
        cf = CF(lat, pdisp.get("distance_only", False), pdisp.get("origin", None))
        cf.write_xml(p["cfinpfile"], p["ntau"])

    with codecs.open(pfile, "w", "utf-8") as f:
        for key in sorted(p.keys()):
            f.write("{0} = {1}\n".format(key, p[key]))


def main():
    import argparse
    import sys

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
