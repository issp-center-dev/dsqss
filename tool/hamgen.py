#!/usr/bin/env python
from __future__ import print_function
import argparse
import toml

from dsqss.std_model import std_model

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
