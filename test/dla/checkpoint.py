import os
import sys
import shutil
import subprocess as sub
import numpy as np
from common import *

if len(sys.argv) < 2:
    print('usage: python checkpoint.py <path/to/bin>')
    sys.exit(1)

BINDIR = sys.argv[1]

SEED = 19937
simtime = 1.0

def evaluate(name=''):
    if name != '':
        name = '_{0}'.format(name)

    res1 = read_result('res{0}.dat'.format(name))
    SF1 = read_result('sf{0}.dat'.format(name))
    CF1 = read_result('cf{0}.dat'.format(name))
    CK1 = read_result('ck{0}.dat'.format(name))
    res1['SF'] = SF1
    res1['CF'] = CF1
    res1['CK'] = CK1
    res1.pop('time')

    res2 = read_result('res{0}_restarted.dat'.format(name))
    SF2 = read_result('sf{0}_restarted.dat'.format(name))
    CF2 = read_result('cf{0}_restarted.dat'.format(name))
    CK2 = read_result('ck{0}_restarted.dat'.format(name))
    res2['SF'] = SF2
    res2['CF'] = CF2
    res2['CK'] = CK2
    res2.pop('time')

    succ = res1 == res2
    if not succ:
        sys.exit(1)

ID = 'checkpoint'

param = {}

param['lattice'] = {'lattice' : 'hypercubic',
                    'dim' : 1,
                    'L' : [4],
                    }

param['hamiltonian'] = {'model' : 'boson',
                         'M' : 1,
                         't' : 1.0,
                         'V' : 0.0,
                         'U' : 0.0,
                         'mu' : 0.0,
                         }

param['parameter'] = {'beta' : 1.0,
                      'ntau' : 5,
                      'nset' : 100,
                     }

param['kpoints'] = {}

if param['hamiltonian']['model'] == 'boson':
    exename = 'dla_B'
else:
    exename = 'dla_H'

cleanup(ID)
cleanup('{0}_restarted'.format(ID))


geninp(param, SEED, simtime=simtime, name=ID)
run(exename, BINDIR=BINDIR, name=ID)
geninp(param, 2*SEED, simtime='INF', name=ID)
run(exename, BINDIR=BINDIR, name=ID)
for nm in ['res','sf','cf','ck']:
    shutil.move('{0}_{1}.dat'.format(nm,ID), '{0}_{1}_restarted.dat'.format(nm,ID))
os.remove('res_{0}.dat.0.cjob'.format(ID))

geninp(param, SEED, simtime=0.0, name=ID)
run(exename, BINDIR=BINDIR, name=ID)

evaluate(ID)

sys.exit(0)
