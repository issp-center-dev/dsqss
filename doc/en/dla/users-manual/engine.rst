.. highlight:: none

Directed loop algorithm solvers, ``dla_H`` and ``dla_B``
========================================================

``dla_H`` and ``dla_B`` are quantum Monte Carlo solvers with the directed loop algorithm.
They take an input file as the command line argument.
``dla_H`` is for spin system and ``dla_B`` is for Bosonic system.

::

  $ dla_H param.in


DSQSS/DLA implements random number parallelization by using MPI.
Since each process performs ``NSET`` Monte Carlo calculation independently,
the total number of MC sets increases :math:`N_\text{process}`
and thus it is expected that the statistical error also reduces to :math:`1/\sqrt{N_\text{process}}` times.

::

  $ mpiexec -np 4 dla_B param.in

