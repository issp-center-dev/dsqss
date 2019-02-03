#!/usr/bin/env python
from __future__ import print_function

import argparse

from dsqss.hamiltonian import Hamiltonian

parser = argparse.ArgumentParser(
    description='Generate hamiltonian XML file for dsqss',
    add_help=True,
)

parser.add_argument('input', help='Input filename')
parser.add_argument('-o', '--output', dest='out',
                    default='hamiltonian.xml',
                    help='Output filename',
                    )
parser.add_argument('-l', '--lattice', dest='lat',
                    default='lattice.dat',
                    help='Lattice file',
                    )

args = parser.parse_args()

ham = Hamiltonian(args.input, args.lat)
ham.write_xml(args.out)
