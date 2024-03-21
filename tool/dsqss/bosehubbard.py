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


def creator_boson(n: float) -> float:
    return np.sqrt(n + 1.0)


def annihilator_boson(n: float) -> float:
    return np.sqrt(n)


class BoseSite(hamiltonian.Site):
    def __init__(self, id: int, M: int, U: float, mu: float) -> None:
        """
        M: max n
        U: n_i (n_i - 1)/2
        mu: -n_i
        """
        M = int(M)
        NX = M + 1

        values = []
        sources: hamiltonian.MatElems = {}
        elements: hamiltonian.MatElems = {}
        for n in range(NX):
            value = -mu * n + 0.5 * U * n * (n - 1)
            values.append(n)
            hamiltonian.append_matelem(elements, state=n, value=value)
            if n > 0:
                # annihilator
                hamiltonian.append_matelem(
                    sources, istate=n, fstate=n - 1, value=annihilator_boson(n)
                )
            if n < M:
                # creator
                value = creator_boson(n)
                hamiltonian.append_matelem(sources, istate=n, fstate=n + 1, value=value)
        super().__init__(id=id, N=NX, values=values, elements=elements, sources=sources)


class BoseBond(hamiltonian.Interaction):
    def __init__(self, id: int, M: int, t: float, V: float) -> None:
        """
        M: max N
        t: c_1 a_2 + c_2 a_1
        V: n_1 n_2
        """

        nbody = 2
        nx = M + 1
        Ns = [nx, nx]

        N = [1.0 * n for n in range(nx)]
        c = [creator_boson(n) for n in N]
        c[-1] = 0.0
        a = [annihilator_boson(n) for n in N]
        elements: hamiltonian.MatElems = {}
        for i in range(nx):
            for j in range(nx):
                # diagonal
                w = V * i * j
                if w != 0.0:
                    hamiltonian.append_matelem(elements, state=[i, j], value=w)
                # offdiagonal
                w = -t * c[i] * a[j]
                if w != 0.0:
                    hamiltonian.append_matelem(
                        elements, istate=[i, j], fstate=[i + 1, j - 1], value=w
                    )
                w = -t * a[i] * c[j]
                if w != 0.0:
                    hamiltonian.append_matelem(
                        elements, istate=[i, j], fstate=[i - 1, j + 1], value=w
                    )
        super().__init__(id=id, nbody=nbody, Ns=Ns, elements=elements)


class BoseHubbard_hamiltonian(hamiltonian.Hamiltonian):
    def __init__(self, param: MutableMapping) -> None:
        M = param["M"]
        Us = util.get_as_list(param, "U", 0.0)
        mus = util.get_as_list(param, "mu", 0.0)
        self.nstypes = max(len(Us), len(mus))
        util.extend_list(Us, self.nstypes)
        util.extend_list(mus, self.nstypes)
        self.sites = [BoseSite(i, M, U, mu) for i, (U, mu) in enumerate(zip(Us, mus))]

        ts = util.get_as_list(param, "t", 0.0)
        Vs = util.get_as_list(param, "V", 0.0)
        self.nitypes = max(len(ts), len(Vs))
        util.extend_list(ts, self.nitypes)
        util.extend_list(Vs, self.nitypes)
        self.interactions = [
            BoseBond(i, M, t, V) for i, (t, V) in enumerate(zip(ts, Vs))
        ]
        self.name = "Bose-Hubbard model"
