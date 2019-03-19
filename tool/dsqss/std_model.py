from .bosehubbard import BoseHubbard_hamiltonian
from .util import ERROR
from .xxz import XXZ_hamiltonian


def std_model(param):
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
