import sys
import pytest
import numpy as np

from dsqss.prob_kernel import heat_bath, metropolis, suwa_todo, reversible_suwa_todo

ALL_KERNELS = [heat_bath, metropolis, suwa_todo, reversible_suwa_todo]
NONZERO_WEIGHT_CASES = [[1.0, 1.0], [2.0, 1.0], [3.0, 1.0, 0.5]]


# ---- shared properties: shape, nonnegativity, row sums ----

@pytest.mark.parametrize("kernel", ALL_KERNELS)
@pytest.mark.parametrize("weights", NONZERO_WEIGHT_CASES)
def test_output_shape(kernel, weights):
    N = len(weights)
    W = kernel(weights)
    assert W.shape == (N, N)


@pytest.mark.parametrize("kernel", ALL_KERNELS)
@pytest.mark.parametrize("weights", NONZERO_WEIGHT_CASES)
def test_nonnegative(kernel, weights):
    W = kernel(weights)
    assert np.all(W >= 0.0)


@pytest.mark.parametrize("kernel", ALL_KERNELS)
@pytest.mark.parametrize("weights", NONZERO_WEIGHT_CASES)
def test_rows_sum_to_one(kernel, weights):
    W = kernel(weights)
    np.testing.assert_allclose(W.sum(axis=1), 1.0, atol=1e-10)


# ---- heat_bath ----

def test_heat_bath_all_rows_equal():
    W = heat_bath([1.0, 2.0, 3.0])
    for i in range(len(W)):
        np.testing.assert_array_equal(W[i], W[0])


def test_heat_bath_uniform_weights():
    W = heat_bath([1.0, 1.0, 1.0, 1.0])
    np.testing.assert_allclose(W, 0.25, atol=1e-12)


def test_heat_bath_zero_weight_column_is_zero():
    W = heat_bath([1.0, 0.0, 1.0])
    np.testing.assert_array_equal(W[:, 1], 0.0)


# ---- metropolis ----

def test_metropolis_zero_weight_row_is_zero():
    W = metropolis([1.0, 0.0, 1.0])
    np.testing.assert_array_equal(W[1, :], 0.0)


def test_metropolis_detailed_balance():
    weights = [1.0, 2.0, 3.0]
    W = metropolis(weights)
    for i in range(len(weights)):
        for j in range(len(weights)):
            assert abs(weights[i] * W[i, j] - weights[j] * W[j, i]) < 1e-10


def test_metropolis_nonzero_rows_sum_to_one():
    weights = [1.0, 0.0, 1.0]
    W = metropolis(weights)
    assert abs(W[0].sum() - 1.0) < 1e-10
    assert abs(W[2].sum() - 1.0) < 1e-10


# ---- suwa_todo ----

def test_suwa_todo_two_states():
    W = suwa_todo([1.0, 2.0])
    assert W.shape == (2, 2)
    np.testing.assert_allclose(W.sum(axis=1), 1.0, atol=1e-10)


@pytest.mark.xfail(
    strict=True,
    reason="Bug: prob_kernel.py:88 — infinite loop when all remaining targets are 0.0. "
           "weights=[1.0,0.0,0.0] causes while-loop to spin forever after all targets zeroed.",
)
@pytest.mark.skipif(sys.platform == "win32", reason="SIGALRM not available on Windows")
def test_suwa_todo_single_nonzero_weight_does_not_hang():
    import signal

    def _timeout(signum, frame):
        raise TimeoutError("suwa_todo hung — infinite loop confirmed")

    signal.signal(signal.SIGALRM, _timeout)
    signal.alarm(2)
    try:
        suwa_todo([1.0, 0.0, 0.0])
    finally:
        signal.alarm(0)


# ---- reversible_suwa_todo ----

def test_reversible_suwa_todo_detailed_balance():
    weights = [1.0, 2.0, 3.0]
    W = reversible_suwa_todo(weights)
    for i in range(len(weights)):
        for j in range(len(weights)):
            assert abs(weights[i] * W[i, j] - weights[j] * W[j, i]) < 1e-10


@pytest.mark.parametrize("kernel", ALL_KERNELS)
@pytest.mark.parametrize("weights", NONZERO_WEIGHT_CASES)
def test_kernel_preserves_stationary_distribution(kernel, weights):
    # p is the equilibrium distribution; a valid transition kernel must
    # leave it invariant: p @ W == p
    W = kernel(weights)
    p = np.array(weights) / sum(weights)
    np.testing.assert_allclose(p @ W, p, atol=1e-10)
