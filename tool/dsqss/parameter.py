#!python
from __future__ import print_function
import codecs

import toml

import dsqss
from dsqss.util import ERROR, INFO
from dsqss.std_lattice import std_lattice
from dsqss.std_model import std_model
from dsqss.hamiltonian import GraphedHamiltonian
from dsqss.algorithm import Algorithm
from dsqss.wavevector import Wavevector
from dsqss.displacement import CF
import dsqss.prob_kernel as pk

def set_default_values(param):
    for name, val in (('npre', 1000),
                      ('ntherm', 1000),
                      ('ndecor', 1000),
                      ('nmcs', 1000),
                      ('nset', 10),
                      ('simulationtime', 0.0),
                      ('ntau', 10),
                      ('taucutoff', None),
                      ('seed', 198212240),
                      ('nvermax', 10000),
                      ('nsegmax', 10000),
                      ('algfile', 'algorithm.xml'),
                      ('latfile', 'lattice.xml'),
                      ('sfinpfile', ''),
                      ('cfinpfile', ''),
                      ('ckinpfile', ''),
                      ('outfile', 'sample.log'),
                      ('sfoutfile', 'sf.dat'),
                      ('cfoutfile', 'cf.dat'),
                      ('ckoutfile', 'ck.dat'),
                      ('kernel', 'suwa todo'),
                     ):
        if name not in param:
            param[name] = val

def check_mandatories(param):
    for name in ('beta',):
        if name not in param:
            ERROR('parameter {0} is not specified.'.format(name))

def dla_pre(param, pfile):
    p = param['parameter']
    set_default_values(p)
    check_mandatories(p)

    lat = std_lattice(param['lattice'])
    lat.write_xml(p['latfile']);

    h = std_model(param['hamiltonian'])
    ham = GraphedHamiltonian(h, lat)

    if p['kernel'] == 'suwa todo':
        kernel = dsqss.prob_kernel.suwa_todo
    elif p['kernel'] == 'reversible suwa todo':
        kernel = dsqss.prob_kernel.reversible_suwa_todo
    elif p['kernel'] == 'heat bath':
        kernel = dsqss.prob_kernel.heat_bath
    elif p['kernel'] == 'metropolice':
        kernel = dsqss.prob_kernel.metropolice
    else:
        ERROR('unknown kernel: {0}'.format(p['kernel']))

    alg = Algorithm(ham, prob_kernel=kernel)
    alg.write_xml(p['algfile'])

    if p['sfinpfile'] != '' or p['ckinpfile'] != '':
        sf = Wavevector()
        sf.generate(param['kpoints'], size=lat.size)
        if p['sfinpfile'] != '':
            sf.write_xml(p['sfinpfile'], lat, p['ntau'], p['taucutoff'])
        if p['ckinpfile'] != p['sfinpfile']:
            sf.write_xml(p['ckinpfile'], lat, p['ntau'], p['taucutoff'])

    if p['cfinpfile'] != '':
        pdisp = param.get('displacement', {})
        cf = CF(lat, pdisp.get('distance_only',False),
                     pdisp.get('origin',None))
        cf.write_xml(p['cfinpfile'], p['ntau'])

    with codecs.open(pfile, 'w', 'utf-8') as f:
        for key in sorted(p.keys()):
            f.write('{0} = {1}\n'.format(key, p[key]))

def main():
    import argparse
    import sys
    parser = argparse.ArgumentParser(
        description='Generate input files for dsqss/dla',
        add_help=True,
    )

    parser.add_argument('input',
                        nargs='?',
                        default=sys.stdin,
                        type=argparse.FileType('r'),
                        help='Input TOML file',
                        )
    parser.add_argument('-p', '--paramfile', dest='pfile',
                        default='param.in',
                        help='Parameter file',
                        )

    args = parser.parse_args()
    if args.input is sys.stdin:
        INFO('waiting for standard input...')

    dla_pre(toml.load(args.input), args.pfile)

if __name__ == '__main__' :
    main()
