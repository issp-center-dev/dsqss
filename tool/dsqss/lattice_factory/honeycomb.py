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
from scipy.special import cosdg, sindg

from ..util import get_as_list


def generate(param):
    dim = 2
    L = get_as_list(param, "L", extendto=dim)
    bc = get_as_list(param, "bc", default=True, extendto=dim)
    basis = np.array([[1.0, 0.0], [cosdg(120), sindg(120)]])

    sites = [
            {"siteid": 0, "type": 0, "coord": [0.0] * dim},
            {"siteid": 1, "type": 0, "coord": [1.0/3, 2.0/3]},
            ]
    bonds = [
        {
            "bondid": 0,
            "type": 0,
            "source": {"siteid": 0},
            "target": {"siteid": 1, "offset": [0, 0]},
        },
        {
            "bondid": 1,
            "type": 0,
            "source": {"siteid": 1},
            "target": {"siteid": 0, "offset": [1, 1]},
        },
        {
            "bondid": 2,
            "type": 0,
            "source": {"siteid": 1},
            "target": {"siteid": 0, "offset": [0, 1]},
        },
    ]

    parameter = {"name": "honeycomb", "dim": dim, "L": L, "bc": bc, "basis": basis}
    unitcell = {"sites": sites, "bonds": bonds}

    return {"parameter": parameter, "unitcell": unitcell}
