import os
import sys
import shutil
import json
import subprocess as sub
import numpy as np
import scipy.stats as stats

from common import *

if len(sys.argv) < 3:
    print('usage: python test.py <path/to/bindir> <jsonfile>')
    sys.exit(1)

test_Green = False

BINDIR = sys.argv[1]
JSONFILE = sys.argv[2]

alpha = 0.001
nset  = 10
SEED  = 19937
NMCS  = 5000
NTHERM = 1000
NPRE = 1000

def evaluate(ref, nset, alpha, name=''):
    if name != '':
        name = '_{}'.format(name)

    res = read_result('res{}.dat'.format(name))
    SF = read_result('sf{}.dat'.format(name))
    res['SF'] = SF

    if test_Green:
        CF = read_result('cf{}.dat'.format(name))
        res['CF'] = CF
        CK = read_result('ck{}.dat'.format(name))
        res['CK'] = CK

    failed = 0
    for name in ref:
        if not name in res:
            continue
        if not name in ['SF', 'CF', 'CK']:
            X,E = res[name]
            Y = ref[name]
            p = p_value(X, E, nset, Y)
            if p < alpha:
                print('NG: {}: X={}, E={}, Y={}, t={}, p={}'.format(name, X, E, Y, (X-Y)/E, p))
                failed += 1
            else:
                print('OK: {}: X={}, E={}, Y={}, t={}, p={}'.format(name, X, E, Y, (X-Y)/E, p))
        else:
            y = np.array(ref[name])
            nk, ntau = y.shape
            for ik in range(nk):
                for it in range(ntau):
                    X,E = res[name]['C{}t{}'.format(ik,it)]
                    Y = y[ik,it]
                    p = p_value(X, E, nset, Y)
                    if p < alpha:
                        print('NG: {}[C{}t{}]: X={}, E={}, Y={}, t={}, p={}'.format(name, ik, it, X, E, Y, (X-Y)/E, p))
                        failed += 1
                    else:
                        print('OK: {}[C{}t{}]: X={}, E={}, Y={}, t={}, p={}'.format(name, ik, it, X, E, Y, (X-Y)/E, p))
    if failed > 0:
        sys.exit(1)

with open(JSONFILE) as f:
    data = json.load(f)

ID = '.'.join(os.path.basename(JSONFILE).split('.')[0:-1])

param = data['Parameter']
ref = data['Result']

if param['Model'] == 'boson':
    exename = 'dla_B'
else:
    exename = 'dla_H'

cleanup(ID)
genXML(param, BINDIR=BINDIR, name=ID)
geninp(param['beta'], SEED, nset=nset, nmcs=NMCS, npre=NPRE, ntherm=NTHERM, name=ID)
run(exename, BINDIR=BINDIR, name=ID)
evaluate(ref, nset, alpha, name=ID)
sys.exit(0)
