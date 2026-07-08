import pytest

from dsqss.hamiltonian import (
    keystate,
    States,
    append_matelem,
    Site,
    Interaction,
    Hamiltonian,
    GraphedHamiltonian,
)
from dsqss.xxz import XXZ_hamiltonian


# ---- keystate ----

def test_keystate_int_input():
    s = keystate(0, 1)
    assert s == States(initial=(0,), final=(1,))


def test_keystate_list_input():
    s = keystate([0, 1], [1, 0])
    assert s == States(initial=(0, 1), final=(1, 0))


def test_keystate_diagonal():
    s = keystate(2, 2)
    assert s.initial == s.final


# ---- append_matelem ----

def test_append_matelem_state_form():
    m = {}
    append_matelem(m, state=0, value=1.5)
    assert m[keystate(0, 0)] == 1.5


def test_append_matelem_istate_fstate_form():
    m = {}
    append_matelem(m, istate=0, fstate=1, value=-0.5)
    assert m[keystate(0, 1)] == -0.5


def test_append_matelem_missing_value_raises():
    with pytest.raises(RuntimeError, match="value"):
        append_matelem({}, state=0)


def test_append_matelem_missing_state_raises():
    with pytest.raises(RuntimeError):
        append_matelem({}, value=1.0)


def test_append_matelem_returns_matelems():
    m = {}
    result = append_matelem(m, state=0, value=1.0)
    assert result is m


# ---- Hamiltonian (via dict) ----

def test_hamiltonian_from_dict():
    d = {
        "name": "test",
        "sites": [{"type": 0, "N": 2, "values": [0.0, 1.0], "elements": [], "sources": []}],
        "interactions": [{"type": 0, "nbody": 1, "N": [2], "elements": []}],
    }
    h = Hamiltonian(d)
    assert h.nstypes == 1
    assert h.nitypes == 1
    assert h.nxmax == 2


@pytest.mark.xfail(
    strict=True,
    reason="Bug: hamiltonian.py:229,240 — RuntimeError raised with empty message. "
           "Should include context about which site/interaction slot is None.",
)
def test_hamiltonian_missing_site_gives_informative_error():
    # Two sites in list, both with type=0 — leaves type=1 slot as None
    bad = {
        "name": "bad",
        "sites": [
            {"type": 0, "N": 2, "values": [0.0, 1.0], "elements": [], "sources": []},
            {"type": 0, "N": 2, "values": [0.0, 1.0], "elements": [], "sources": []},
        ],
        "interactions": [],
    }
    with pytest.raises(RuntimeError, match=r".+"):  # fails because message is ""
        Hamiltonian(bad)


# ---- XXZ_hamiltonian ----

def test_xxz_spin_half_nstypes():
    h = XXZ_hamiltonian({"M": 1, "Jz": 1.0, "Jxy": 1.0})
    assert h.nstypes == 1


def test_xxz_spin_half_site_n():
    h = XXZ_hamiltonian({"M": 1, "Jz": 1.0, "Jxy": 1.0})
    assert h.sites[0].N == 2


def test_xxz_spin_half_diagonal_elements():
    h = XXZ_hamiltonian({"M": 1, "Jz": 1.0, "Jxy": 0.0})
    bond = h.interactions[0]
    # [↓↓]: -Jz * (-0.5)*(-0.5) = -0.25
    assert abs(bond.elements[keystate([0, 0], [0, 0])] - (-0.25)) < 1e-10
    # [↑↑]: -Jz * (0.5)*(0.5) = -0.25
    assert abs(bond.elements[keystate([1, 1], [1, 1])] - (-0.25)) < 1e-10
    # [↓↑]: -Jz * (-0.5)*(0.5) = +0.25
    assert abs(bond.elements[keystate([0, 1], [0, 1])] - 0.25) < 1e-10


def test_xxz_spin_half_offdiagonal_elements():
    h = XXZ_hamiltonian({"M": 1, "Jz": 0.0, "Jxy": 1.0})
    bond = h.interactions[0]
    # spin-flip: [↓↑] → [↑↓] and [↑↓] → [↓↑]
    assert keystate([0, 1], [1, 0]) in bond.elements
    assert keystate([1, 0], [0, 1]) in bond.elements


# ---- GraphedHamiltonian ----

def test_graphed_ham_indeed_interactions_nonempty(graphed_ham):
    assert len(graphed_ham.indeed_interactions) > 0


def test_graphed_ham_nitypes_equals_vertex_count(graphed_ham, chain_lattice):
    assert graphed_ham.nitypes == len(chain_lattice.vertices)


def test_graphed_ham_nxmax(graphed_ham):
    # S=1/2 has 2 states → nxmax = 2
    assert graphed_ham.nxmax == 2
