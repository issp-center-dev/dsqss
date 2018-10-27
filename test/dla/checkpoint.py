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

nset = 100
SEED = 19937
simtime = 1.0

def evaluate(name=''):
    if name != '':
        name = '_{}'.format(name)

    res1 = read_result('res{}.dat'.format(name))
    SF1 = read_result('sf{}.dat'.format(name))
    CF1 = read_result('cf{}.dat'.format(name))
    CK1 = read_result('ck{}.dat'.format(name))
    res1['SF'] = SF1
    res1['CF'] = CF1
    res1['CK'] = CK1
    res1.pop('time')

    res2 = read_result('res{}_restarted.dat'.format(name))
    SF2 = read_result('sf{}_restarted.dat'.format(name))
    CF2 = read_result('cf{}_restarted.dat'.format(name))
    CK2 = read_result('ck{}_restarted.dat'.format(name))
    res2['SF'] = SF2
    res2['CF'] = CF2
    res2['CK'] = CK2
    res2.pop('time')

    succ = res1 == res2
    if not succ:
        sys.exit(1)

ID = 'checkpoint'

param = {'Model' : 'boson',
         'D' : 1,
         'L' : [4],
         'beta' : 1.0,
         'M' : 1,
         't' : 1.0,
         'V' : 0.0,
         'U' : 0.0,
         'mu' : 0.0,
         'ntau' : 5,
         }

beta = param['beta']

if param['Model'] == 'boson':
    exename = 'dla_B'
else:
    exename = 'dla_H'

cleanup(ID)
cleanup('{}_restarted'.format(ID))
genXML(param, BINDIR=BINDIR, name=ID)
geninp(beta, SEED, nset=nset, simtime=simtime, name=ID)
run(exename, BINDIR=BINDIR, name=ID)
geninp(beta, 2*SEED, nset=nset, simtime='INF', name=ID)
run(exename, BINDIR=BINDIR, name=ID)
for nm in ['res','sf','cf','ck']:
    shutil.move('{}_{}.dat'.format(nm,ID), '{}_{}_restarted.dat'.format(nm,ID))
os.remove('res_{}.dat.0.cjob'.format(ID))

geninp(beta, SEED, nset=nset, simtime=0.0, name=ID)
run(exename, BINDIR=BINDIR, name=ID)

evaluate(ID)

sys.exit(0)
