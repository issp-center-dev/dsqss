#!/usr/bin/env python
from __future__ import print_function

import argparse
import sys

import dsqss

parser = argparse.ArgumentParser(
    description='Generate lattice file for dsqss',
    add_help=True,
)

parser.add_argument('input', help='Input filename')
parser.add_argument('-o', '--output', dest='out',
                    default='lattice.xml',
                    help='Output filename',
                    )

args = parser.parse_args()

param = dsqss.read_keyvalues(args.input)
dsqss.generate_lattice(param, outputfile=args.out,)
