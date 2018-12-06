.. -*- coding: utf-8 -*-
.. highlight:: none

Calculation of the energy on spin chain by DSQSS/PMWA
======================================================
In this tutorial, you learn how to use DSQSS / PMWA by calculating the ground state energy of antiferromagnetic Heisenberg chain of S = 1/2.

The calculation by DSQSS / PMWA is done in the following three stages:

1. Make an input file,
2. Perform calculation,
3. Interpretation of calculation result.




Make an input file
******************************************

To run DSQSS/PMWA, following two input files are needed:

1. lattice definition file,
2. parameter file.

The utility tool ``dsqss_pre.py`` for doing automatically these process is prepared.
This tool is made by a Python script that generates input files for DSQSS/DLA and DSQSS/PMWA from a single input file.
First of all, prepare a text file ``std.in`` with the following contents as an input file for ``dsqss_pre.py``. 

::

  [System]
  solver = PMWA
  [Hamiltonian]
  model_type = spin
  Jxy = -1.0
  Jz = -1.0
  Gamma = 0.1
  [Lattice]
  lattice_type = square
  D = 1
  L = 16
  Beta = 100
  [Parameter]
  CB = 1
  SEED = 31415
  NSET = 10
  NMCS = 100
  NPRE = 100
  NTHERM = 100
  NDECOR = 100

This file is located in ``sample/pmwa/1DDimer`` directory.
Next, type the following command:

::

  $ $DSQSS_ROOT/bin/dsqss_pre.py -i std.in

Then, input files ``param.in`` and ``lattice.xml`` are generated.

Perform calculation
*******************

After making input files, the calculation by  ``DSQSS/PMWA`` starts by typing the following command:
::

  $ $DSQSS_ROOT/bin/pmwa_H param.in

Random numbers parallel computation is possible by using MPI (it is also possible to perform parallel computation by dividing space/imaginary time. Please see DLA user manual for details ). 

::

  $ mpiexec -np 4 $DSQSS_ROOT/bin/pmwa_H param.in

Calculation can be done independently for parallel number (4 for the above example), and the accuracy can be improved by increasing the number of Monte Carlo samples.

Interpretation of calculation result
************************************

The calculation result is written to the output file ``sample.log``.
The energy is obtained by typing the following command:
::

   $ grep ene sample.log
   R ene  = -0.5705441 0.0003774399737579577

In the case of DSQSS / PMWA, it is necessary to insert a transverse magnetic field. Therefore, please note that extrapolation is necessary to obtain the result at zero magnetic field.
