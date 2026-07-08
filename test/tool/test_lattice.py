import pytest
import numpy as np

from dsqss.lattice import Lattice
from dsqss.lattice_factory.hypercubic import generate as hypercubic_generate


def _open_chain(L=4):
    lat = Lattice()
    lat.load_dict(hypercubic_generate({"dim": 1, "L": L, "bc": False}))
    return lat


# ---- site counts ----

def test_chain_nsites(chain_lattice):
    assert chain_lattice.nsites == 4


def test_chain_site_types_are_zero(chain_lattice):
    for site in chain_lattice.sites:
        assert site.stype == 0


def test_square_nsites(square_lattice):
    assert square_lattice.nsites == 16


# ---- interaction counts ----

def test_chain_nints_periodic(chain_lattice):
    assert chain_lattice.nints == 4


def test_chain_nints_open():
    lat = _open_chain(4)
    assert lat.nints == 3


def test_square_nints_periodic(square_lattice):
    # 2 bonds per cell × 16 cells = 32
    assert square_lattice.nints == 32


# ---- coordination numbers ----

def test_chain_coordination_number(chain_lattice):
    for site in chain_lattice.sites:
        assert site.z == 2


def test_chain_open_boundary_endpoint_coordination():
    lat = _open_chain(4)
    # endpoints (sites 0 and 3) are in only 1 bond; interior sites are in 2
    zs = [site.z for site in lat.sites]
    assert zs[0] == 1
    assert zs[-1] == 1
    assert all(z == 2 for z in zs[1:-1])


# ---- vertices ----

def test_chain_has_one_vertex_type(chain_lattice):
    assert len(chain_lattice.vertices) == 1


# ---- site coordinates ----

def test_chain_site_coords(chain_lattice):
    coords = [site.coord[0] for site in chain_lattice.sites]
    assert coords == [0.0, 1.0, 2.0, 3.0]


# ---- dat file roundtrip ----

def test_lattice_dat_roundtrip(chain_lattice, tmp_path):
    path = str(tmp_path / "chain.dat")
    chain_lattice.save_dat(path)
    lat2 = Lattice()
    lat2.load_dat(path)
    assert lat2.nsites == chain_lattice.nsites
    assert lat2.nints == chain_lattice.nints
    assert lat2.dim == chain_lattice.dim
    assert lat2.name == chain_lattice.name
