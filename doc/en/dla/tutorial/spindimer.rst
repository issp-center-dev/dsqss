Energy calculation of the antiferromagnetic Heisenberg dimer by DSQSS/DLA
==========================================================================

This tutorial gives how to use DSQSS/DLA through a calculation the energy of the :math:`S=1/2` antiferromagnetic Heisenberg dimer :math:`\mathcal{H} = J \vec{S}_1 \cdot \vec{S}_2` .

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

``dsqss_pre.py`` is a utility tool to generate these file from one textfile such as the following (sample/dla/01_spindimer/std.in)
::

  [System]
  solver = DLA
  [Hamiltonian]
  model_type = spin
  M =  1                 # S=1/2
  J = -0.5               # coupling constant, negative for AF, not 1 but 1/2 due to PBC
  F = 0.0                # magnetic field
  [Lattice]
  lattice_type = square  # hypercubic, periodic
  D = 1                  # dimension
  L = 2                  # number of sites along each direction
  Beta = 100             # inverse temperature
  [Parameter]
  nset = 5               # set of Monte Carlo sweeps
  npre = 10              # MCSteps to estimate hyperparameter
  ntherm = 10            # MCSweeps for thermalization
  nmcs = 10              # MCSweeps for measurement

Note that the coupling constant `J` is set not to -1.0 but -0.5
since this tool generates a lattice under the periodic boundary condition
and in this case the lattice has two sites and two bonds in results.

Give this file to dsqss_pre.py as ::

  $ python $DSQSS_ROOT/bin/dsqss_pre.py -i std.in

This generates the following four files:
a parameter file ``param.in``, a lattice file ``lattice.xml``, an algorithm file ``algorithm.xml``, and an auxiliary file for the algorithm file ``hamiltonian.xml``.

Perform QMC calculation
************************

Once input files are prepared, you can perform a quantum Monte Carlo calculation based on the directed loop algorithm (dla) by DSQSS/DLA.
::

  $ $DSQSS_ROOT/bin/dla_H param.in

``dla_H`` is a dla solver for spin systems. If you want to deal with Bose-Hubbard models, please use ``dla_B``.

You can perform random number parallelization by using MPI.
::

  $ mpiexec -np 4 $DSQSS_ROOT/bin/dla_H param.in

By the above command, the total number of Monte Carlo samples times by four (equals to the number of process)
and the obtained statistical error is expected to reduce to half (equals to the inverse square root of the number of process).

Analyze the result
*******************

The result of the calculation is written into a text file ``sample.log``.
Since the energy per site is recorded with a name ``ene``,
you can for example draw this by the ``grep`` command by the following.
::

  $ grep ene sample.log
  R ene = -3.75780000e-01 8.89848302e-04

The two figures stand for the expectation value and the statistical error, respectively.
The result value is compatible with the exact solution, :math:`-3|J|/8 = -0.375|J|`, within the statistical error.
