import types
import pytest

from dsqss.algorithm import AlgSite, AlgInteraction, Algorithm


# ---- AlgSite / AlgInteraction / Algorithm built from the S=1/2 XXZ chain ----

def test_algsite_created_for_each_site(graphed_ham):
    alg = Algorithm(graphed_ham)
    assert len(alg.sites) == graphed_ham.nstypes


def test_alginteraction_created_for_each_indeed_interaction(graphed_ham):
    alg = Algorithm(graphed_ham)
    assert len(alg.interactions) == len(graphed_ham.indeed_interactions)


def test_algsite_channels_nonempty(graphed_ham):
    alg = Algorithm(graphed_ham)
    site = alg.sites[0]
    assert len(site.initialconfigurations) > 0


def test_alginteraction_vertex_created(graphed_ham):
    alg = Algorithm(graphed_ham)
    for interaction in alg.interactions:
        assert interaction.vertex is not None


def test_algorithm_write_xml(graphed_ham, tmp_path):
    alg = Algorithm(graphed_ham)
    path = str(tmp_path / "algorithm.xml")
    alg.write_xml(path)
    with open(path) as f:
        content = f.read()
    assert "<Algorithm>" in content


# ---- interactions without off-diagonal elements and with a constant diagonal ----
# (e.g. a coupling whose value is zero); EBASE used to be -inf

def _alginteraction(spin_half_ham, elements, **kwargs):
    from dsqss.hamiltonian import IndeedInteraction, Interaction
    from dsqss.lattice import Vertex

    inter = Interaction(id=0, nbody=2, Ns=[2, 2], elements=elements)
    vertex = Vertex(0, 0, [0, 0], [2, 2])
    hamint = IndeedInteraction(spin_half_ham.sites, [inter], vertex)
    return AlgInteraction(hamint, spin_half_ham.sites, **kwargs)


def _ebase(interaction):
    return (
        interaction.ebase_negsign
        + interaction.ebase_nobounce
        + interaction.ebase_extra
    )


def test_alginteraction_empty_elements_has_zero_ebase(spin_half_ham):
    interaction = _alginteraction(spin_half_ham, {})
    assert _ebase(interaction) == 0.0
    assert interaction.intelements == {}
    assert interaction.vertex.initialconfigurations == []


def test_alginteraction_constant_diagonal_has_finite_ebase(spin_half_ham):
    from dsqss.hamiltonian import keystate

    elements = {
        keystate(st, st): 0.3 for st in [(0, 0), (0, 1), (1, 0), (1, 1)]
    }
    interaction = _alginteraction(spin_half_ham, elements)
    assert _ebase(interaction) == pytest.approx(0.3)
    assert interaction.intelements == {}


def test_alginteraction_empty_elements_with_extra_shift(spin_half_ham):
    interaction = _alginteraction(spin_half_ham, {}, ebase_extra=0.5)
    assert _ebase(interaction) == pytest.approx(0.5)
    assert len(interaction.intelements) == 4
    assert all(v == pytest.approx(0.5) for v in interaction.intelements.values())


def test_alginteraction_offdiagonal_only_gets_extra_shift(spin_half_ham):
    from dsqss.hamiltonian import keystate

    elements = {
        keystate((0, 1), (1, 0)): -0.5,
        keystate((1, 0), (0, 1)): -0.5,
    }
    interaction = _alginteraction(spin_half_ham, elements)
    assert _ebase(interaction) == pytest.approx(0.25)


def test_algorithm_write_xml_with_empty_interaction(
    spin_half_ham, chain_lattice_dict, tmp_path
):
    from dsqss.hamiltonian import GraphedHamiltonian
    from dsqss.lattice import Lattice

    # next-nearest-neighbour bonds carrying an interaction without elements
    chain_lattice_dict["unitcell"]["bonds"].append(
        {
            "bondid": 1,
            "type": 1,
            "source": {"siteid": 0},
            "target": {"siteid": 0, "offset": [2]},
        }
    )
    lat = Lattice()
    lat.load_dict(chain_lattice_dict)

    ham_dict = spin_half_ham.to_dict()
    ham_dict["interactions"].append(
        {"type": 1, "nbody": 2, "N": [2, 2], "elements": []}
    )

    ham = GraphedHamiltonian(ham_dict, lat)
    alg = Algorithm(ham)
    assert len(alg.interactions) == 2
    path = str(tmp_path / "algorithm.xml")
    alg.write_xml(path)
    with open(path) as f:
        content = f.read()
    assert "inf" not in content
    assert "nan" not in content


# ---- known bug: ZeroDivisionError when a site has zero states ----

def test_alginteraction_zero_states_site_raises_informative_error():
    fake_site = types.SimpleNamespace(N=0, sources={})
    hamint = types.SimpleNamespace(itype=0, stypes=[0], nbody=1, elements={})
    with pytest.raises(ValueError):
        AlgInteraction(hamint, [fake_site])
