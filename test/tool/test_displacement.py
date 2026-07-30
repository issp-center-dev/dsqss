import pytest
import numpy as np

from dsqss.displacement import Displacement
from dsqss.lattice import Lattice
from dsqss.lattice_factory.hypercubic import generate as hypercubic_generate


def _open_chain(L=4):
    lat = Lattice()
    lat.load_dict(hypercubic_generate({"dim": 1, "L": L, "bc": False}))
    return lat


# ---- basic attributes ----

def test_displacement_nsites(chain_lattice):
    d = Displacement(chain_lattice)
    assert d.nsites == 4


def test_displacement_matrix_shape(chain_lattice):
    d = Displacement(chain_lattice)
    assert d.displacements.shape == (4, 4)


# ---- self-displacement consistency ----

def test_displacement_self_is_same_kind(chain_lattice):
    d = Displacement(chain_lattice)
    kinds = {d.displacements[i, i] for i in range(d.nsites)}
    assert len(kinds) == 1, "all self-displacements must map to the same kind"


def test_displacement_self_is_kind_zero(chain_lattice):
    d = Displacement(chain_lattice)
    for i in range(d.nsites):
        assert d.displacements[i, i] == 0


# ---- translation symmetry ----

def test_displacement_translation_symmetry(chain_lattice):
    d = Displacement(chain_lattice)
    # sites 0→1 and 1→2 have the same displacement vector (+1)
    assert d.displacements[0, 1] == d.displacements[1, 2]
    assert d.displacements[1, 2] == d.displacements[2, 3]


# ---- periodic boundary ----

def test_displacement_nkinds_periodic_chain(chain_lattice):
    d = Displacement(chain_lattice)
    # L=4 periodic: unique displacements are 0, +1, +2, -1 → 4 kinds
    assert d.nkinds == 4


def test_displacement_periodic_wrapping(chain_lattice):
    d = Displacement(chain_lattice)
    # In a periodic L=4 chain: 0→3 wraps to displacement -1, same as 1→0
    assert d.displacements[0, 3] == d.displacements[1, 0]


# ---- open boundary ----

def test_displacement_open_boundary_nkinds():
    lat = _open_chain(4)
    d = Displacement(lat)
    # No wrapping: displacements are 0, ±1, ±2, ±3 → 7 kinds
    assert d.nkinds == 7


def test_displacement_open_no_wrap():
    lat = _open_chain(4)
    d = Displacement(lat)
    # 0→3 and 0→1 should be different kinds (dr=3 vs dr=1, no wrapping)
    assert d.displacements[0, 3] != d.displacements[0, 1]


# ---- distance_only mode ----

def test_displacement_distance_only_reduces_nkinds(chain_lattice):
    d_full = Displacement(chain_lattice, distance_only=False)
    d_dist = Displacement(chain_lattice, distance_only=True)
    # distance_only merges ±dr into same kind → fewer or equal kinds
    assert d_dist.nkinds <= d_full.nkinds


def test_displacement_distance_only_symmetric(chain_lattice):
    d = Displacement(chain_lattice, distance_only=True)
    # distance is symmetric: d(i,j) kind == d(j,i) kind
    for i in range(d.nsites):
        for j in range(d.nsites):
            assert d.displacements[i, j] == d.displacements[j, i]
