import sys
import os
import os.path
import subprocess

if len(sys.argv) > 1:
    bindir = sys.argv[1]
elif 'DSQSS_ROOT' in os.environ:
    bindir = os.path.join(os.environ['DSQSS_ROOT'], 'bin')
    pass
else:
    bindir = ''

name = 'xmzu'
Ms = [1,2]
Ts = [0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.25, 1.5, 1.75, 2.0]

for M in Ms:
    output = open('{}_{}.dat'.format(name,M), 'w')
    for i,T in enumerate(Ts):
        with open('std_{}_{}.in'.format(M,i), 'w') as f:
            f.write('''
    solver = DLA
    model_type = spin
    J = -1
    F = 0.0
    lattice_type = square
    D = 1
    L = 30
    nset = 5
    ntherm = 1000
    ndecor = 1000
    nmcs = 1000
           ''')
            f.write('M = {}\n'.format(M))
            f.write('beta = {}\n'.format(1.0/T))
            f.write('outfile = res_{}_{}.dat\n'.format(M,i))
        cmd = [os.path.join(bindir, 'dsqss_pre.py'), '-p', 'param_{}_{}.in'.format(M,i), '-i', 'std_{}_{}.in'.format(M,i)]
        subprocess.call(cmd)
        cmd = [os.path.join(bindir, 'dla_H'), 'param_{}_{}.in'.format(M,i)]
        subprocess.call(cmd)
        with open('res_{}_{}.dat'.format(M,i)) as f:
            for line in f:
                if not line.startswith('R'):
                    continue
                words = line.split()
                if words[1] == name:
                    output.write('{} {} {}\n'.format(T, words[3], words[4]))

