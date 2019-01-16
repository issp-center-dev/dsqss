.. -*- coding: utf-8 -*-
.. highlight:: none

Make an input file for a lattice
==================================

In PMWA, as a simple tool ``lattgene`` is prepared for generating a lattice definition file ``lattice.xml`` for a standard model. In this section, the usage of 
``lattgene`` is explained.

``lattgene`` generates a lattice definition file for a cubic lattice.
The parameters to be specified are as follows.

.. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Parameter, type, Remarks
     D, int, The dimensions
     L, int, The size of the lattice (specified for each dimension consecutively)
     B, double, The inverse temperature
     NLdiv, int, The number of divisions of L (divide NLdiv for each dimension)
     NBdiv, int, The number of divisions of L
     NFIELD, int, The type of magnetic field (basically set to 0)


Examples of use are described below.

1. One-dimensional 8-sites chain, inverse temperature 10.0, definition of lattice file when division number is 1:

   ``$ lattgene 1 8 10.0 1 1 0``

2. Two dimensional 4*4 sites lattice, reverse temperature 10.0, definition of lattice file when division number is 1:

   ``$ lattgene 2 4 4 10.0 1 1 0``

