from math import sqrt
import codecs

import dsqss.hamgen
from dsqss.hamgen import Site, Interaction, MatElem

from dsqss.util import ERROR, get_as_list, extend_list

def Splus(S,m):
    return sqrt((S-m)*(S+m+1.0))
def Sminus(S,m):
    return sqrt((S+m)*(S-m+1.0))
def creator_boson(n):
    return sqrt(n+1.0)
def annihilator_boson(n):
    return sqrt(n)

class SpinSite(Site):
    def __init__(self, id, M, D, h):
        '''
        M: 2S+1
        D: Sz^2
        h: -Sz
        '''
        S = 0.5*M
        NX = M+1
        sources = []
        elements = []
        for st in range(NX):
            m = st-0.5*M
            elements.append(MatElem(state = st, value = -h*m + D*m*m))
            if st > 0:
                # annihilator
                sources.append(MatElem(istate = st, fstate = st-1,
                                       value = 0.5*Sminus(S,m)))
            if st < M:
                # creator
                sources.append(MatElem(istate = st, fstate = st+1,
                                       value = 0.5*Splus(S,m)))
        super().__init__(id=id, N=NX, elements=elements, sources=sources)

class BoseSite:
    def __init__(self, id, M, U, mu):
        '''
        M: max n
        U: n_i (n_i - 1)/2
        mu: -n_i
        '''
        M = int(M)
        NX = M+1

        sources = []
        elements = []
        for n in range(NX):
            elements.append(MatElem(state = n, value = -mu*n + 0.5*U*n*(n-1)))
            if n > 0:
                # annihilator
                sources.append(MatElem(istate = n, fstate = n-1,
                                       value = annihilator_boson(n)))
            if n < M:
                # creator
                sources.append(MatElem(istate = n, fstate = n+1,
                                       value = creator_boson(n)))
        super().__init__(id=id, N=NX, elements=elements, sources=sources)


class XXZBond(Interaction):
    def __init__(self, id, M, z, x):
        '''
        M: 2S+1
        z: Sz_1 Sz_2
        x: Sx_1 Sx_2 + Sy_1 Sy_2
        '''

        nbody = 2
        nx = M+1
        Ns = [nx,nx]
        S = 0.5*M
        Sz = [i-0.5*M for i in range(nx)]
        Sp = [Splus(S,m) for m in Sz]
        Sm = [Sminus(S,m) for m in Sz]
        elements = []
        for i in range(nx):
            for j in range(nx):
                # diagonal
                w = -z*Sz[i]*Sz[j]
                if w != 0.0:
                    elements.append(MatElem(state=[i,j], value=w))

                # offdiagnal
                w = -0.5*x*Sp[i]*Sm[j]
                if w != 0.0:
                    elements.append(MatElem(istate=[i,j],
                                            fstate=[i+1,j-1],
                                            value=w))
                w = -0.5*x*Sm[i]*Sp[j]
                if w != 0.0:
                    elements.append(MatElem(istate=[i,j],
                                            fstate=[i-1,j+1],
                                            value=w))
        super().__init__(id=id, nbody=nbody, Ns=Ns, elements=elements)

class BoseBond:
    def __init__(self, id, M, t, V):
        '''
        M: max N
        t: c_1 a_2 + c_2 a_1
        V: n_1 n_2
        '''

        nbody = 2
        nx = M+1
        Ns = [nx,nx]

        N = [1.0*n for n in range(nx)]
        c = [creator_boson(n) for n in N]
        c[-1] = 0.0
        a = [annihilator_boson(n) for n in N]
        elements = []
        for i in range(self.nx):
            for j in range(self.nx):
                # diagonal
                w = V*i*j
                if w != 0.0:
                    elements.append(MatElem(state=[i,j], value=w))
                # offdiagonal
                w = -t*c[i]*a[j]
                if w != 0.0:
                    elements.append(MatElem(istate=[i,j],
                                            fstate=[i+1,j-1],
                                            value=w))
                w = -t*a[i]*c[j]
                if w != 0.0:
                    elements.append(MatElem(istate=[i,j],
                                            fstate=[i-1,j+1],
                                            value=w))
        super().__init__(id=id, nbody=nbody, Ns=Ns, elements=elements)

class HamGen:
    def __init__(self, param):
        if param['model'].lower() == 'spin':
            M = param['M']
            Ds = get_as_list(param, 'D', 0.0)
            hs = get_as_list(param, 'h', 0.0)
            nstypes = max(len(Ds), len(hs))
            extend_list(Ds, nstypes)
            extend_list(hs, nstypes)
            self.sites = [ SpinSite(i,M,D,h)
                           for i,(D,h) in enumerate(zip(Ds, hs)) ]

            Jzs = get_as_list(param, 'Jz', 0.0)
            Jxys = get_as_list(param, 'Jxy', 0.0)
            nitypes = max(len(Jzs), len(Jxys))
            extend_list(Jzs,  nitypes)
            extend_list(Jxys, nitypes)
            self.interactions = [ XXZBond(i, M, Jz, Jx)
                                  for i,(Jz,Jx) in enumerate(zip(Jzs, Jxys))]

            self.name = 'S={0}/2 XXZ model'.format(M)
        elif param['model'].lower() == 'boson':
            M = param['M']
            Us = get_as_list(param, 'U', 0.0)
            mus = get_as_list(param, 'mu', 0.0)
            nstypes = max(len(Us), len(mus))
            extend_list(Us,  nstypes)
            extend_list(mus, nstypes)
            self.sites = [ BoseSite(i,M,U,mu)
                           for i,(U,mu) in enumerate(zip(Us, mus)) ]

            ts = get_as_list(param, 't', 0.0)
            Vs = get_as_list(param, 'V', 0.0)
            nitypes = max(len(ts), len(Vs))
            extend_list(ts, nitypes)
            extend_list(Vs, nitypes)
            self.interactions = [ BoseBond(i, M, t, V)
                                  for i,(t,V) in enumerate(zip(ts, Vs))]
            self.name = 'Bose-Hubbard model'
        else:
            msg = 'Unknown model: {0}\n'.format(param['model'])
            msg += 'following are available: spin, boson'
            raise RuntimeError(msg)

    def to_dict(self):
        return {'name' : self.name,
                'sites' : list(map(lambda x: x.to_dict(), self.sites)),
                'interactions' : list(map(lambda x: x.to_dict(), self.interactions)),
                }
