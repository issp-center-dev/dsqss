import math
from collections import namedtuple
from itertools import product, chain
from copy import deepcopy
import codecs

import numpy as np

from .hamiltonian import keystate, append_matelem
from .prob_kernel import suwa_todo
from .util import ERROR, INFO, tagged

Channel = namedtuple('Channel', ['outleg', 'state', 'prob'])
SiteInitialConfiguration = namedtuple('SiteInitialConfiguration',
        ['state', 'channels'])

InitialConfiguration = namedtuple('InitialConfiguration',
        ['istate', 'fstate', 'direction', 'newstate', 'channels'])

def num_channels(sources, nx):
    nchs = np.zeros(nx,dtype=int)
    for source in sources:
        st = source[0][0]
        nchs[st] += 1
    return nchs

class AlgSite:
    def __init__(self, hamsite, cutoff=1e-10):
        self.id = hamsite.id
        self.N = hamsite.N
        self.vtype = self.id
        self.initialconfigurations = []
        for st in range(self.N):
            channels = []
            bouncew = 1.0
            for source in hamsite.sources:
                if source[0][0] != st:
                    continue
                weight = hamsite.sources[source]**2
                t = source[1][0]
                bouncew -= weight
                channels.append(Channel(0, t, 0.5*weight))
                channels.append(Channel(1, t, 0.5*weight))
            if bouncew > cutoff:
                channels.append(Channel(-1, -1, bouncew))
            self.initialconfigurations.append(SiteInitialConfiguration(st,channels))
        self.vertex = WormVertex(hamsite)

    def write_xml(self, f, level):
        indent = '  '
        f.write(indent*level + '<Site>\n')
        level += 1

        f.write(indent*level + tagged('STYPE', self.id))
        f.write(indent*level + tagged('NumberOfStates', self.N))
        f.write(indent*level + tagged('VertexTypeOfSource', self.vtype))

        for ic in self.initialconfigurations:
            f.write('\n')
            f.write(indent*level + '<InitialConfiguration>\n')
            level += 1
            f.write(indent*level + tagged('State', ic.state))
            f.write(indent*level + tagged('NumberOfChannels', len(ic.channels)))
            for c in ic.channels:
                f.write(indent*level + '<Channel>')
                f.write(' {0} {1} {2:0< 18}'.format(c.outleg, c.state, c.prob))
                f.write(' </Channel>\n')
            level -= 1
            f.write(indent*level + '</InitialConfiguration>\n')

        level -= 1
        f.write(indent*level + '</Site>\n')

class Vertex(object):
    def __init__(self, vtype, category, nbody, ics):
        self.id = vtype
        self.category = category
        self.nbody = nbody
        self.initialconfigurations = ics

    def write_xml(self, f, level):
        indent = '  '
        f.write(indent*level + '<Vertex>\n')
        level += 1
        f.write(indent*level + tagged('VTYPE', self.id))
        f.write(indent*level + tagged('VCATEGORY', self.category))
        f.write(indent*level + tagged('NBODY', self.nbody))
        f.write(indent*level + tagged('NumberOfInitialConfigurations',
                                      len(self.initialconfigurations)))
        for ic in self.initialconfigurations:
            f.write('\n')
            f.write(indent*level + '<InitialConfiguration>\n')
            level+=1

            f.write(indent*level + '<State>')
            for i,final in zip(ic.istate, ic.fstate):
                f.write(' {0} {1}'.format(i,final))
            f.write(' </State>\n')

            f.write(indent*level + tagged('IncomingDirection', ic.direction))
            f.write(indent*level + tagged('NewState', ic.newstate))
            f.write(indent*level + tagged('NumberOfChannels', len(ic.channels)))

            for c in ic.channels:
                f.write(indent*level + tagged('Channel', (c.outleg, c.state, c.prob)))

            level-=1
            f.write(indent*level + '</InitialConfiguration>\n')
        level -= 1
        f.write(indent*level + '</Vertex>\n')

class WormVertex(Vertex):
    def __init__(self, hamsite):
        self.id = hamsite.id
        self.category = 1
        self.nbody = 1
        self.initialconfigurations = []
        for source in hamsite.sources:
            self.initialconfigurations.append(
                InitialConfiguration(source[0],source[1],
                                     0, source[1][0],
                                     [Channel(-1,-1,1.0)]))
            self.initialconfigurations.append(
                InitialConfiguration(source[0],source[1],
                                     1, source[0][0],
                                     [Channel(-1,-1,1.0)]))

class IntVertex(Vertex):
    def __init__(self, vtype, nbody, ics):
        self.id = vtype
        self.category = 2
        self.nbody = nbody
        self.initialconfigurations = ics

class AlgInteraction:
    def __init__(self, hamint, hamsites, prob_kernel=suwa_todo, ebase_extra=0.0):
        self.itype = hamint.itype
        self.vtype = hamint.itype + len(hamsites)
        self.nbody = hamint.nbody
        self.prob_kernel = prob_kernel


        self.hamint = hamint
        self.sites = [hamsites[stype] for stype in hamint.stypes]
        self.sitesources = [deepcopy(site.sources) for site in self.sites]
        self.intelements = deepcopy(hamint.elements)
        self.ebase_negsign = float('inf')

        self.signs = {}

        sumw = 0.0
        maxd = -float('inf')
        maxo = -float('inf')
        for elem,v in self.intelements.items():
            v = -v
            if elem[0] == elem[1]:
                self.intelements[elem] = v
                self.ebase_negsign = min(self.ebase_negsign, v)
                maxd = max(maxd, v)
            else:
                append_matelem(self.signs, istate=elem[0], fstate=elem[1], value=np.sign(v))
                v = abs(v)
                self.intelements[elem] = v
                sumw += v
                maxo = max(maxo, v)
        self.ebase_negsign *= -1.0
        maxd += self.ebase_negsign

        ndiag = 0
        for st in product(*map(lambda site: range(site.N), self.sites)):
            ndiag += 1
            k = (st,st)
            if k in self.intelements:
                self.intelements[k] += self.ebase_negsign
                sumw += self.intelements[k]
            else:
                append_matelem(self.intelements, state=st, value=self.ebase_negsign)
                sumw += self.intelements[k]
        self.ebase_nobounce = max((2*maxo - sumw)/ndiag, 0.0)

        self.ebase_extra = ebase_extra

        if self.ebase_extra + maxd + self.ebase_nobounce == 0.0:
            self.ebase_extra = 0.5*maxo

        for st in product(*map(lambda site: range(site.N), self.sites)):
            k = (st,st)
            self.intelements[k] += self.ebase_nobounce + self.ebase_extra

        self.intelements = {k:v for k,v in self.intelements.items() if v>0.0}

        initialconfigurations = []
        for st in self.intelements:
            for inleg in range(2*self.nbody):
                insite = inleg//2
                for instate in range(self.sites[insite].N):
                    ic = self.make_initialconfiguration(st, inleg, instate)
                    if not ic:
                        continue
                    else:
                        initialconfigurations.append(ic)
        self.vertex = IntVertex(self.vtype, self.nbody, initialconfigurations)

    def make_initialconfiguration(self, states, inleg, instate):
        nbody = self.nbody
        insite = inleg//2
        intau = inleg%2
        oldinstate = states[intau][insite]

        if intau == 0:
            p = self.sitesources[insite].get(
                    keystate(istate=instate, fstate=states[0][insite]), 0.0)
        else:
            p = self.sitesources[insite].get(
                    keystate(istate=states[1][insite], fstate=instate), 0.0)
        if p == 0.0:
            return False

        midstates = [list(states[0]), list(states[1])]
        midstates[intau][insite] = instate

        probs = []
        outlegs = []
        outstates = []
        # import pdb; pdb.set_trace()
        for outleg in range(2*nbody):
            outsite = outleg//2
            outtau = outleg%2
            for outstate in range(self.sites[outsite].N):
                st = deepcopy(midstates)
                oldoutstate = st[outtau][outsite]
                st[outtau][outsite] = outstate
                p = self.intelements.get(keystate(istate=st[0], fstate=st[1]), 0.0)
                if outtau == 0:
                    p *= self.sitesources[outsite].get(keystate(istate=oldoutstate, fstate=outstate), 0.0)
                else:
                    p *= self.sitesources[outsite].get(keystate(istate=outstate, fstate=oldoutstate), 0.0)
                if p == 0.0:
                    continue
                probs.append(p)
                outlegs.append(outleg)
                outstates.append(outstate)
                if inleg == outleg and outstate == oldinstate:
                    incomeindex = len(probs)-1
        if len(probs) == 0:
            return False
        W = self.prob_kernel(probs)[incomeindex,:]
        channels = [Channel(outleg, outstate, p)
                    for outleg, outstate, p in zip(outlegs, outstates, W) if p>0.0]
        return InitialConfiguration(states[0],
                                    states[1],
                                    inleg,
                                    instate,
                                    channels)

    def write_xml(self, f, level):
        indent = '  '
        f.write(indent*level + '<Interaction>\n')
        level += 1
        f.write(indent*level + tagged('ITYPE', self.itype))
        f.write(indent*level + tagged('VTYPE', self.vtype))
        f.write(indent*level + tagged('NBODY', self.nbody))
        f.write(indent*level + tagged('EBASE', self.ebase_negsign + self.ebase_nobounce + self.ebase_extra))
        for st in self.intelements:
            if st[0] != st[1]:
                continue
            f.write(indent*level + tagged('VertexDensity', chain(st[0], [self.intelements[st]])))
        for st,v in self.signs.items():
            f.write(indent*level + tagged('Sign', chain(*zip(st[0],st[1]), [v])))
        level -= 1
        f.write(indent*level + '</Interaction>\n')

class Algorithm:
    def __init__(self, ham, prob_kernel=suwa_todo, ebase_extra=0.0):
        self.name = ham.name
        self.nstypes = ham.nstypes
        self.nitypes = ham.nitypes
        self.nxmax = ham.nxmax
        hamsites = [deepcopy(hamsite) for hamsite in ham.sites]
        for site in hamsites:
            maxprob = 0.0
            nchs = num_channels(site.sources, site.N)
            for source in site.sources:
                st = source[0][0]
                maxprob = max(maxprob, nchs[st]*site.sources[source]**2)
            for source in site.sources:
                site.sources[source] /= math.sqrt(maxprob)

        self.sites = [AlgSite(site) for site in hamsites]
        self.interactions = [AlgInteraction(hamint, hamsites, prob_kernel=prob_kernel, ebase_extra=ebase_extra)
                             for hamint in ham.indeed_interactions]

    def write_xml(self, filename):
        with codecs.open(filename, 'w', 'utf_8') as f:
            indent = '  '
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write('<Algorithm>\n')
            level = 1

            f.write(level*indent + tagged('Comment', self.name))
            f.write(level*indent + '<General>\n')
            level += 1
            f.write(level*indent + tagged('NSTYPE', self.nstypes))
            f.write(level*indent + tagged('NITYPE', self.nitypes))
            f.write(level*indent + tagged('NVTYPE', self.nstypes+self.nitypes))
            f.write(level*indent + tagged('NXMAX', self.nxmax))
            f.write(level*indent + tagged('WDIAG', 0.25))
            level -= 1
            f.write(level*indent + '</General>\n')
            f.write('\n')

            for site in self.sites:
                site.write_xml(f, level)
            f.write('\n')
            for inter in self.interactions:
                inter.write_xml(f, level)
            f.write('\n')
            for site in self.sites:
                site.vertex.write_xml(f, level)
            f.write('\n')
            for inter in self.interactions:
                inter.vertex.write_xml(f, level)
            f.write('\n')

            f.write('</Algorithm>\n')

