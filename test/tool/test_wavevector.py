import pytest
import numpy as np

from dsqss.wavevector import Wavevector


# ---- generate ----

def test_generate_1d_shape():
    wv = Wavevector()
    wv.generate({}, [4])
    # steps = 4//2 = 2; range(0, 3, 2) = [0, 2] → nk=2
    assert wv.dim == 1
    assert wv.nk == 2
    assert wv.ks.shape == (1, 2)


def test_generate_2d_shape():
    wv = Wavevector()
    wv.generate({}, [4, 4])
    # per dim: nk=2; total = 2*2 = 4
    assert wv.dim == 2
    assert wv.nk == 4
    assert wv.ks.shape == (2, 4)


def test_generate_custom_ksteps():
    wv = Wavevector()
    # ksteps=2, size=8: range(0, 5, 2) = [0,2,4] → nk=3
    wv.generate({"ksteps": 2}, [8])
    assert wv.nk == 3


def test_generate_k_zero_always_present():
    wv = Wavevector()
    wv.generate({}, [4, 4])
    # first k-point (ik=0) must be (0,0)
    assert list(wv.ks[:, 0]) == [0, 0]


# ---- save / load roundtrip ----

def test_save_load_roundtrip(tmp_path):
    wv = Wavevector()
    wv.generate({}, [6])
    path = str(tmp_path / "kpoints.dat")
    wv.save(path)

    wv2 = Wavevector()
    wv2.load(path)

    assert wv2.dim == wv.dim
    assert wv2.nk == wv.nk
    np.testing.assert_array_equal(wv2.ks, wv.ks)
