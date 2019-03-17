import numpy as np
from scipy.special import cosdg, sindg

from ..util import get_as_list


def generate(param):
    dim = 2
    L = get_as_list(param, "L", extendto=dim)
    bc = get_as_list(param, "bc", default=True, extendto=dim)
    basis = np.array([[1.0, 0.0], [cosdg(120), sindg(120)]])

    sites = [
            {"siteid": 0, "type": 0, "coord": [0.0, 0.0]},
            {"siteid": 1, "type": 0, "coord": [0.5, 0.0]},
            {"siteid": 1, "type": 0, "coord": [0.5, 0.5]},
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
            "source": {"siteid": 0},
            "target": {"siteid": 2, "offset": [0, 0]},
        },
        {
            "bondid": 2,
            "type": 0,
            "source": {"siteid": 1},
            "target": {"siteid": 2, "offset": [0, 0]},
        },
        {
            "bondid": 3,
            "type": 0,
            "source": {"siteid": 1},
            "target": {"siteid": 0, "offset": [1, 0]},
        },
        {
            "bondid": 4,
            "type": 0,
            "source": {"siteid": 2},
            "target": {"siteid": 1, "offset": [0, 1]},
        },
        {
            "bondid": 5,
            "type": 0,
            "source": {"siteid": 2},
            "target": {"siteid": 0, "offset": [1, 1]},
        },
    ]

    parameter = {"name": "kagome", "dim": dim, "L": L, "bc": bc, "basis": basis}
    unitcell = {"sites": sites, "bonds": bonds}

    return {"parameter": parameter, "unitcell": unitcell}
