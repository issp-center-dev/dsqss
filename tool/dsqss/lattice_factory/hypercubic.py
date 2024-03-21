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

import numpy as np

import dsqss.util


def generate(param):
    dim = param["dim"]
    L = dsqss.util.get_as_list(param, "L", extendto=dim)
    bc = dsqss.util.get_as_list(param, "bc", default=True, extendto=dim)
    basis = np.eye(dim)

    sites = [{"siteid": 0, "type": 0, "coord": [0.0] * dim}]
    bonds = []
    for d in range(dim):
        offset = [0] * dim
        offset[d] = 1
        bonds.append(
            {
                "bondid": d,
                "type": 0,
                "source": {"siteid": 0},
                "target": {"siteid": 0, "offset": offset},
            }
        )

    parameter = {"name": "hypercubic", "dim": dim, "L": L, "bc": bc, "basis": basis}
    unitcell = {"sites": sites, "bonds": bonds}

    return {"parameter": parameter, "unitcell": unitcell}
