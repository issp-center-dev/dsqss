.. -*- coding: utf-8 -*-
.. highlight:: none

Output files for DSQSS/PMWA
==============================

DSQSS/PMWA outputs one result file and two temporary files for restart calculation (evout_sample.log, RND_evout_sample.log).

- Result file

  Many parameters are common to DSQSS/DLA. In this section, the parameters which are not included in DSQSS/DLA or where the usage is different from DSQSS/DLA are explained.
    .. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Type, Name, Description
     P, L, The size of the lattice.
     P, DOML, The size of the domain divided by parallelization
     P, DOMBETA, The size of the inverse temperature divided by parallelization
     P, NDIVL, The number of lattice divisions
     P, NTEST, "Number of samples to be tested (for details, see the description of Monte Carlo calculation)"
     R, nver, The number of kinks and worms
     R, nkin, The number of kinks
     R, wndx, The expected value of square of winding number in :math:`x` direction
     R, wndy, The expected value of square of winding number in :math:`y` direction
     R, wndz, The expected value of square of winding number in :math:`z` direction
     R, wnd2, The total number of squares of winding number(wndx+wndy+wndz)
     R, bmxu, The expected value of :math:`S_x` (uniform :math:`\tau` integral)
     R, bmpu, The expected value of :math:`S_+` (uniform :math:`\tau` integral)
     R, bmmu, The expected value of :math:`S_-` (uniform :math:`\tau` integral)
     R, comp, The compressibility
     R, lxmx, The local worm number fluctuation at each site
     I, the maximum number of vertices, The maximum number of vertices
     I, the maximum number of worms, The maximum number of worms

  Here, the type means the letter given to the beginning of each line of output. P, R, I indicate Parameter, Result, Information, respectively.

- File for restart calculation

  PMWA implements the restart function, and if there are two files ``evout_sample.log`` and ``RNDevout_sample.log``, a restart is forcibly performed.
  In the following, the output contents of each file are briefly described.
	
  1. evout_sample.log
    
  In this file, the number of cycles at the end of computation, information on world lines, and vertex information are outputted.
  In restart calculation, the loaded arrangement is set as the starting condition.

  ::
  
    26 : Number of cycles at the end of calculation.
    0 1 : Information on the world line at 0-th site in the domain.
    Information on the world line of the section between i/N beta, (i+1)/N beta; 0: down, 1: up
    0 0 : Information on the world line at the 1st site in the domain.
    1 1 : Information on the world line at the 2nd site in the domain.
    ...
    8 0.056997107 2 1 4 :lavel for vertex, tau, kind of vertex, Number of world lines, Bond number
    9 0.056997107 2 0 5
    44 0.28066013 2 1 3

  
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
     
     The output binary file for objects generating random numbers. At recalculation, calculations are performed with the random number information which is read as the initial condition.
