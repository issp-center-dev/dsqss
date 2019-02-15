#!python
from __future__ import print_function

import argparse

from dsqss.util import ERROR
from dsqss.lattice import Lattice
from dsqss.hamiltonian import GraphedHamiltonian
from dsqss.algorithm import Algorithm
from dsqss.wavevector import Wavevector
from dsqss.displacement import CF
import dsqss.prob_kernel

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
                        help='kr XML file',
                        )
    parser.add_argument('--ntau', dest='ntau',
                        default=10,
                        help='number of imaginary time discritization',
                        )
    parser.add_argument('--taucutoff', dest='taucutoff',
                        default=None,
                        help='cutoff of imaginary time distance for the structure factor',
                        )
    parser.add_argument('--cf', dest='cf',
                        default=None,
                        help='displacement XML file',
                        )
    parser.add_argument('--distance-only', dest='distance_only',
                        action='store_true',
                        help='use |r_ij| instead of r_ij',
                        )
    parser.add_argument('--displacement-origin', dest='displacement_origin',
                        default=None,
                        help='origin site (i) of displacement vector r_ij = r_j - r_i. ' +
                             'If None, all pairs of sites <ij> are considered.',
                        )
    parser.add_argument('--kernel', dest='kernel',
                        default='suwa todo',
                        help='transition kernel of Monte Carlo: "suwa todo", "reversible suwa todo", "heat bath", or "metropolice".',
                        )

    args = parser.parse_args()

    if args.kernel == 'suwa todo':
        kernel = dsqss.prob_kernel.suwa_todo
    elif args.kernel == 'reversible suwa todo':
        kernel = dsqss.prob_kernel.reversible_suwa_todo
    elif args.kernel == 'heat bath':
        kernel = dsqss.prob_kernel.heat_bath
    elif args.kernel == 'metropolice':
        kernel = dsqss.prob_kernel.metropolice
    else:
        ERROR('unknown kernel: {0}'.format(args.kernel))

    lat = Lattice(args.lat)
    if not args.wolat:
        lat.write_xml(args.latxml)
    if not args.woalg:
        ham = GraphedHamiltonian(args.ham, lat)
        alg = Algorithm(ham, prob_kernel=kernel)
        alg.write_xml(args.algxml)
    if args.kpoint is not None:
        wv = Wavevector()
        wv.load(args.kpoint)
        wv.write_xml(args.sf, lat, ntau=args.ntau, taucutoff=args.taucutoff)
    if args.cf is not None:
        cf = CF(lat, origin=args.displacement_origin,
                     distance_only=args.distance_only)
        cf.write_xml(args.cf, ntau=args.ntau)


if __name__ == '__main__':
    main()
