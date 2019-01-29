#!/usr/bin/env python
from __future__ import print_function

import argparse
import sys

import dsqss

parser = argparse.ArgumentParser(
    description='Generate hamiltonian file for dsqss',
    add_help=True,
)

parser.add_argument('input', help='Input filename')
parser.add_argument('-o', '--output', dest='out',
                    default='hamiltonian.xml',
                    help='Output filename',
                    )
parser.add_argument('--nstypes',
                    default=1,
                    type=int,
                    help='the number of site types',
                    )
parser.add_argument('--nitypes',
                    default=1,
                    type=int,
                    help='the number of interaction types',
                    )

args = parser.parse_args()

param = dsqss.read_keyvalues(args.input)
dsqss.generate_hamiltonian(param, outputfile=args.out,
                           nstypes=args.nstypes, nitypes=args.nitypes)
