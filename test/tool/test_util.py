import pytest
import math

from dsqss.util import index2coord, coord2index, tagged, extend_list, get_as_list


# ---- index2coord ----

def test_index2coord_zero():
    assert index2coord(0, [4, 4]) == [0, 0]


def test_index2coord_linear():
    # 5 = 1*4 + 1  →  [1, 1]
    assert index2coord(5, [4, 4]) == [1, 1]


def test_index2coord_1d():
    assert index2coord(3, [6]) == [3]


def test_index2coord_3d():
    # 13 = 1 + 2*4 + 1*16  → [1, 2, 1] in [4,4,4]
    idx = 1 + 2 * 4 + 1 * 16
    assert index2coord(idx, [4, 4, 4]) == [1, 2, 1]


# ---- coord2index ----

def test_coord2index_zero():
    assert coord2index([0, 0], [4, 4]) == 0


def test_coord2index_known():
    # r=[1,2], size=[4,4]  →  1 + 2*4 = 9
    assert coord2index([1, 2], [4, 4]) == 9


# ---- roundtrip ----

@pytest.mark.parametrize("size", [[4], [4, 4], [3, 2, 5]])
def test_index2coord_coord2index_roundtrip(size):
    total = 1
    for s in size:
        total *= s
    for i in range(total):
        assert coord2index(index2coord(i, size), size) == i


# ---- tagged ----

def test_tagged_scalar_int():
    assert tagged("Foo", 3) == "<Foo> 3 </Foo>\n"


def test_tagged_list_ints():
    assert tagged("X", [1, 2]) == "<X> 1 2 </X>\n"


def test_tagged_string_not_iterated():
    # strings must not be iterated char-by-char
    assert tagged("N", "hello") == "<N> hello </N>\n"


def test_tagged_float_width_18():
    result = tagged("X", [1.5])
    # extract the value between "<X> " and " </X>"
    inner = result[len("<X> "):result.index(" </X>")]
    assert len(inner) == 18


def test_tagged_float_contains_value():
    result = tagged("X", [1.5])
    assert "1.5" in result


# ---- extend_list ----

def test_extend_list_extends():
    lst = [1, 2]
    result = extend_list(lst, 5)
    assert result == [1, 2, 2, 2, 2]


def test_extend_list_no_op_when_already_long():
    lst = [1, 2, 3]
    result = extend_list(lst, 2)
    assert result == [1, 2, 3]


def test_extend_list_returns_same_object():
    lst = [1, 2]
    result = extend_list(lst, 4)
    assert result is lst


# ---- get_as_list ----

def test_get_as_list_scalar_becomes_list():
    assert get_as_list({"a": 3}, "a") == [3]


def test_get_as_list_list_unchanged():
    assert get_as_list({"a": [1, 2]}, "a") == [1, 2]


def test_get_as_list_uses_default():
    assert get_as_list({}, "a", default=7) == [7]


def test_get_as_list_extendto():
    assert get_as_list({"a": 5}, "a", extendto=3) == [5, 5, 5]


def test_get_as_list_missing_no_default_raises():
    with pytest.raises(KeyError):
        get_as_list({}, "a")
