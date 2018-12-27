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

     Kind, Name, Description
     P, L, Three-dimensional lattice information
     P, DOML, The domain size divided by parallelization
     P, DOMBETA, The domain size of inverse temperature divided by parallelization
     P, NDIVL, Number of grid divisions
     P, NTEST, Number of samples to be tested
     R, nver, Number of kinks and worms
     R, nkin, Number of kinks
     R, wndx, Expected value of square of winding number in x direction
     R, wndy, Expected value of square of winding number in y direction
     R, wndz, Expected value of square of winding number in z direction
     R, wnd2, Total number of squares of winding number(wndx+wndy+wndz)
     R, bmxu, Expected value of :math:`S_x` (uniform :math:`\tau` integral)
     R, bmpu, Expected value of :math:`S_+` (uniform :math:`\tau` integral)
     R, bmmu, Expected value of :math:`S_-` (uniform :math:`\tau` integral)
     R, comp, Compressibility
     R, lxmx, Local fluctuation about worms at each site
     I, the maximum number of vertices, Maximum number of vertices
     I, the maximum number of worms, Maximum number of worms

  Here, the type means the letter given to the beginning of each line of output, P, R, I indicate Parameter, Result, Information, respectively.

- File for restart calculation

  The PMWA implements the restart function, and if there are two files ``evout_sample.log`` and ``RNDevout_sample.log``, a restart is forcibly performed.
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

  
  Here, the types of vertexes are defined as follows:

  .. csv-table::
     :header-rows: 1
     :widths: 1,4
  
     Kind of vertex, Description
     -5, Start or end points in the imaginary time direction in each domain (valid even without domain division)
     "-2(left) , -4(right)", Diagonal vertex that crosses domains (valid even without domain division)
     "-1(left) , -3(right)", Off-diagonal vertex that crosses domains(kink) (valid even without domain division)
     0, On-site vertex (without worms)
     1, 2-site vertex
     2, Kink
     3, 2-site vertex (next nearest neighbour)(It is left for compatibility)
     4, A worm that moves at that time (annihilation operator (the number of world lines is larger on the side where :math:`\tau`  is larger than this worm))
     5, A worm that moves at that time (creation operator (the number of world lines is larger on the side where :math:`\tau`  is larger than this worm))
     6, "A worm that stops at that time (both creation and annihilation operators, bidirectional list is connected) or an unnecessary vertex (bidirectional list is unconnected)."
     7, A marker (for measuring imaginary-time correlation function)
 
  2. RNDevout_sample.log
	    
     A file in which an object that generates a random number is written as the binary format.
     For recalculation mode, the initial condition is set by reading the above file.
