.. -*- coding: utf-8 -*-
.. highlight:: none

Run DSQSS/PMWA
==============================
After generating the input file, PMWA runs by typing the following command (the name of the input file is assumed as ``param.in``).
Execution file is different depending on the system, i.e. the spin system or the hard core boson system.

1. Spin system

  ``$pmwa_H param.in``
  
2. Hard core boson system

  ``$pmwa_B param.in``

After calculation, one result file and two temporary files for restart mode (evout_sample.log, RND_evout_sample.log) are outputted.

  
Output files
==============================
Here, the parameters specific to PMWA for one result file and two temporary files for restart mode are described.

- Result file
  
  .. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Kind, Name, Description
     P, L, Information for three-dimensional lattice.
     P, DOML, The size of the domain divided by parallelization
     P, DOMBETA, The size of the inverse temperature divided by parallelization
     P, NDIVL, The number of lattice divisions
     P, NTEST, "Number of samples to be tested (For details, see the description of Monte Carlo calculation)"
     R, nver, Number of kinks and worms
     R, nkin, Number of kinks
     R, wndx, Expected value of square of winding number in :math:`x` direction
     R, wndy, Expected value of square of winding number in :math:`y` direction
     R, wndz, Expected value of square of winding number in :math:`z` direction
     R, wnd2, Total number of squares of winding number(wndx+wndy+wndz)
     R, bmxu, Expected value of :math:`S_x` (uniform :math:`\tau` integral)
     R, bmpu, Expected value of :math:`S_+` (uniform :math:`\tau` integral)
     R, bmmu, Expected value of :math:`S_-` (uniform :math:`\tau` integral)
     R, comp, Compressibility
     R, lxmx, Local worm number fluctuation at each site
     I, the maximum number of vertices, Maximum number of vertices
     I, the maximum number of worms, Maximum number of worms

Here, the type is the letter given to the beginning of each line of output, P, R, I indicate Parameter, Result, Information respectively.

- Output files for restart mode
	
PMWA implements the restart function, and if there are two files below, a restart is automatically performed. Below, a brief description of the output contents of each file is described.
	
  1. evout_sample.log
  Files outputted for the number of cycles at the end of computation, world lines information, and vertex information.
  For restart calculation, calculations are performed with the loaded arrangement as the initial condition.
  ::
  
    26 : Number of cycles at the end of calculation
    0 1 : Information on the world line at site :math:`0` in the domain.
    i/N beta, (i+1)/N beta Information on the world line of the section : 0: down, 1: up
    0 0 :Information on the world line 1 at the site 1 in the domain.
    1 1  :Information on the world line 2 at the site 2 in the domain.
    ...
    8 0.056997107 2 1 4  Vertex label, tau, type of vertex, number of world lines, bond number
    9 0.056997107 2 0 5
    44 0.28066013 2 1 3

  Here, the types of vertexes are defined as follows.

  .. csv-table::
    :header-rows: 1
    :widths: 1,4
  
    The type of vertex, Description
    -5, The start and end points in the imaginary time direction in each domain. It does not need to split domain.
    "-4(right), -2(left)", Vertex diagonally across domains. It does not need to split domain.
    "-3(right), -1(left)", Vertex off-diagonally across domains. It does not need to split domain.
    0, On site vertex (except worms).
    1, Two site vertex
    2, Kink.
    3, Two site vertex (next nearest) (it is left for compatibility).
    4, The worm moving at that time (annihilation operator).
    5, The worm stopping at that time (creation operator).
    6, "The worm stopping at that time (not related to neither annihilation or creation operators) or needless vertex."
    7, Marker(for imaginary time correlation function measurement).
  
  2. RNDevout_sample.log
     Files that output binary format for objects generating random numbers. At recalculation, calculations are performed with the random number information which is read as the initial condition.

