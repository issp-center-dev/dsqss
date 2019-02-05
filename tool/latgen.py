#!/usr/bin/env python
from __future__ import print_function

import argparse
import sys

import toml

from dsqss.std_lattice import std_lattice

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
