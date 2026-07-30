import pytest

from dsqss.result import Result, Results


# ---- Result ----

def test_result_to_str_default_delim():
    assert Result(0.5, 0.01).to_str() == "0.5+/-0.01"


def test_result_to_str_custom_delim():
    assert Result(0.5, 0.01).to_str(delim=" +- ") == "0.5 +- 0.01"


def test_result_stores_mean_and_err():
    r = Result(1.234, 0.005)
    assert r.mean == 1.234
    assert r.err == 0.005


# ---- Results ----

def _write_result_file(path, lines):
    path.write_text("\n".join(lines) + "\n")


def test_results_parses_r_lines(tmp_path):
    f = tmp_path / "result.dat"
    _write_result_file(f, [
        "R Energy 0 1.234 0.005 tag",
        "R Magnetization 0 0.0 0.001 tag",
    ])
    r = Results(f)
    assert abs(r.result["Energy"].mean - 1.234) < 1e-9
    assert abs(r.result["Energy"].err - 0.005) < 1e-9
    assert abs(r.result["Magnetization"].mean - 0.0) < 1e-9


def test_results_ignores_non_r_lines(tmp_path):
    f = tmp_path / "result.dat"
    _write_result_file(f, [
        "# comment",
        "  some other line",
        "R Energy 0 2.0 0.1 tag",
    ])
    r = Results(f)
    assert "Energy" in r.result
    assert len(r.result) == 1


def test_results_to_str_single(tmp_path):
    f = tmp_path / "result.dat"
    _write_result_file(f, ["R Energy 0 1.5 0.02 tag"])
    r = Results(f)
    # Results.to_str defaults delim=" " (unlike Result.to_str's "+/-" default)
    assert r.to_str("Energy") == "1.5 0.02"
    assert r.to_str("Energy", delim="+/-") == "1.5+/-0.02"


def test_results_to_str_multiple(tmp_path):
    f = tmp_path / "result.dat"
    _write_result_file(f, [
        "R Energy 0 1.5 0.02 tag",
        "R Mag 0 0.3 0.01 tag",
    ])
    r = Results(f)
    s = r.to_str(["Energy", "Mag"], delim="+/-")
    assert "1.5+/-0.02" in s
    assert "0.3+/-0.01" in s
