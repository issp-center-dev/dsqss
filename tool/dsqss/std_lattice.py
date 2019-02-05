from dsqss.util import ERROR
from dsqss.hypercubic import HyperCubicLattice

def std_lattice(param):
    if param['lattice'] == 'hypercubic':
        return HyperCubicLattice(param)
    else:
        ERROR('Unknown lattice: param["lattice"] = {0}'.format(param['lattice'].lower()))

