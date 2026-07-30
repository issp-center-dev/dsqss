import types
from io import StringIO

import pytest

from dsqss.pmwa_pre import info


def _make_info(text):
    args = types.SimpleNamespace(gendir="/nonexistent", input=StringIO(text))
    return info(args)


PMWA_SPIN_INPUT = """\
solver = PMWA
model_type = spin
Jxy = -1.0
Jz = -1
h = 0.25
gamma = 0.2
lattice_type = square
D = 1
L = 8
Beta = 10
"""


def test_valid_pmwa_input_parses():
    inf = _make_info(PMWA_SPIN_INPUT)
    assert inf.prm.info_dict["solver"] == "PMWA"


def test_spin_keyword_mapping():
    inf = _make_info(PMWA_SPIN_INPUT)
    # jxy -> t (absolute value: bipartite unitary transform)
    assert float(inf.prm.param_dict["t"]) == 1.0
    # jz -> v, h -> mu, gamma -> g (halved for spin)
    assert float(inf.prm.param_dict["v"]) == -1.0
    assert float(inf.prm.param_dict["mu"]) == 0.25
    assert float(inf.prm.param_dict["g"]) == 0.1


def test_missing_solver_exits_with_message(capfd):
    with pytest.raises(SystemExit):
        _make_info("model_type = spin\nL = 8\n")
    assert "solver is not specified" in capfd.readouterr().err


def test_dla_solver_redirects_to_dla_pre(capfd):
    with pytest.raises(SystemExit):
        _make_info("solver = DLA\nL = 8\n")
    assert "dla_pre" in capfd.readouterr().err


def test_unknown_solver_rejected(capfd):
    with pytest.raises(SystemExit):
        _make_info("solver = PMWAA\nL = 8\n")
    assert "unknown solver" in capfd.readouterr().err


def test_get_solver_name():
    inf = _make_info(PMWA_SPIN_INPUT)
    assert inf.get_solver() == "pmwa_H"
