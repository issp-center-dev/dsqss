import pytest

from dsqss.lattice import Lattice
from dsqss.lattice_factory.hypercubic import generate as hypercubic_generate
from dsqss.lattice_factory.triangular import generate as triangular_generate
from dsqss.lattice_factory.honeycomb import generate as honeycomb_generate
from dsqss.lattice_factory.kagome import generate as kagome_generate


# ---- helpers ----

def _sites(d):
    return d["unitcell"]["sites"]


def _bonds(d):
    return d["unitcell"]["bonds"]


def _make_lattice(d):
    lat = Lattice()
    lat.load_dict(d)
    return lat


# ---- hypercubic ----

def test_hypercubic_1d_one_site_one_bond():
    d = hypercubic_generate({"dim": 1, "L": 4})
    assert len(_sites(d)) == 1
    assert len(_bonds(d)) == 1


def test_hypercubic_2d_one_site_two_bonds():
    d = hypercubic_generate({"dim": 2, "L": [4, 4]})
    assert len(_sites(d)) == 1
    assert len(_bonds(d)) == 2


def test_hypercubic_3d_one_site_three_bonds():
    d = hypercubic_generate({"dim": 3, "L": [2, 2, 2]})
    assert len(_sites(d)) == 1
    assert len(_bonds(d)) == 3


def test_hypercubic_scalar_l_broadcast():
    d = hypercubic_generate({"dim": 2, "L": 4})
    assert d["parameter"]["L"] == [4, 4]


def test_hypercubic_open_boundary():
    d = hypercubic_generate({"dim": 1, "L": 4, "bc": False})
    assert d["parameter"]["bc"] == [False]


def test_hypercubic_creates_valid_lattice():
    d = hypercubic_generate({"dim": 1, "L": 4})
    _make_lattice(d)  # must not raise


# ---- triangular ----

def test_triangular_one_site_three_bonds():
    d = triangular_generate({"L": [4, 4]})
    assert len(_sites(d)) == 1
    assert len(_bonds(d)) == 3


def test_triangular_creates_valid_lattice():
    d = triangular_generate({"L": [4, 4]})
    _make_lattice(d)


# ---- honeycomb ----

def test_honeycomb_two_sites_per_cell():
    d = honeycomb_generate({"L": [4, 4]})
    assert len(_sites(d)) == 2


def test_honeycomb_three_bonds_per_cell():
    d = honeycomb_generate({"L": [4, 4]})
    assert len(_bonds(d)) == 3


def test_honeycomb_creates_valid_lattice():
    d = honeycomb_generate({"L": [4, 4]})
    _make_lattice(d)


# ---- kagome ----

def test_kagome_three_sites_per_cell():
    d = kagome_generate({"L": [4, 4]})
    assert len(_sites(d)) == 3


def test_kagome_six_bonds_per_cell():
    d = kagome_generate({"L": [4, 4]})
    assert len(_bonds(d)) == 6


def test_kagome_creates_valid_lattice():
    d = kagome_generate({"L": [4, 4]})
    _make_lattice(d)
