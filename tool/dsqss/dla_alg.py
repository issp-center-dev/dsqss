#!python
from __future__ import print_function

import argparse

from dsqss.lattice import Lattice
from dsqss.hamiltonian import GraphedHamiltonian
from dsqss.algorithm import Algorithm
from dsqss.wavevector import Wavevector

def main():

    parser = argparse.ArgumentParser(
        description='Generate algorithm and lattice XML files for DSQSS/DLA',
        conflict_handler='resolve',
        epilog='Note that the input lattice file is ALWAYS required even if the lattice XML file will not be generated (--without_lattice).',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=True,
    )

    parser.add_argument('-l', '--lattice', dest='lat',
                        default='lattice.dat',
                        help='Input Lattice file',
                        )
    parser.add_argument('-h', '--hamiltonian', dest='ham',
                         default='hamiltonian.toml',
                         help='Input Hamiltonian file',
                         )
    parser.add_argument('-L', '--LatticeXML', dest='latxml',
                        default='lattice.xml',
                        help='Output Lattice XML file',
                        )
    parser.add_argument('-A', '--AlgorithmXML', dest='algxml',
                        default='algorithm.xml',
                        help='Output Algorithm XML file',
                        )
    parser.add_argument('--without_lattice', dest='wolat',
                        action='store_true',
                        help='Never output Lattice XML file',
                        )
    parser.add_argument('--without_algorithm', dest='woalg',
                        action='store_true',
                        help='Never output Algorithm XML file',
                        )
    parser.add_argument('-k', '--kpoint', dest='kpoint',
                        default=None,
                        help='kpoints data file',
                        )
    parser.add_argument('--sf', dest='sf',
                        default='sf.xml',
                        help='kpoints XML file',
                        )

    args = parser.parse_args()

    lat = Lattice(args.lat)
    if not args.wolat:
        lat.write_xml(args.latxml)
    if not args.woalg:
        ham = GraphedHamiltonian(args.ham, lat)
        alg = Algorithm(ham)
        alg.write_xml(args.algxml)
    if args.kpoint is not None:
        wv = Wavevector(lat)
        wv.load(args.kpoint)
        wv.write_xml(args.sf)

if __name__ == '__main__':
    main()
