#!/usr/bin/env python
from __future__ import print_function
import argparse
import sys
import codecs

import toml

import dsqss
from dsqss.util import ERROR, INFO
from dsqss.std_lattice import std_lattice
from dsqss.std_model import std_model
from dsqss.hamiltonian import GraphedHamiltonian
from dsqss.algorithm import Algorithm

def set_default_values(param):
    for name, val in (('npre', 1000),
                      ('ntherm', 1000),
                      ('ndecor', 1000),
                      ('nmcs', 1000),
                      ('nset', 10),
                      ('simulationtime', 0.0),
                      ('seed', 198212240),
                      ('nvermax', 10000),
                      ('nsegmax', 10000),
                      ('algfile', 'algorithm.xml'),
                      ('latfile', 'lattice.xml'),
                      ('outfile', 'sample.log'),
                      ('sfoutfile', 'sf.dat'),
                      ('cfoutfile', 'cf.dat'),
                      ('ckoutfile', 'ck.dat'),
                      ):
        if name not in param:
            param[name] = val

def check_parameters(param):
    for name in ('beta',):
        if name not in param:
            ERROR('parameter {0} is not specified.'.format(name))

def dla_pre(inp, pfile):
    param = toml.load(inp)
    p = param['parameter']
    set_default_values(p)
    check_parameters(p)
    lat = std_lattice(param['lattice'])
    lat.write_xml(p['latfile']);
    h = std_model(param['hamiltonian'])
    ham = GraphedHamiltonian(h, lat)
    alg = Algorithm(ham)
    alg.write_xml(p['algfile'])

    with codecs.open(pfile, 'w', 'utf-8') as f:
        for key in sorted(p.keys()):
            f.write('{0} = {1}\n'.format(key, p[key]))

if __name__ == '__main__' :
    parser = argparse.ArgumentParser(
        description='Generate input files for dsqss/dla',
        add_help=True,
    )

    parser.add_argument('input',
                        nargs='?',
                        default=sys.stdin,
                        type=argparse.FileType('r'),
                        help='Input file',
                        )
    parser.add_argument('-p', '--paramfile', dest='pfile',
                        default='param.in',
                        help='Parameter file',
                        )

    args = parser.parse_args()
    if args.input is sys.stdin:
        INFO('waiting for standard input...')

    dla_pre(args.input, args.pfile)

