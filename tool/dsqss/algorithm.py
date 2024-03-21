# DSQSS (Discrete Space Quantum Systems Solver)
# Copyright (C) 2018- The University of Tokyo
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from typing import List, Iterable, TextIO, Sequence, Optional, NamedTuple, Tuple

import math
from copy import deepcopy
from itertools import chain, product

import numpy as np

import dsqss
from dsqss import prob_kernel
import dsqss.hamiltonian
import dsqss.util
from dsqss.util import tagged


class Channel(NamedTuple):
    outleg: int
    state: int
    prob: float


class SiteInitialConfiguration(NamedTuple):
    state: int
    channels: List[Channel]


class InitialConfiguration(NamedTuple):
    istate: Tuple[int, ...]
    fstate: Tuple[int, ...]
    direction: int
    newstate: int
    channels: List[Channel]


def num_channels(sources: dsqss.hamiltonian.MatElems, nx: int) -> np.ndarray:
    nchs = np.zeros(nx, dtype=int)
    for source in sources:
        st = source[0][0]
        nchs[st] += 1
    return nchs


class AlgSite:
    id: int
    N: int
    values: np.ndarray
    vtype: int
    initialconfigurations: List[SiteInitialConfiguration]

    def __init__(self, hamsite: dsqss.hamiltonian.Site, cutoff=1e-10) -> None:
        self.id = hamsite.id
        self.N = hamsite.N
        self.values = np.array(hamsite.values)
        self.vtype = self.id
        self.initialconfigurations = []
        for st in range(self.N):
            channels: List[Channel] = []
            bouncew = 1.0
            for source in hamsite.sources:
                if source[0][0] != st:
                    continue
                weight = hamsite.sources[source] ** 2
                t = source[1][0]
                bouncew -= weight
                channels.append(Channel(outleg=0, state=t, prob=0.5 * weight))
                channels.append(Channel(outleg=1, state=t, prob=0.5 * weight))
            if bouncew > cutoff:
                channels.append(Channel(outleg=-1, state=-1, prob=bouncew))
            self.initialconfigurations.append(SiteInitialConfiguration(st, channels))
        self.vertex = WormVertex(hamsite)

    def write_xml(self, f: TextIO, level: int) -> None:
        indent = "  "
        f.write(indent * level + "<Site>\n")
        level += 1

        f.write(indent * level + tagged("STYPE", self.id))
        f.write(indent * level + tagged("NumberOfStates", self.N))
        f.write(indent * level + tagged("LocalStates", self.values))
        f.write(indent * level + tagged("VertexTypeOfSource", self.vtype))

        for ic in self.initialconfigurations:
            f.write("\n")
            f.write(indent * level + "<InitialConfiguration>\n")
            level += 1
            f.write(indent * level + tagged("State", ic.state))
            f.write(indent * level + tagged("NumberOfChannels", len(ic.channels)))
            for c in ic.channels:
                f.write(indent * level + "<Channel>")
                f.write(" {0} {1} {2:0< 18}".format(c.outleg, c.state, c.prob))
                f.write(" </Channel>\n")
            level -= 1
            f.write(indent * level + "</InitialConfiguration>\n")

        level -= 1
        f.write(indent * level + "</Site>\n")


class Vertex(object):
    id: int
    category: int
    nbody: int
    initialconfigurations: List[InitialConfiguration]

    def __init__(
        self, vtype: int, category: int, nbody: int, ics: Iterable[InitialConfiguration]
    ):
        self.id = vtype
        self.category = category
        self.nbody = nbody
        self.initialconfigurations = list(ics)

    def write_xml(self, f: TextIO, level: int) -> None:
        indent = "  "
        f.write(indent * level + "<Vertex>\n")
        level += 1
        f.write(indent * level + tagged("VTYPE", self.id))
        f.write(indent * level + tagged("VCATEGORY", self.category))
        f.write(indent * level + tagged("NBODY", self.nbody))
        f.write(
            indent * level
            + tagged("NumberOfInitialConfigurations", len(self.initialconfigurations))
        )
        for ic in self.initialconfigurations:
            f.write("\n")
            f.write(indent * level + "<InitialConfiguration>\n")
            level += 1

            f.write(indent * level + "<State>")
            for i, final in zip(ic.istate, ic.fstate):
                f.write(" {0} {1}".format(i, final))
            f.write(" </State>\n")

            f.write(indent * level + tagged("IncomingDirection", ic.direction))
            f.write(indent * level + tagged("NewState", ic.newstate))
            f.write(indent * level + tagged("NumberOfChannels", len(ic.channels)))

            for c in ic.channels:
                f.write(indent * level + tagged("Channel", (c.outleg, c.state, c.prob)))

            level -= 1
            f.write(indent * level + "</InitialConfiguration>\n")
        level -= 1
        f.write(indent * level + "</Vertex>\n")


class WormVertex(Vertex):
    id: int
    category: int
    nbody: int
    initialconfigurations: List[InitialConfiguration]

    def __init__(self, hamsite: dsqss.hamiltonian.Site) -> None:
        self.id = hamsite.id
        self.category = 1
        self.nbody = 1
        self.initialconfigurations = []
        for source in hamsite.sources:
            self.initialconfigurations.append(
                InitialConfiguration(
                    source.initial,
                    source.final,
                    0,
                    source.final[0],
                    [Channel(outleg=-1, state=-1, prob=1.0)],
                )
            )
            self.initialconfigurations.append(
                InitialConfiguration(
                    source.initial,
                    source.final,
                    1,
                    source.initial[0],
                    [Channel(outleg=-1, state=-1, prob=1.0)],
                )
            )


class IntVertex(Vertex):
    id: int
    category: int
    nbody: int
    initialconfigurations: List[InitialConfiguration]

    def __init__(
        self, vtype: int, nbody: int, ics: Iterable[InitialConfiguration]
    ) -> None:
        self.id = vtype
        self.category = 2
        self.nbody = nbody
        self.initialconfigurations = list(ics)


class AlgInteraction:
    itype: int
    vtype: int
    nbody: int
    kernel: prob_kernel.KernelCallBack
    hamint: dsqss.hamiltonian.IndeedInteraction
    sites: List[dsqss.hamiltonian.Site]
    sitesources: List[dsqss.hamiltonian.MatElems]
    intelements: dsqss.hamiltonian.MatElems
    ebase_negsign: float
    signs: dsqss.hamiltonian.MatElems

    def __init__(
        self,
        hamint: dsqss.hamiltonian.IndeedInteraction,
        hamsites: Sequence[dsqss.hamiltonian.Site],
        kernel: prob_kernel.KernelCallBack = prob_kernel.suwa_todo,
        ebase_extra: float = 0.0,
    ):
        self.itype = hamint.itype
        self.vtype = hamint.itype + len(hamsites)
        self.nbody = hamint.nbody
        self.kernel = kernel

        self.hamint = hamint
        self.sites = [hamsites[stype] for stype in hamint.stypes]
        self.sitesources = [deepcopy(site.sources) for site in self.sites]
        self.intelements = deepcopy(hamint.elements)
        self.ebase_negsign = float("inf")

        self.signs = {}

        sumw = 0.0
        maxd = -float("inf")
        maxo = -float("inf")
        for elem, v in self.intelements.items():
            v = -v
            if elem[0] == elem[1]:
                self.intelements[elem] = v
                self.ebase_negsign = min(self.ebase_negsign, v)
                maxd = max(maxd, v)
            else:
                dsqss.hamiltonian.append_matelem(
                    self.signs, istate=elem[0], fstate=elem[1], value=np.sign(v)
                )
                v = abs(v)
                self.intelements[elem] = v
                sumw += v
                maxo = max(maxo, v)
        self.ebase_negsign *= -1.0
        maxd += self.ebase_negsign

        ndiag = 0
        for st in product(*map(lambda site: range(site.N), self.sites)):
            ndiag += 1
            k = dsqss.hamiltonian.keystate(st, st)
            if k in self.intelements:
                self.intelements[k] += self.ebase_negsign
                sumw += self.intelements[k]
            else:
                dsqss.hamiltonian.append_matelem(
                    self.intelements, state=st, value=self.ebase_negsign
                )
                sumw += self.intelements[k]
        self.ebase_nobounce = max((2 * maxo - sumw) / ndiag, 0.0)

        self.ebase_extra = ebase_extra

        if self.ebase_extra + maxd + self.ebase_nobounce == 0.0:
            self.ebase_extra = 0.5 * maxo

        for st in product(*map(lambda site: range(site.N), self.sites)):
            k = dsqss.hamiltonian.keystate(st, st)
            self.intelements[k] += self.ebase_nobounce + self.ebase_extra

        self.intelements = {k: v for k, v in self.intelements.items() if v > 0.0}

        initialconfigurations = []
        for st in self.intelements:
            for inleg in range(2 * self.nbody):
                insite = inleg // 2
                for instate in range(self.sites[insite].N):
                    ic = self.make_initialconfiguration(st, inleg, instate)
                    if ic is None:
                        continue
                    else:
                        initialconfigurations.append(ic)
        self.vertex = IntVertex(self.vtype, self.nbody, initialconfigurations)

    def make_initialconfiguration(
        self,
        states: dsqss.hamiltonian.States,
        inleg: int,
        instate: int,
    ) -> Optional[InitialConfiguration]:
        nbody = self.nbody
        insite = inleg // 2
        intau = inleg % 2
        oldinstate = states[intau][insite]

        if intau == 0:
            p = self.sitesources[insite].get(
                dsqss.hamiltonian.keystate(istate=instate, fstate=states[0][insite]),
                0.0,
            )
        else:
            p = self.sitesources[insite].get(
                dsqss.hamiltonian.keystate(istate=states[1][insite], fstate=instate),
                0.0,
            )
        if p == 0.0:
            return None

        midstates = [list(states[0]), list(states[1])]
        midstates[intau][insite] = instate

        incomeindex = 100000  # very large integer
        probs = []
        outlegs = []
        outstates = []
        for outleg in range(2 * nbody):
            outsite = outleg // 2
            outtau = outleg % 2
            for outstate in range(self.sites[outsite].N):
                st = deepcopy(midstates)
                oldoutstate = st[outtau][outsite]
                st[outtau][outsite] = outstate
                p = self.intelements.get(
                    dsqss.hamiltonian.keystate(istate=st[0], fstate=st[1]), 0.0
                )
                if outtau == 0:
                    p *= self.sitesources[outsite].get(
                        dsqss.hamiltonian.keystate(istate=oldoutstate, fstate=outstate),
                        0.0,
                    )
                else:
                    p *= self.sitesources[outsite].get(
                        dsqss.hamiltonian.keystate(istate=outstate, fstate=oldoutstate),
                        0.0,
                    )
                if p == 0.0:
                    continue
                probs.append(p)
                outlegs.append(outleg)
                outstates.append(outstate)
                if inleg == outleg and outstate == oldinstate:
                    incomeindex = len(probs) - 1
        if len(probs) == 0:
            return None
        W = self.kernel(probs)[incomeindex, :]
        channels = [
            Channel(outleg=outleg, state=outstate, prob=p)
            for outleg, outstate, p in zip(outlegs, outstates, W)
            if p > 0.0
        ]
        return InitialConfiguration(
            states.initial, states.final, inleg, instate, channels
        )

    def write_xml(self, f, level):
        indent = "  "
        f.write(indent * level + "<Interaction>\n")
        level += 1
        f.write(indent * level + tagged("ITYPE", self.itype))
        f.write(indent * level + tagged("VTYPE", self.vtype))
        f.write(indent * level + tagged("NBODY", self.nbody))
        f.write(
            indent * level
            + tagged(
                "EBASE", self.ebase_negsign + self.ebase_nobounce + self.ebase_extra
            )
        )
        for st in self.intelements:
            if st[0] != st[1]:
                continue
            f.write(
                indent * level
                + tagged("VertexDensity", chain(st[0], [self.intelements[st]]))
            )
        for st, v in self.signs.items():
            f.write(indent * level + "<Sign> ")
            for ini, fin in zip(st[0], st[1]):
                f.write("{0} {1} ".format(ini, fin))
            f.write("{0} </Sign>\n".format(v))
        level -= 1
        f.write(indent * level + "</Interaction>\n")


class Algorithm:
    name: str
    nstypes: int
    nitypes: int
    nxmax: int

    def __init__(
        self,
        ham: dsqss.hamiltonian.GraphedHamiltonian,
        prob_kernel: prob_kernel.KernelCallBack = prob_kernel.suwa_todo,
        ebase_extra: float = 0.0,
    ):
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
                maxprob = max(maxprob, nchs[st] * site.sources[source] ** 2)
            for source in site.sources:
                site.sources[source] /= math.sqrt(maxprob)

        self.sites = [AlgSite(site) for site in hamsites]
        self.interactions = [
            AlgInteraction(
                hamint, hamsites, kernel=prob_kernel, ebase_extra=ebase_extra
            )
            for hamint in ham.indeed_interactions
        ]

    def write_xml(self, filename: dsqss.util.Filename) -> None:
        with open(filename, "w", encoding="utf_8") as f:
            indent = "  "
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<Algorithm>\n")
            level = 1

            f.write(level * indent + tagged("Comment", self.name))
            f.write(level * indent + "<General>\n")
            level += 1
            f.write(level * indent + tagged("NSTYPE", self.nstypes))
            f.write(level * indent + tagged("NITYPE", self.nitypes))
            f.write(level * indent + tagged("NVTYPE", self.nstypes + self.nitypes))
            f.write(level * indent + tagged("NXMAX", self.nxmax))
            f.write(level * indent + tagged("WDIAG", 0.25))
            level -= 1
            f.write(level * indent + "</General>\n")
            f.write("\n")

            for site in self.sites:
                site.write_xml(f, level)
            f.write("\n")
            for inter in self.interactions:
                inter.write_xml(f, level)
            f.write("\n")
            for site in self.sites:
                site.vertex.write_xml(f, level)
            f.write("\n")
            for inter in self.interactions:
                inter.vertex.write_xml(f, level)
            f.write("\n")

            f.write("</Algorithm>\n")
