import numpy as np

from ..util import get_as_list


def generate(param):
    dim = param["dim"]
    L = get_as_list(param, "L", extendto=dim)
    bc = get_as_list(param, "bc", default=True, extendto=dim)
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
