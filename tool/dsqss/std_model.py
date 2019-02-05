from dsqss.hamiltonian import Hamiltonian
from dsqss.xxz import XXZ_hamiltonian
from dsqss.bosehubbard import BoseHubbard_hamiltonian
from dsqss.util import ERROR

def std_model(param, lat):
    if param['model'].lower() == 'spin':
        hamdict = XXZ_hamiltonian(param)
    elif param['model'].lower() == 'boson':
        hamdict = BoseHubbard_hamiltonian(param)
    else:
        ERROR('Unknown model: param["model"] = {0}'.format(param['model'].lower()))

    return Hamiltonian(hamdict.to_dict(), lat)

