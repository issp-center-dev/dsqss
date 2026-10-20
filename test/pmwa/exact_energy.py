"""Exact reference values for the DSQSS/PMWA tests.

Computes the energy per site (and the density per site) of the model that
pmwa_H / pmwa_B actually simulate, by exact diagonalization at finite
temperature, so that the ``Result`` entries of the ``*.json`` test cases can
be (re)generated instead of being copied from an earlier run.

The Hamiltonians, read off src/pmwa/probability.{H,B}.cpp (the ``ene``
estimator is ``(Eu + Et - <n_vertex>/beta) / V`` and the shifts cancel):

  pmwa_H (nmax = 1, written in the frame after the sublattice rotation):
      H = sum_<ij> [ -t (Sx_i Sx_j + Sy_i Sy_j) - v Sz_i Sz_j ]
          - mu sum_i Sz_i - 2 g sum_i Sx_i
  pmwa_B:
      H = sum_<ij> [ -t (b_i^+ b_j + h.c.) + v n_i n_j ]
          + sum_i [ u/2 n_i (n_i - 1) - mu n_i ] - g sum_i (b_i^+ + b_i)

where t, v, u, mu, g are the keys of the pmwa parameter file.  The lattice is
the periodic hypercubic lattice as lattgene_P builds it: along a direction of
length 2 the +1 and -1 neighbours coincide, so that bond is counted twice.

Usage:
    python exact_energy.py spin_chain_1.json [...]   # print ene (and amzu)
                                                     # for the Parameter of
                                                     # each test case
"""

import json
import sys
from functools import reduce

import numpy as np
import scipy.sparse as sp
import scipy.sparse.linalg as sla

DENSE_LIMIT = 4096  # full diagonalization up to this dimension


def bonds_hypercubic(L):
    """Nearest-neighbour bonds of a periodic hypercubic lattice (lattgene_P)."""
    dims = list(L)
    nsite = int(np.prod(dims))
    strides = [int(np.prod(dims[:k])) for k in range(len(dims))]

    def coord(i):
        return [(i // strides[k]) % dims[k] for k in range(len(dims))]

    def index(c):
        return sum(ck * sk for ck, sk in zip(c, strides))

    bonds = []
    for i in range(nsite):
        c = coord(i)
        for k in range(len(dims)):
            if dims[k] == 1:
                continue
            c2 = list(c)
            c2[k] = (c[k] + 1) % dims[k]
            bonds.append((i, index(c2)))
    return nsite, bonds


def _site_operators(local_ops, nsite):
    ident = sp.identity(local_ops[0].shape[0], format="csr")

    def embed(o, i):
        return reduce(
            lambda m, k: sp.kron(m, o if k == i else ident, format="csr"),
            range(nsite),
            sp.identity(1, format="csr"),
        )

    return [[embed(o, i) for i in range(nsite)] for o in local_ops]


def hamiltonian(model, L, t, v, g, mu=0.0, u=0.0, nmax=1):
    """Return (nsite, H, number operator) for the pmwa model."""
    nsite, bonds = bonds_hypercubic(L)
    if model == "spin":
        sx = sp.csr_matrix(np.array([[0.0, 0.5], [0.5, 0.0]]))
        sy = sp.csr_matrix(np.array([[0.0, -0.5j], [0.5j, 0.0]]))
        sz = sp.csr_matrix(np.diag([0.5, -0.5]))
        SX, SY, SZ = _site_operators([sx, sy, sz], nsite)
        H = sum(
            -t * (SX[i] @ SX[j] + SY[i] @ SY[j]) - v * SZ[i] @ SZ[j]
            for i, j in bonds
        )
        H = H - mu * sum(SZ) - 2.0 * g * sum(SX)
        number = sum(SZ)
    elif model == "boson":
        d = nmax + 1
        b = sp.csr_matrix(np.diag(np.sqrt(np.arange(1, d)), 1))
        n = sp.csr_matrix(np.diag(np.arange(d, dtype=float)))
        B, N = _site_operators([b, n], nsite)
        H = sum(
            -t * (B[i].T @ B[j] + B[j].T @ B[i]) + v * N[i] @ N[j] for i, j in bonds
        )
        H = H + sum(0.5 * u * (Ni @ Ni - Ni) - mu * Ni for Ni in N)
        H = H - g * sum(Bi + Bi.T for Bi in B)
        number = sum(N)
    else:
        raise ValueError("unknown model: {0}".format(model))
    H = H.tocsr()
    if abs(H.imag).max() < 1e-12:
        H = H.real
    return nsite, H, number.tocsr()


def thermal_averages(H, number, beta, nev=40):
    """Energy and density per site at inverse temperature beta.

    Dense diagonalization when the Hilbert space is small; otherwise the
    lowest ``nev`` eigenpairs (adequate when beta times the gap to the
    discarded states is large).
    """
    if H.shape[0] <= DENSE_LIMIT:
        E, U = np.linalg.eigh(H.toarray())
    else:
        E, U = sla.eigsh(H.tocsc(), k=nev, which="SA")
        order = np.argsort(E)
        E, U = E[order], U[:, order]
    w = np.exp(-beta * (E - E[0]))
    w /= w.sum()
    ene = float((E * w).sum())
    nexp = float(np.einsum("ia,ij,ja,a->", U.conj(), number.toarray(), U, w).real)
    return ene, nexp


def pmwa_parameters(param):
    """Map the ``Parameter`` dict of a test case to the pmwa parameters.

    Same conventions as ``geninp`` in test.py: for the spin model
    t = Jxy, v = -Jz, mu = h, and nmax = 2S.
    """
    if param["Model"] == "boson":
        return dict(
            model="boson",
            t=param["t"],
            v=param["V"],
            u=param["U"],
            mu=param["mu"],
            g=param["G"],
            nmax=param["M"],
        )
    return dict(
        model="spin",
        t=param["Jxy"],
        v=-param["Jz"],
        u=0.0,
        mu=param["h"],
        g=param["G"],
        nmax=int(2 * param["S"]),
    )


def exact_result(param, nev=40):
    """``Result`` dict (ene, amzu) for the ``Parameter`` dict of a test case."""
    p = pmwa_parameters(param)
    if p["model"] == "spin" and p["nmax"] != 1:
        raise ValueError("pmwa_H is defined for S=1/2 only")
    nsite, H, number = hamiltonian(
        p["model"], param["L"], p["t"], p["v"], p["g"], p["mu"], p["u"], p["nmax"]
    )
    ene, nexp = thermal_averages(H, number, param["beta"], nev)
    return {"ene": ene / nsite, "amzu": nexp / nsite}


def main(argv):
    if len(argv) < 2:
        print("usage: python exact_energy.py <jsonfile> [<jsonfile> ...]")
        return 1
    for jsonfile in argv[1:]:
        with open(jsonfile) as f:
            param = json.load(f)["Parameter"]
        res = exact_result(param)
        print(
            "{0}: ene = {1:.10f}, amzu = {2:.10f}".format(
                jsonfile, res["ene"], res["amzu"]
            )
        )
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
