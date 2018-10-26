import os
import sys
import shutil
import json
import subprocess as sub
import numpy as np
import scipy.stats as stats

def p_value(X, E, N, Y):
    t = (X-Y)/E
    return 2*stats.t.cdf(-np.abs(t), N-1)

def cleanup(name=''):
    if name != '':
        name = '_{}'.format(name)
    for prefix, suffix in [('hamiltonian', '.xml'),
                           ('algorithm', '.xml'),
                           ('lattice', '.xml'),
                           ('sf', '.xml'),
                           ('cf', '.xml'),
                           ('res', '.dat'),
                           ('sf', '.dat'),
                           ('cf', '.dat'),
                           ('ck', '.dat'),
                           ('cjob.res', '.dat'),
                           ]:
        fname = '{}{}{}'.format(prefix, name, suffix)
        if os.path.exists(fname):
            os.remove(fname)


def genXML(param, BINDIR='.', name=''):
    if name != '':
        name = '_{}'.format(name)

    D = param['D']
    z = 2*D
    if param['Model'] == 'boson':
        M = param['M']
        t = param['t']
        V = param['V']
        U = param['U']/z
        mu = param['mu']/z
        retval = sub.call(['{}/generators/hamgen_B'.format(BINDIR), str(M), str(t), str(V), str(U), str(mu)])
        if retval != 0:
            sys.exit(retval)
    elif param['Model'] == 'spin':
        S = param['S']
        M = int(2*S)
        if M != 2*S:
            print('ERROR: S is NOT an integer or half-integer')
            sys.exit(1)
        Jz = -param['Jz']
        Jxy = -param['Jxy']
        if Jz != Jxy:
            print('ERROR: Jz != Jxy')
            sys.exit(1)
        h = param['h']/z
        retval = sub.call(['{}/generators/hamgen_H'.format(BINDIR), str(M), str(Jz), str(h)])
        if retval != 0:
            sys.exit(retval)
    else:
        print('ERROR: unknown model')
        sys.exit(1)

    retval = sub.call(['{}/generators/dla_alg'.format(BINDIR)])
    if retval != 0:
        sys.exit(retval)
    shutil.move('hamiltonian.xml', 'hamiltonian{}.xml'.format(name))
    shutil.move('algorithm.xml', 'algorithm{}.xml'.format(name))

    L = param['L']
    # beta = param['beta']
    beta = 1.0
    cmds = ['{}/generators/lattgene_C'.format(BINDIR), str(D)]
    cmds.extend(map(str,L))
    cmds.append(str(beta))
    retval = sub.call(cmds)
    if retval != 0:
        sys.exit(retval)
    shutil.move('lattice.xml', 'lattice{}.xml'.format(name))

    ntau = param['ntau']

    cmds = ['{}/generators/sfgene'.format(BINDIR), str(D)]
    cmds.extend(map(str,L))
    cmds.extend([str(ntau), str(ntau), '0'])
    retval = sub.call(cmds)
    if retval != 0:
        sys.exit(retval)
    shutil.move('sf.xml', 'sf{}.xml'.format(name))

    cmds = ['{}/generators/cfgene'.format(BINDIR), str(D)]
    cmds.extend(map(str,L))
    cmds.append(str(ntau))
    retval = sub.call(cmds)
    if retval != 0:
        sys.exit(retval)
    shutil.move('cf.xml', 'cf{}.xml'.format(name))

def geninp(beta, seed, nset=10, nmcs=1000, ntherm=1000, npre=1000, simtime=0.0, name=''):
    if name != '':
        name = '_{}'.format(name)

    with open('qmc{}.inp'.format(name), 'w') as f:
        f.write('runtype = 0\n')
        f.write('beta = {}\n'.format(beta))
        f.write('seed = {}\n'.format(seed))
        f.write('nset = {}\n'.format(nset))
        f.write('nmcs = {}\n'.format(nmcs))
        f.write('ntherm = {}\n'.format(ntherm))
        f.write('npre = {}\n'.format(npre))
        f.write('simulationtime = {}\n'.format(simtime))
        f.write('latfile = lattice{}.xml\n'.format(name))
        f.write('algfile = algorithm{}.xml\n'.format(name))
        f.write('outfile = res{}.dat\n'.format(name))
        f.write('sfinpfile = sf{}.xml\n'.format(name))
        f.write('cfinpfile = cf{}.xml\n'.format(name))
        f.write('ckinpfile = sf{}.xml\n'.format(name))
        f.write('sfoutfile = sf{}.dat\n'.format(name))
        f.write('cfoutfile = cf{}.dat\n'.format(name))
        f.write('ckoutfile = ck{}.dat\n'.format(name))

def run(exename, BINDIR='.', name=''):
    if name != '':
        name = '_{}'.format(name)

    retval = sub.call(['{}/{}'.format(BINDIR,exename), 'qmc{}.inp'.format(name)])
    if retval != 0:
        sys.exit(retval)


def read_result(filename):
    res = {}
    with open(filename) as f:
        for line in f:
            if not line.startswith('R'):
                continue
            words = line.split()
            res[words[1]] = tuple(map(float, words[3:5]))
    return res
