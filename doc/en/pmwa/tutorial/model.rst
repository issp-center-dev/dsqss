.. -*- coding: utf-8 -*-
.. highlight:: none

 Make an input file for DSQSS/PMWA
===================================

To execute PMWA, a text format input file is required.
There are two kinds of parameters in the input file: parameters specifying calculation conditions and parameters specifying Hamiltonian.


An example of the input file is shown below:

::

	RUNTYPE = 0
	NSET   = 10
	NMCS   = 10000
	NPRE   = 10000
	NTHERM = 10000
	NDECOR = 10000
	SEED  = 31415
	NC    = 0
	NVERMAX = 10000000
	NWORMAX = 1000
	latfile   = lattice.xml
	outfile   = sample.log
	CB      = 2
	G       = 0.3
	U       = 0
	V       = 3
	t       = 1
	MU      = 2
	NMAX    = 1

The meaning of each parameter is as follows.

- Parameter of calculation condition

  .. csv-table::
     :header-rows: 1
     :widths: 2,1,4

     Parameter, type, Remarks
     RUNTYPE, int, "Calculation mode (0: normal calculation, 1: restart calculation)"
     CB, int , "Initial arrangement (0: Vacuum,1: Checker-Board,2: Random)"
     NSET, int, Repetition number of Monte Carlo calculation
     NMCS, int, Number of Monte Carlo sweeps used for physical quantity calculation
     NPRE, int, Number of Monte Carlo steps used for pre-calculation to determine the number of trials to generate worms per sweep
     NMCSE, int, Number of Monte Carlo sweeps used for initial relaxation
     NMCSD, int, Number of Monte Carlo sweeps between measurements
     SEED, int, "If it is 0 or more, seed is actually used, if negative, seed is given with random number."
     NVERMAX, int, "Maximum number of vertexes (default is :math:`10^8` ). There is no upper limit when -1."
     NWORMAX, int, "Maximum number of worms (default is :math:`10^3` ). There is no upper limit when -1."
     SFINPFILE, str, "When input, calculate Structure Factors specified in this file."
     SFOUTFILE, str, "If input, output Structure Factors to the specified file (default is sf.out)."
     Step_x, int, Give the spatial width when computing the correlation function (default: 1).
     Step_k, int, Give the width of wavenumber when computing correlation function of wavenumber representation (default: 1).
     CFOUTFILE, str, "When it is input, it outputs the correlation function to the specified file (default is cf.out)."

Here, NVERMAX and NWORMAX are automatically resized and determined (implemented in ver. 1.2.0).

- Parameter of interactions

PMWA can treat the models for hard-core boson and S=1/2 XXZ system. 
The hamiltonian for hard-core boson system is given by

.. math::
   {\cal H} &= -t_{b} \sum_{\langle i, j\rangle}b_i^{\dagger} b_j + U_{BB}\sum_i n_i(n_i -1)
   +V_{B1}\sum_{\langle i, j\rangle} n_i n_j \\ 
   &+\mu\sum_i n_i-\Gamma\sum_i(b_i^{\dagger}+b_i),

where :math:`\langle i,j \rangle` indicates a nearest-neighbor pair.
For S=1/2 XXZ model, the hamiltonian is given by

.. math::
   {\cal H}^{XXZ} = -J_{xy} \sum_{\langle i, j\rangle}(S_i^x S_j^x + S_i^y S_j^y)-J_z\sum_{\langle i, j\rangle}S_i^zS_j^z-H \sum_{i}S_{i}^z -\Gamma \sum_i S_i^x.

The parameters specified in the input file and the parameters in the above expression correspond as follows.

  .. csv-table::
     :header-rows: 1
     :widths: 1,1,1

     Parameter, Boson, Spin
     t, :math:`t_b`, :math:`J_{xy}` 
     U, :math:`U_{BB}`, `-`
     V, :math:`V_{B1}`, :math:`J_{z}`
     MU, :math:`\mu`, :math:`H`
     G, :math:`\Gamma`, :math:`\Gamma/2` 

Each parameter is specified as double type.
