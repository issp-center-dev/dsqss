.. highlight:: none

Directed loop algorithm solver ``dla``
========================================================

``dla`` is a quantum Monte Carlo solver with the directed loop algorithm.
This takes an input file as the command line argument.::

  $ dla param.in


DSQSS/DLA implements random number parallelization by using MPI.
Since each process performs ``NSET`` Monte Carlo calculation independently,
the total number of MC sets increases :math:`N_\text{process}`
and thus it is expected that the statistical error also reduces to :math:`1/\sqrt{N_\text{process}}` times.

::

  $ mpiexec -np 4 dla param.in

