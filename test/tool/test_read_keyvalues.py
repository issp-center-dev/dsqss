import pytest
from io import StringIO

from dsqss.read_keyvalues import read_keyvalues, parse_list


# ---- read_keyvalues ----

def test_simple_keyvalue():
    result = read_keyvalues(StringIO("beta = 1.0\n"))
    assert result == {"beta": "1.0"}


def test_comment_stripped():
    result = read_keyvalues(StringIO("beta = 1.0 # a comment\n"))
    assert result == {"beta": "1.0"}


def test_key_is_lowercased():
    result = read_keyvalues(StringIO("Beta = 1.0\n"))
    assert "beta" in result
    assert "Beta" not in result


def test_comma_separated_becomes_list():
    result = read_keyvalues(StringIO("L = 4,4\n"))
    assert result["l"] == ["4", "4"]


def test_blank_line_ignored():
    result = read_keyvalues(StringIO("\nbeta = 1.0\n"))
    assert result == {"beta": "1.0"}


def test_comment_only_line_ignored():
    result = read_keyvalues(StringIO("# full comment line\nbeta = 1.0\n"))
    assert result == {"beta": "1.0"}


def test_multiple_keys():
    src = "beta = 2.0\nnmcs = 1000\n"
    result = read_keyvalues(StringIO(src))
    assert result["beta"] == "2.0"
    assert result["nmcs"] == "1000"


def test_leading_space_in_key_is_handled():
    # data.strip() on line 34 is not assigned back, but keys are still stripped
    # individually via d_re.group(1).strip(), so behavior is correct.
    result = read_keyvalues(StringIO("  beta = 1.0\n"))
    assert "beta" in result
    assert "  beta" not in result


# ---- parse_list ----

def test_parse_list_scalar_string():
    assert parse_list("3", 3, int) == [3, 3, 3]


def test_parse_list_extends_list():
    assert parse_list(["1", "2"], 4, int) == [1, 2, 2, 2]


def test_parse_list_no_type_conversion():
    assert parse_list("abc", 2) == ["abc", "abc"]


def test_parse_list_exact_length():
    assert parse_list([1, 2, 3], 3) == [1, 2, 3]


def test_parse_list_single_element_repeated():
    assert parse_list([7], 4) == [7, 7, 7, 7]
