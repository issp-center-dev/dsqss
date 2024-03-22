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

from typing import Sequence

import sys
if sys.version_info >= (3, 8):
    from typing import Protocol
else:
    from typing_extensions import Protocol

import numpy as np


class KernelCallBack(Protocol):
    def __call__(self, weights: Sequence[float], cutoff: float = ...) -> np.ndarray:
        ...


def heat_bath(weights: Sequence[float], cutoff: float = 1e-10) -> np.ndarray:
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the heat bath algorithm
    """

    ws = np.array(weights)
    ws /= ws.sum()
    return np.array([ws for _ in range(len(ws))])


def metropolis(weights: Sequence[float], cutoff: float = 1e-10) -> np.ndarray:
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    (including sugestion probability T(i=>j))
    calculated by the Metropolis algorithm
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


def suwa_todo(weights: Sequence[float], cutoff: float = 1e-10) -> np.ndarray:
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Suwa-Todo algorithm
    (H. Suwa and S. Todo, Phys. Rev. Lett. 105, 120603 (2010))
    """

    N = len(weights)
    W = np.zeros((N, N))
    indices = np.argsort(weights, kind="mergesort")[::-1]
    target = [w for w in weights]
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


def reversible_suwa_todo(weights: Sequence[float], cutoff: float = 1e-10) -> np.ndarray:
    """
    return an array W,
    where W[i,j] is the probability of a transition i=>j
    calculated by the Suwa-Todo algorithm
    under the detailed balance condition
    (H. Suwa and S. Todo, arXiv:1106.3562)
    """
    ws = np.array(weights)
    N = len(ws)
    indices = np.argsort(ws, kind="mergesort")[::-1]
    ws = np.array(ws)
    W = np.zeros([N, N])
    for i in range(N):
        ii = indices[i]
        W[ii, ii] = ws[ii]

    S3 = sum(ws[indices[2:]])
    wdiff = ws[indices[0]] - ws[indices[1]]
    if wdiff >= S3:
        for i in range(1, N):
            ii = indices[i]
            rst_swap(indices[0], ii, ws[ii], W)
    else:
        w3 = wdiff / S3
        for i in range(2, N):
            ii = indices[i]
            v = w3 * ws[ii]
            rst_swap(0, ii, v, W)
        for j in range(N - 1, 0, -1):
            jj = indices[j]
            v = W[jj, jj] / j
            for k in range(j - 1, -1, -1):
                rst_swap(jj, indices[k], v, W)
    W /= W.sum(axis=1).reshape(-1, 1)
    W[W < cutoff] = 0.0
    return W / W.sum(axis=1).reshape(-1, 1)


def rst_swap(i: int, j: int, w: np.ndarray, W: np.ndarray) -> None:
    W[i, i] -= w
    W[i, j] += w
    W[j, i] += w
    W[j, j] -= w
