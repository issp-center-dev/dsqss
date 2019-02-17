from .hamiltonian import Hamiltonian
from .xxz import XXZ_hamiltonian
from .bosehubbard import BoseHubbard_hamiltonian
from .util import ERROR

def std_model(param):
    if param['model'].lower() == 'spin':
        hamdict = XXZ_hamiltonian(param)
    elif param['model'].lower() == 'boson':
        hamdict = BoseHubbard_hamiltonian(param)
    else:
        ERROR('Unknown model: param["model"] = {0}'.format(param['model'].lower()))

    return hamdict

def main():
    import argparse
    import sys
    import toml
    parser = argparse.ArgumentParser(
        description='Generate hamiltonian TOML file for DSQSS/DLA',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=True,
    )

    parser.add_argument('input', help='Input filename')
    parser.add_argument('-o', '--output', dest='out',
                        default='hamiltonian.toml',
                        help='Output filename',
                        )
    args = parser.parse_args()

    inp = toml.load(args.input)
    if 'hamiltonian' in inp:
        inp = inp['hamiltonian']
    ham = std_model(inp)
    ham.write_toml(args.out)

if __name__ == '__main__':
    main()
