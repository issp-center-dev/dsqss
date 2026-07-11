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


# ---- known bug: ZeroDivisionError when a site has zero states ----

def test_alginteraction_zero_states_site_raises_informative_error():
    fake_site = types.SimpleNamespace(N=0, sources={})
    hamint = types.SimpleNamespace(itype=0, stypes=[0], nbody=1, elements={})
    with pytest.raises(ValueError):
        AlgInteraction(hamint, [fake_site])
