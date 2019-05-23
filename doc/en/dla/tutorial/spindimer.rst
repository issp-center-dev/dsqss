Energy calculation of the antiferromagnetic Heisenberg dimer by DSQSS/DLA
==========================================================================

This tutorial gives how to use DSQSS/DLA through a calculation the energy of the :math:`S=1/2` antiferromagnetic Heisenberg dimer :math:`\mathcal{H} = -J \vec{S}_1 \cdot \vec{S}_2` .

DSQSS/DLA calculation has the following three parts:

1. Prepare the input files
2. Perform QMC calculation
3. Analyze the result


Prepare the input files
************************

DSQSS/DLA requires the following input files:

#. Parameter file
#. lattice file
#. algorithm file

``dla_pre`` is a utility tool to generate these files from one textfile such as the following (sample/dla/01_spindimer/std.toml)
::

  [hamiltonian]
  model = "spin"
  M =  1                 # S=1/2
  Jz = -1.0              # coupling constant, negative for AF
  Jxy = -1.0             # coupling constant, negative for AF
  h = 0.0                # magnetic field
  [lattice]
  lattice = "hypercubic" # hypercubic, periodic
  dim = 1                # dimension
  L = 2                  # number of sites along each direction
  bc = false             # open boundary
  [parameter]
  beta = 100             # inverse temperature
  nset = 5               # set of Monte Carlo sweeps
  npre = 10              # MCSteps to estimate hyperparameter
  ntherm = 10            # MCSweeps for thermalization
  nmcs = 100             # MCSweeps for measurement
  seed = 31415           # seed of RNG


Give this file to ``dla_pre`` as ::

  $ $DSQSS_ROOT/bin/dla_pre std.in

This generates the following four files:
a parameter file ``param.in``, a lattice file ``lattice.xml``, an algorithm file ``algorithm.xml``.

Perform QMC calculation
************************

Once input files are prepared, you can perform a quantum Monte Carlo calculation based on the directed loop algorithm (dla) by DSQSS/DLA.
::

  $ $DSQSS_ROOT/bin/dla param.in

You can perform random number parallelization by using MPI. [#fn_ompi_macos]_
::

  $ mpiexec -np 4 $DSQSS_ROOT/bin/dla param.in

By the above command, the total number of Monte Carlo samples times by four (equals to the number of process)
and the obtained statistical error is expected to reduce to half (equals to the inverse square root of the number of process).

Analyze the result
*******************

The result of the calculation is written into a text file ``sample.log``.
Since the energy per site is recorded with a name ``ene``,
you can for example draw this by the ``grep`` command by the following.
::

  $ grep ene sample.log
  R ene = -3.74380000e-01 5.19493985e-03

The two figures stand for the expectation value and the statistical error, respectively.
The result value is compatible with the exact solution, :math:`-3|J|/8 = -0.375|J|`, within the statistical error.

.. only:: html

   .. rubric:: Footnote

.. [#fn_ompi_macos] After finishing DSQSS/DLA, the OpenMPI on macOS may say an error message, ``No such file or directory (errno 2)``. 
   You can ignore this error safely.
   If you're annoyed by it, please put an extra option ``--mca shmem posix`` to ``mpiexec``.
