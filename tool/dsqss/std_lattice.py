# from .hypercubic import HyperCubicLattice
import codecs

from .lattice import Lattice
from .lattice_factory import honeycomb, hypercubic, kagome, triangular
from .util import ERROR


def std_lattice(param, ret_dict=False):
    if "lattice" in param and param["lattice"] is not str:
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
    inp = toml.load(args.input)
    lat, lattice_dict = std_lattice(inp, True)
    if len(args.out) > 0:
        lat.save_dat(args.out)
    if len(args.toml) > 0:
        with codecs.open(args.toml, "w", "utf-8") as f:
            toml.dump(lattice_dict, f)
    if len(args.gnuplot) > 0:
        lat.write_gnuplot(args.gnuplot)


if __name__ == "__main__":
    main()
