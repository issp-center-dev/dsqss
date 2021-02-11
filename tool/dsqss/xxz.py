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

from typing import MutableMapping

import numpy as np

from dsqss import util
from dsqss import hamiltonian


def Splus(S: float, m: float) -> float:
    return np.sqrt((S - m) * (S + m + 1.0))


def Sminus(S: float, m: float) -> float:
    return np.sqrt((S + m) * (S - m + 1.0))


class SpinSite(hamiltonian.Site):
    def __init__(self, id: int, M: int, D: float, h: float) -> None:
        """
        M: 2S+1
        D: Sz^2
        h: -Sz
        """
        S = 0.5 * M
        NX = M + 1
        values = []
        elements: hamiltonian.MatElems = {}
        sources: hamiltonian.MatElems = {}
        for st in range(NX):
            m = st - 0.5 * M
            values.append(m)
            hamiltonian.append_matelem(elements, state=st, value=-h * m + D * m * m)
            if st > 0:
                # annihilator
                hamiltonian.append_matelem(
                    sources, istate=st, fstate=st - 1, value=0.5 * Sminus(S, m)
                )
            if st < M:
                # creator
                hamiltonian.append_matelem(
                    sources, istate=st, fstate=st + 1, value=0.5 * Splus(S, m)
                )
        super().__init__(id=id, N=NX, values=values, elements=elements, sources=sources)


class XXZBond(hamiltonian.Interaction):
    def __init__(self, id: int, M: int, z: float, x: float) -> None:
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
        elements: hamiltonian.MatElems = {}
        for i in range(nx):
            for j in range(nx):
                # diagonal
                w = -z * Sz[i] * Sz[j]
                if w != 0.0:
                    hamiltonian.append_matelem(elements, state=[i, j], value=w)

                # offdiagnal
                w = -0.5 * x * Sp[i] * Sm[j]
                if w != 0.0:
                    hamiltonian.append_matelem(
                        elements, istate=[i, j], fstate=[i + 1, j - 1], value=w
                    )
                w = -0.5 * x * Sm[i] * Sp[j]
                if w != 0.0:
                    hamiltonian.append_matelem(
                        elements, istate=[i, j], fstate=[i - 1, j + 1], value=w
                    )
        super().__init__(id=id, nbody=nbody, Ns=Ns, elements=elements)


class XXZ_hamiltonian(hamiltonian.Hamiltonian):
    def __init__(self, param: MutableMapping) -> None:
        M = param["M"]
        Ds = util.get_as_list(param, "D", 0.0)
        hs = util.get_as_list(param, "h", 0.0)
        self.nstypes = max(len(Ds), len(hs))
        util.extend_list(Ds, self.nstypes)
        util.extend_list(hs, self.nstypes)
        self.sites = [SpinSite(i, M, D, h) for i, (D, h) in enumerate(zip(Ds, hs))]

        Jzs = util.get_as_list(param, "Jz", 0.0)
        Jxys = util.get_as_list(param, "Jxy", 0.0)
        self.nitypes = max(len(Jzs), len(Jxys))
        util.extend_list(Jzs, self.nitypes)
        util.extend_list(Jxys, self.nitypes)
        self.interactions = [
            XXZBond(i, M, Jz, Jx) for i, (Jz, Jx) in enumerate(zip(Jzs, Jxys))
        ]

        self.name = f"S={M}/2 XXZ model"
