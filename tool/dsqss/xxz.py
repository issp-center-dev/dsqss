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

from math import sqrt

from .hamiltonian import Hamiltonian, Interaction, Site, append_matelem
from .util import extend_list, get_as_list


def Splus(S, m):
    return sqrt((S - m) * (S + m + 1.0))


def Sminus(S, m):
    return sqrt((S + m) * (S - m + 1.0))


class SpinSite(Site):
    def __init__(self, id, M, D, h):
        """
        M: 2S+1
        D: Sz^2
        h: -Sz
        """
        S = 0.5 * M
        NX = M + 1
        values= []
        elements = {}
        sources = {}
        for st in range(NX):
            m = st - 0.5 * M
            values.append(m)
            append_matelem(elements, state=st, value=-h * m + D * m * m)
            if st > 0:
                # annihilator
                append_matelem(
                    sources, istate=st, fstate=st - 1, value=0.5 * Sminus(S, m)
                )
            if st < M:
                # creator
                append_matelem(
                    sources, istate=st, fstate=st + 1, value=0.5 * Splus(S, m)
                )
        super(SpinSite, self).__init__(id=id, N=NX, values=values,
                                       elements=elements, sources=sources)


class XXZBond(Interaction):
    def __init__(self, id, M, z, x):
        """
        M: 2S+1
        z: Sz_1 Sz_2
        x: Sx_1 Sx_2 + Sy_1 Sy_2
        """

        nbody = 2
        nx = M + 1
        Ns = [nx, nx]
        S = 0.5 * M
        Sz = [i - 0.5 * M for i in range(nx)]
        Sp = [Splus(S, m) for m in Sz]
        Sm = [Sminus(S, m) for m in Sz]
        elements = {}
        for i in range(nx):
            for j in range(nx):
                # diagonal
                w = -z * Sz[i] * Sz[j]
                if w != 0.0:
                    append_matelem(elements, state=[i, j], value=w)

                # offdiagnal
                w = -0.5 * x * Sp[i] * Sm[j]
                if w != 0.0:
                    append_matelem(
                        elements, istate=[i, j], fstate=[i + 1, j - 1], value=w
                    )
                w = -0.5 * x * Sm[i] * Sp[j]
                if w != 0.0:
                    append_matelem(
                        elements, istate=[i, j], fstate=[i - 1, j + 1], value=w
                    )
        super(XXZBond, self).__init__(id=id, nbody=nbody, Ns=Ns, elements=elements)


class XXZ_hamiltonian(Hamiltonian):
    def __init__(self, param):
        M = param["M"]
        Ds = get_as_list(param, "D", 0.0)
        hs = get_as_list(param, "h", 0.0)
        nstypes = max(len(Ds), len(hs))
        extend_list(Ds, nstypes)
        extend_list(hs, nstypes)
        self.sites = [SpinSite(i, M, D, h) for i, (D, h) in enumerate(zip(Ds, hs))]

        Jzs = get_as_list(param, "Jz", 0.0)
        Jxys = get_as_list(param, "Jxy", 0.0)
        nitypes = max(len(Jzs), len(Jxys))
        extend_list(Jzs, nitypes)
        extend_list(Jxys, nitypes)
        self.interactions = [
            XXZBond(i, M, Jz, Jx) for i, (Jz, Jx) in enumerate(zip(Jzs, Jxys))
        ]

        self.name = "S={0}/2 XXZ model".format(M)
