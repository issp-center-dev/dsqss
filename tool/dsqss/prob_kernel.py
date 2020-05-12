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

from copy import deepcopy

import numpy as np


def heat_bath(weights, cutoff=1e-10):
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the heat bath algorithm
    """
    ws = np.array(weights)
    ws /= ws.sum()
    return np.array([ws for i in range(len(ws))])


def metropolis(weights, cutoff=1e-10):
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Metropolice algorithm
    """
    N = len(weights)
    n = np.count_nonzero(weights)

    W = np.zeros((N, N))
    for i in range(N):
        if weights[i] == 0.0:
            continue
        acc = 0.0
        for j in range(N):
            if i == j:
                continue
            p = min(weights[j] / weights[i], 1.0) / (n - 1)
            if p < cutoff:
                p = 0.0
            W[i, j] = p
            acc += p
        W[i, i] = 1.0 - acc
    return W


def suwa_todo(weights, cutoff=1e-10):
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Suwa-Todo algorithm
    (H. Suwa and S. Todo, Phys. Rev. Lett. 105, 120603 (2010))
    """
    N = len(weights)
    W = np.zeros((N, N))
    indices = np.argsort(weights, kind="mergesort")[::-1]
    target = deepcopy(weights)
    for i in range(N):
        s = weights[indices[i]]
        j = 1
        while target[indices[j]] == 0.0:
            j = (j + 1) % N
        while s > 0.0:
            p = min(s, target[indices[j]])
            s -= p
            target[indices[j]] -= p
            W[indices[i], indices[j]] = p
            if target[indices[j]] <= 0.0:
                target[indices[j]] = 0.0
                if j == i:
                    break
                j = (j + 1) % N
    W /= W.sum(axis=1).reshape(-1, 1)
    W[W < cutoff] = 0.0
    return W / W.sum(axis=1).reshape(-1, 1)


def reversible_suwa_todo(weights, cutoff=1e-10):
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Suwa-Todo algorithm
    under the detailed balance condition
    (H. Suwa and S. Todo, arXiv:1106.3562)
    """
    N = len(weights)
    indices = np.argsort(weights, kind="mergesort")[::-1]
    weights = np.array(weights)
    W = np.zeros([N, N])
    for i in range(N):
        I = indices[i]
        W[I, I] = weights[I]

    S3 = sum(weights[indices[2:]])
    wdiff = weights[indices[0]] - weights[indices[1]]
    if wdiff >= S3:
        for i in range(1, N):
            I = indices[i]
            rst_swap(indices[0], I, weights[I], W)
    else:
        w3 = wdiff / S3
        for i in range(2, N):
            I = indices[i]
            v = w3 * weights[I]
            rst_swap(0, I, v, W)
        for j in range(N - 1, 0, -1):
            J = indices[j]
            v = W[J, J] / j
            for k in range(j - 1, -1, -1):
                rst_swap(J, indices[k], v, W)
    W /= W.sum(axis=1).reshape(-1, 1)
    W[W < cutoff] = 0.0
    return W / W.sum(axis=1).reshape(-1, 1)


def rst_swap(i, j, w, W):
    W[i, i] -= w
    W[i, j] += w
    W[j, i] += w
    W[j, j] -= w
