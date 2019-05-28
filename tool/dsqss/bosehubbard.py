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


def creator_boson(n):
    return sqrt(n + 1.0)


def annihilator_boson(n):
    return sqrt(n)


class BoseSite(Site):
    def __init__(self, id, M, U, mu):
        """
        M: max n
        U: n_i (n_i - 1)/2
        mu: -n_i
        """
        M = int(M)
        NX = M + 1

        values = []
        sources = {}
        elements = {}
        for n in range(NX):
            value = -mu * n + 0.5 * U * n * (n - 1)
            values.append(n)
            append_matelem(elements, state=n, value=value)
            if n > 0:
                # annihilator
                append_matelem(
                    sources, istate=n, fstate=n - 1, value=annihilator_boson(n)
                )
            if n < M:
                # creator
                value = creator_boson(n)
                append_matelem(sources, istate=n, fstate=n + 1, value=value)
        super(BoseSite, self).__init__(id=id, N=NX, values=values,
                                       elements=elements, sources=sources)


class BoseBond(Interaction):
    def __init__(self, id, M, t, V):
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
        elements = {}
        for i in range(nx):
            for j in range(nx):
                # diagonal
                w = V * i * j
                if w != 0.0:
                    append_matelem(elements, state=[i, j], value=w)
                # offdiagonal
                w = -t * c[i] * a[j]
                if w != 0.0:
                    append_matelem(
                        elements, istate=[i, j], fstate=[i + 1, j - 1], value=w
                    )
                w = -t * a[i] * c[j]
                if w != 0.0:
                    append_matelem(
                        elements, istate=[i, j], fstate=[i - 1, j + 1], value=w
                    )
        super(BoseBond, self).__init__(id=id, nbody=nbody,
                                       Ns=Ns, elements=elements)


class BoseHubbard_hamiltonian(Hamiltonian):
    def __init__(self, param):
        M = param["M"]
        Us = get_as_list(param, "U", 0.0)
        mus = get_as_list(param, "mu", 0.0)
        nstypes = max(len(Us), len(mus))
        extend_list(Us, nstypes)
        extend_list(mus, nstypes)
        self.sites = [BoseSite(i, M, U, mu)
                      for i, (U, mu) in enumerate(zip(Us, mus))]

        ts = get_as_list(param, "t", 0.0)
        Vs = get_as_list(param, "V", 0.0)
        nitypes = max(len(ts), len(Vs))
        extend_list(ts, nitypes)
        extend_list(Vs, nitypes)
        self.interactions = [
            BoseBond(i, M, t, V) for i, (t, V) in enumerate(zip(ts, Vs))
        ]
        self.name = "Bose-Hubbard model"
