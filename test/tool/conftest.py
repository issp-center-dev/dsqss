import pytest
import numpy as np

from dsqss.lattice_factory.hypercubic import generate as hypercubic_generate
from dsqss.lattice import Lattice


@pytest.fixture
def chain_lattice_dict():
    return hypercubic_generate({"dim": 1, "L": 4})


@pytest.fixture
def chain_lattice(chain_lattice_dict):
    lat = Lattice()
    lat.load_dict(chain_lattice_dict)
    return lat


@pytest.fixture
def square_lattice_dict():
    return hypercubic_generate({"dim": 2, "L": [4, 4]})


@pytest.fixture
def square_lattice(square_lattice_dict):
    lat = Lattice()
    lat.load_dict(square_lattice_dict)
    return lat


@pytest.fixture
def spin_half_ham():
    from dsqss.xxz import XXZ_hamiltonian
    return XXZ_hamiltonian({"M": 1, "Jz": 1.0, "Jxy": 1.0})


@pytest.fixture
def graphed_ham(spin_half_ham, chain_lattice):
    from dsqss.hamiltonian import GraphedHamiltonian
    return GraphedHamiltonian(spin_half_ham, chain_lattice)
