.. -*- coding: utf-8 -*-
.. highlight:: none

Input files for DSQSS/PMWA
==============================

In the input file of DSQSS/DLA and DSQSS/PMWA, there are many common parameters. In this section, the parameters which are not included in DSQSS/DLA or where the usage is different from DSQSS/DLA are explained.

- Parameters for calculation conditions

.. csv-table::
     :header-rows: 1
     :widths: 2,1,1,4

     Parameter, type, default, description
     RUNTYPE, int, , "Calculation mode (0: from scratch, 1: restart)"
     CB, int , 0, "Initial pattern (0: Vacuum, 1: Checker-Board, 2: Random)"
     NWORMAX, int, 1000, "The maximum number of worms. When the value is equal to be -1, the upper limit becomes infinity."
     Step_x, int, 1, "The spatial width in calculating the correlation functions."
     Step_k, int, 1, "The width of the wave-number space in calculating correlation functions of wave-number representation."

- Parameters related to the model

.. csv-table::
     :header-rows: 1
     :widths: 2,1,4

     Parameter, type, description 
     t, double, " :math:`t` in boson system. :math:`J_{xy}` in spin system."
     U, double, " :math:`U` in boson system. This parameter is not used in spin system."
     V, double, " :math:`V` in boson system. :math:`J_{z}` in spin system."
     MU, double,  " :math:`\mu` in boson system. :math:`H`  in spin system."
     G, double, " :math:`\Gamma` in boson system. :math:`\Gamma/2` in spin system."
     NMAX, \-, "This value is not used (always fixed to be 1)."


An example of the input file is shown as follows:
::

  RUNTYPE = 0
  NSET  = 10
  NMCS  = 1000
  NPRE = 1000
  NTHERM = 1000
  NDECOR = 1000
  SEED  = 31415
  NC    = 0
  NVERMAX = 10000000
  NWORMAX = 1000
  algfile   = algorithm.xml
  latfile   = lattice.xml
  outfile   = sample.log
  CB      = 2
  G       = 0.3
  U       = 0
  V       = 3
  t       = 1
  MU      = 2
  NMAX    = 1

