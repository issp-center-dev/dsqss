#!/usr/bin/env python
from __future__ import print_function

import argparse
import sys

from dsqss.lattice import Lattice

parser = argparse.ArgumentParser(
    description='Generate lattice XML file for dsqss',
    add_help=True,
)

parser.add_argument('input', help='Input filename')
parser.add_argument('-o', '--output', dest='out',
                    default='lattice.xml',
                    help='Output filename',
                    )

args = parser.parse_args()

lat = Lattice(args.input)
lat.write_xml(args.out)
