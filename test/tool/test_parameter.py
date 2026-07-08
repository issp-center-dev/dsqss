import pytest

from dsqss.parameter import set_default_values, check_mandatories, Parameter


# ---- set_default_values ----

def test_set_default_values_fills_missing():
    param = {"beta": 1.0}
    set_default_values(param)
    assert param["npre"] == 1000
    assert param["ntherm"] == 1000
    assert param["nmcs"] == 1000
    assert param["algfile"] == "algorithm.xml"
    assert param["latfile"] == "lattice.xml"
    assert param["outfile"] == "sample.log"


def test_set_default_values_does_not_overwrite():
    param = {"beta": 1.0, "npre": 500, "nmcs": 200}
    set_default_values(param)
    assert param["npre"] == 500
    assert param["nmcs"] == 200


def test_set_default_values_fills_all_defaults():
    param = {"beta": 1.0}
    set_default_values(param)
    expected_keys = [
        "npre", "ntherm", "ndecor", "nmcs", "nset", "simulationtime",
        "ntau", "seed", "nvermax", "nsegmax",
        "algfile", "latfile", "wvfile", "dispfile",
        "outfile", "sfoutfile", "cfoutfile", "ckoutfile",
    ]
    for k in expected_keys:
        assert k in param, f"missing default key: {k}"


# ---- check_mandatories ----

def test_check_mandatories_missing_beta_exits():
    with pytest.raises(SystemExit):
        check_mandatories({})


def test_check_mandatories_with_beta_passes():
    check_mandatories({"beta": 1.0})  # must not raise


# ---- Parameter class ----

def test_parameter_flat_dict():
    p = Parameter({"beta": 1.0})
    assert p["beta"] == 1.0
    assert "npre" in p  # defaults filled in


def test_parameter_nested_dict():
    p = Parameter({"parameter": {"beta": 2.0}})
    assert p["beta"] == 2.0


def test_parameter_is_dict_subclass():
    p = Parameter({"beta": 1.0})
    assert isinstance(p, dict)
