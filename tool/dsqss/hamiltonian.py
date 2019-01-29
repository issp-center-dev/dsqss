from math import sqrt
import codecs
from dsqss import parse_list

class SpinSite:
    def __init__(self, M):
        M = int(M)
        self.nx = M+1
        S = 0.5*M
        ms = [i-0.5*M for i in range(self.nx)]
        self.creator = [0.5*sqrt((S-m)*(S+m+1.0)) for m in ms]
        self.annihilator = [0.5*sqrt((S+m)*(S-m+1.0)) for m in ms]

class BoseSite:
    def __init__(self, M):
        M = int(M)
        self.nx = M+1
        self.creator = [0.5*sqrt(n+1.0) for n in range(self.nx)]
        self.annihilator = [0.5*sqrt(n) for n in range(self.nx)]

class XXZBond:
    def __init__(self, M, z, x, D1, D2, F1, F2,):
        '''
        M: 2S+1
        z: Sz_1 Sz_2
        x: Sx_1 Sx_2 + Sy_1 Sy_2
        Di: (Sz_i)^2 (per bond)
        Fi: -Sz_i (per bond)
        '''

        self.nbody = 2
        self.stypes = [0,0]
        self.weights = []
        self.initial_states = []
        self.final_states = []
        self.nx = M+1
        S = 0.5*M
        Sz = [i-0.5*M for i in range(self.nx)]
        Sp = [sqrt((S-m)*(S+m+1)) for m in Sz]
        Sm = [sqrt((S+m)*(S-m+1)) for m in Sz]
        for i in range(self.nx):
            for j in range(self.nx):
                # diagonal
                w = -z*Sz[i]*Sz[j]
                w += D1*Sz[i]*Sz[i] + D2*Sz[j]*Sz[j]
                w -= F1*Sz[i] + F2*Sz[j]
                if w != 0.0:
                    self.weights.append(w)
                    self.initial_states.append((i,j))
                    self.final_states.append((i,j))
                # offdiagnal
                w = -0.5*x*Sp[i]*Sm[j]
                if w != 0.0:
                    self.weights.append(w)
                    self.initial_states.append((i, j))
                    self.final_states.append((i+1, j-1))
                w = -0.5*x*Sm[i]*Sp[j]
                if w != 0.0:
                    self.weights.append(w)
                    self.initial_states.append((i, j))
                    self.final_states.append((i-1, j+1))

class BoseBond:
    def __init__(self, M, t, V, U1, U2, F1, F2):
        '''
        M: max N
        t: c_1 a_2 + c_2 a_1
        V: n_1 n_2
        Ui: n_i (n_i - 1)/2 (per bond)
        Fi: -n_i (per bond)
        '''

        self.nbody = 2
        self.stypes = [0,0]
        self.weights = []
        self.initial_states = []
        self.final_states = []
        self.nx = M+1
        N = [1.0*n for n in range(self.nx)]
        c = [sqrt(n+1) for n in N]
        c[-1] = 0.0
        a = [sqrt(n) for n in N]
        for i in range(self.nx):
            for j in range(self.nx):
                # diagonal
                w = 0.5*U1*i*(i-1) + 0.5*U2*j*(j-1)
                w -= F1*i + F2*j
                w += V*i*j
                if w != 0.0:
                    self.weights.append(w)
                    self.initial_states.append((i,j))
                    self.final_states.append((i,j))
                # offdiagonal
                w = -t*c[i]*a[j]
                if w != 0.0:
                    self.weights.append(w)
                    self.initial_states.append((i,j))
                    self.final_states.append((i+1,j-1))
                w = -t*a[i]*c[j]
                if w != 0.0:
                    self.weights.append(w)
                    self.initial_states.append((i,j))
                    self.final_states.append((i-1,j+1))

class Hamiltonian:
    def __init__(self, param, nstypes=1, nitypes=1):
        if param['model'].lower() == 'spin':
            M = int(param['m'])
            self.sites = [SpinSite(M)]
            self.interactions = []
            Jzs = parse_list(param['jz'], nitypes, float)
            Jxs = parse_list(param['jx'], nitypes, float)
            Fs = parse_list(param['f'], nitypes, float)
            Gs = parse_list(param['g'], nitypes, float)
            for z,x,f,g in zip(Jzs, Jxs, Fs, Gs):
                self.interactions.append(XXZBond(M, z, x, g, g, f, f))
            self.name = 'S={0}/2 XXZ model'.format(M)
        elif param['model'].lower() == 'boson':
            M = int(param['m'])
            self.sites = [BoseSite(M)]
            self.interactions = []
            ts = parse_list(param['t'], nitypes, float)
            Vs = parse_list(param['v'], nitypes, float)
            Us = parse_list(param['u'], nitypes, float)
            Fs = parse_list(param['f'], nitypes, float)
            for t, v, u, f in zip(ts, Vs, Us, Fs):
                self.interactions.append(BoseBond(M, t, v, u, u, f, f))
            self.name = 'Bose-Hubbard model'
            pass
        else:
            msg = 'Unknown model: {0}\n'.format(param['model'])
            msg += 'following are available: spin, boson'
            raise RuntimeError(msg)
    
    def write_xml(self, filename):
        with codecs.open(filename, 'w', 'utf_8') as f:
            nxmax = 0
            for site in self.sites:
                nxmax = max(site.nx, nxmax)

            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write('<Hamiltonian>\n')
            indent = '  '
            level = 1
            
            f.write(indent*level + '<General>\n')
            level += 1
            f.write(indent*level + '<Comment>' + self.name + '</Comment>\n')
            f.write(indent*level + '<NSTYPE> {0} </NSTYPE>\n'.format(len(self.sites)))
            f.write(indent*level + '<NITYPE> {0} </NITYPE>\n'.format(len(self.interactions)))
            f.write(indent*level + '<NXMAX> {0} </NXMAX>\n'.format(nxmax))
            level -= 1
            f.write(indent*level + '</General>\n')
            f.write('\n')

            for st, site in enumerate(self.sites):
                f.write(indent*level + '<Site>\n')
                level += 1
                f.write(indent*level + '<STYPE> {0} </STYPE>\n'.format(st))
                f.write(indent*level + '<TTYPE> {0} </TTYPE>\n'.format(0))
                f.write(indent*level + '<NX> {0} </NX>\n'.format(site.nx))
                level -= 1
                f.write(indent*level + '</Site>\n')
                f.write('\n')

            for st, site in enumerate(self.sites):
                f.write(indent*level + '<Source>\n')
                level += 1
                f.write(indent*level + '<STYPE> {0} </STYPE>\n'.format(st))
                f.write(indent*level + '<TTYPE> {0} </TTYPE>\n'.format(0))
                for i in range(site.nx):
                    w = site.creator[i]
                    if w != 0.0:
                        f.write(indent*level + '<Weight> {0} {1} '.format(i,i+1))
                        f.write('{0:0< 18} '.format(w))
                        f.write('</Weight>\n')
                    w = site.annihilator[i]
                    if w != 0.0:
                        f.write(indent*level + '<Weight> {0} {1} '.format(i,i-1))
                        f.write('{0:0< 18} '.format(w))
                        f.write('</Weight>\n')
                level -= 1
                f.write(indent*level + '</Source>\n')
                f.write('\n')

            for itype,interaction in enumerate(self.interactions):
                nbody = interaction.nbody
                f.write(indent*level + '<Interaction>\n')
                level += 1
                f.write(indent*level + '<ITYPE> {0} </ITYPE>\n'.format(itype))
                f.write(indent*level + '<NBODY> {0} </NBODY>\n'.format(nbody))
                f.write(indent*level + '<STYPE> ')
                for st in interaction.stypes:
                    f.write('{0} '.format(st))
                f.write('</STYPE>\n')

                for w,ini,fin in zip(interaction.weights,
                                    interaction.initial_states,
                                    interaction.final_states):
                    f.write(indent*level + '<Weight> ')
                    for i,j in zip(ini,fin):
                        f.write('{0} {1} '.format(i,j))
                    f.write('{0:0< 18} '.format(w))
                    f.write('</Weight>\n')

                level -= 1
                f.write(indent*level + '</Interaction>\n')
                f.write('\n')

            level -= 1
            f.write('</Hamiltonian>\n')

def generate_hamiltonian(params,
                         nstypes=1,
                         nitypes=1,
                         outputfile='hamiltonian.xml'):
    ham = Hamiltonian(params, nstypes=nstypes, nitypes=nitypes)
    ham.write_xml(outputfile)
