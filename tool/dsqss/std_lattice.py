from dsqss.util import ERROR
from dsqss.hypercubic import HyperCubicLattice

def std_lattice(param):
    if param['lattice'] == 'hypercubic':
        return HyperCubicLattice(param)
    else:
        ERROR('Unknown lattice: param["lattice"] = {0}'.format(param['lattice'].lower()))

def main():
    import argparse
    import sys
    import toml

    parser = argparse.ArgumentParser(
        description='Generate lattice file for DSQSS/DLA',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=True,
    )

    parser.add_argument('input', help='Input filename')
    parser.add_argument('-o', '--output', dest='out',
                        default='lattice.dat',
                        help='Output filename',
                        )

    args = parser.parse_args()
    inp = toml.load(args.input)
    if 'lattice' in inp:
        inp = inp['lattice']
    lat = std_lattice(inp)
    lat.save(args.out)

if __name__ == '__main__':
    main()
