.. highlight:: none

Standard input files for DSQSS/DLA
======================================

List of files
************************

.. csv-table::
    :header-rows: 0
    :widths: 1,4

    lattice.dat, "格子ファイル."
    lattice.toml, "格子TOMLファイル."
    hamiltonian.toml, "ハミルトニアンファイル."
    kpoints.dat, "波数ファイル."


.. _lattice_data_file:

Lattice datafile ``lattice.dat``
**************************************
A lattice datafile is a textfile describing information of space (lattice), such as the number of sites and links connecting some sites.
This file is used as an input file of ``dla_alg``.

From ``#`` mark to the end of the line is a comment and ignored.
A blank line is also ignored.
A "list" is written as spece separated elements in one line like ``2 2``.

The lattice datafile includes the five sections, 
``name``, ``lattice``, ``directions``, ``sites``, ``interactions``.


``name``
   The name of lattice.
   This is just a comment or marker, and will not be used.

``lattice``
   The section specifying general information of lattice.

   - First line
      - An integer denoting dimension of the lattice.
   - Second line
      - A list of integers denoting the size of the lattice. 
   - Third line
      - A list of integers denoting the boundary condition of the lattice. ``1`` means the periodic boundary condition and ``0`` means the open one.
   - Rest
      - A list of figures denoting the primitive translation vector :math:`\vec{e}^i`.
        The first integer means the index of the vector :math:`i` and the rest floating point numbers mean coordinates of the vector in Cartesian, :math:`e_d^i`.


``directions``
   The section specifying directions of bonds (two-body interactions.)

   - First line
      - An integer denoting the number of the directions.
   - Rest
      - A list of figures denoting the a direction.
        The first integer means the index of the direction and the rest floating point numbers mean coordinates.
        The coordinates is represented by using the basis specified in ``lattice``.

``sites``
   The section specifying sites.

   - First line
      - An integer denoting the number of the sites.
   - Rest
      - A list of figures denoting a site.
        The first two integers mean the index and the type of the site, respectively.
        The rest floating point numbers mean the coordinates in the basis specified in ``lattice``.

``interactions``
   The section specifying interactions.

   - First line
      - An integer denoting the number of the interactions.
   - Rest
      - A list of :math:`N = \text{involved sites} + 5` integers. The details are following.

.. csv-table::
    :header-rows: 1
    :widths: 1,4

    column, description
    1, The index of the interaction.
    2, The type of the interaction.
    3, The number of involved sites.
    4 ... (N-2), The indices of the involved sites.
    N-1, "If the bond crosses the boundary, this is 1. Otherwise, 0."
    N, The index of the direction.


The following is an example describing :math:`4 \times 4` square lattice::

   name
   2 dimensional hypercubic lattice

   lattice
   2 # dim
   4 4 # size
   1 1 # 0:open boundary, 1:periodic boundary
   0 1.0 0.0 # latvec_0
   1 0.0 1.0 # latvec_1

   directions
   2 # ndirections
   # id, coords...
   0 1.0 0.0 
   1 0.0 1.0 

   sites
   16 # nsites
   # id, type, coord...
   0 0 0 0
   1 0 1 0
   2 0 2 0
   3 0 3 0
   4 0 0 1
   5 0 1 1
   6 0 2 1
   7 0 3 1
   8 0 0 2
   9 0 1 2
   10 0 2 2
   11 0 3 2
   12 0 0 3
   13 0 1 3
   14 0 2 3
   15 0 3 3

   interactions
   32 # nints
   # id, type, nbody, sites..., edge_flag, direction
   0 0 2 0 1 0 0
   1 0 2 0 4 0 1
   2 0 2 1 2 0 0
   3 0 2 1 5 0 1
   4 0 2 2 3 0 0
   5 0 2 2 6 0 1
   6 0 2 3 0 1 0
   7 0 2 3 7 0 1
   8 0 2 4 5 0 0
   9 0 2 4 8 0 1
   10 0 2 5 6 0 0
   11 0 2 5 9 0 1
   12 0 2 6 7 0 0
   13 0 2 6 10 0 1
   14 0 2 7 4 1 0
   15 0 2 7 11 0 1
   16 0 2 8 9 0 0
   17 0 2 8 12 0 1
   18 0 2 9 10 0 0
   19 0 2 9 13 0 1
   20 0 2 10 11 0 0
   21 0 2 10 14 0 1
   22 0 2 11 8 1 0
   23 0 2 11 15 0 1
   24 0 2 12 13 0 0
   25 0 2 12 0 1 1
   26 0 2 13 14 0 0
   27 0 2 13 1 1 1
   28 0 2 14 15 0 0
   29 0 2 14 2 1 1
   30 0 2 15 12 1 0
   31 0 2 15 3 1 1


Lattice TOML file ``lattice.toml``
*************************************
A lattice TOML file is a `TOML`_ formatted file defining a lattice by using an unitcell and primitive translation vectors.
This is used as an input file of ``dla_alg``.

This has two tables, ``parameter`` and ``unitcell``.

``parameter``
   A table denoting general information of the lattice.

   ``parameter.name``
      A string denoting the name of lattice.

   ``parameter.L``
      An array of integers denoting the size of lattice.

   ``parameter.bc``
      An array of booleans denoting the boundary condition of lattice.
      ``true`` means the periodic boundary condition and ``false`` means the open one.

   ``parameter.basis``
      An two-dimensional array (array of array) of floating point numbers denoting the primitive translation vectors.

``unitcell``
   A table describing a unitcell.

   ``unitcell.sites``
      An array of tables denoting sites in one unitcell.
      One table corresponding one site.

      ``unitcell.sites.siteid``
         An integer denoting the local index of the site in one unitcell.

      ``unitcell.sites.type``
         An integer denoting the type of the site.

      ``unitcell.sites.coord``
         An array of floating point numbers denoting the coordinate of site in one unitcell.

   ``unitcell.bonds``
      An array of tables denoting bonds in one unitcell.
      One table corresponding one bond.

      ``unitcell.bonds.bondid``
         An integer denoting the local index of the bond in one unitcell.

      ``unitcell.bonds.type``
         An integer denoting the type of the bond.

      ``unitcell.bonds.source``
         A table denoting an end (source site) of the bond.

         ``unitcell.bonds.source.siteid``
            An integer denoting the local index of the source site.

      ``unitcell.bonds.target``
         A table denoting the other end (target site) of the bond.

         ``unitcell.bonds.target.siteid``
            An integer denoting the local index of the target site.

         ``unitcell.bonds.target.offset``
            An array of integers denoting the relative coordinate of the unitcell where the target site belongs
            from the unitcell where the source site belongs.


The following is an example describing two dimensional square lattice.
::

   [parameter]
   name = "square lattice"
   dim = 2
   L = [4,4]
   bc = [true, true]
   basis = [[1,0], [0,1]]

   [unitcell]

   [[unitcell.sites]]
   siteid = 0
   type = 0
   coord = [0,0]

   [[unitcell.bonds]]
   bondid = 0
   type = 0
   source = { siteid = 0 }
   target = { siteid = 0, offset = [1,0] }
   [[unitcell.bonds]]
   bondid = 1
   type = 0
   source = { siteid = 0 }
   target = { siteid = 0, offset = [0,1] }





.. _hamiltonian_file:

Hamiltonian file ``hamiltonian.toml``
***************************************************
A Hamiltonian file is a `TOML`_ formatted file describing the local Hamiltonian.
This file is used as an input file of ``dla_alg``.
A utility tool ``dla_hamgen`` generates Hamiltonian file for common models, XXZ spin model and Bose-Hubbard model.


``name``
   A string denoting the name of the Hamiltonian.

``sites``
   An array of tables denoting the site Hamiltonians.
   One table corresponds to one type of site hamiltonian.

   ``sites.id``
      An integer denoting the type of site Hamiltonian.

   ``sites.N``
      An integer denoting the number of states of the local degree of freedom.
      For example, for :math:`S=1/2` spin this is 2.

   ``sites.values``
      An array of floating point numbers denoting the elements of local basis such as :math:`S^z`.


   ``sites.elements``
      An array of tables denoting the elements of the Hamiltonian matrix.
      One table corresponds to one element.

      ``sites.elements.istate``
         An integer denoting the index of the initial state (before the Hamiltonian acts on.)

      ``sites.elements.fstate``
         An integer denoting the index of the final state (after the Hamiltonian acts on.)

      ``sites.elements.value``
         A floating point number denoting the matrix element.

   ``sites.sources``
      An array of tables denoting the elements of the source Hamiltonian matrix, which is introduced to create and annihilate worm heads.
      One table corresponds to one element.

      ``sites.elements.istate``
         An integer denoting the index of the initial state (before the Hamiltonian acts on.)

      ``sites.elements.fstate``
         An integer denoting the index of the final state (after the Hamiltonian acts on.)

      ``sites.elements.value``
         A floating point number denoting the matrix element.

``interactions``
   An array of tables denoting the many-body interaction Hamiltonians.
   One table corresponds to one type of interaction.

   ``interactions.id``
      An integer denoting the type of interaction.

   ``interactions.nbody``
      An integer denoting the number of involved sites.

   ``interactions.N``
      An array of integers denoting the number of local states on the involved sites.

   ``interactions.elements``
      An array of tables denoting the elements of the Hamiltonian matrix.
      One table corresponds to one element.

      ``interactions.elements.istate``
         An array of integers denoting the index of the initial state (before the Hamiltonian acts on.)

      ``interactions.elements.fstate``
         An array of integers denoting the index of the final state (after the Hamiltonian acts on.)

      ``interactions.elements.value``
         A floating point number denoting the matrix element.

The following is an example describing :math:`S=1/2` antiferromagnetic Heisenberg spin model
::

   name = "S=1/2 XXZ model"
   [[sites]]
   id = 0
   N = 2
   values = [-0.5, 0.5]
   [[sites.elements]]
   istate = [ 0,]
   fstate = [ 0,]
   value = 0.5

   [[sites.elements]]
   istate = [ 1,]
   fstate = [ 1,]
   value = -0.5

   [[sites.sources]]
   istate = [ 0,]
   fstate = [ 1,]
   value = 0.5

   [[sites.sources]]
   istate = [ 1,]
   fstate = [ 0,]
   value = 0.5


   [[interactions]]
   id = 0
   nbody = 2
   N = [ 2, 2,]
   [[interactions.elements]]
   istate = [ 0, 0,]
   fstate = [ 0, 0,]
   value = 0.25

   [[interactions.elements]]
   istate = [ 0, 1,]
   fstate = [ 0, 1,]
   value = -0.25

   [[interactions.elements]]
   istate = [ 0, 1,]
   fstate = [ 1, 0,]
   value = 0.5

   [[interactions.elements]]
   istate = [ 1, 0,]
   fstate = [ 1, 0,]
   value = -0.25

   [[interactions.elements]]
   istate = [ 1, 0,]
   fstate = [ 0, 1,]
   value = 0.5

   [[interactions.elements]]
   istate = [ 1, 1,]
   fstate = [ 1, 1,]
   value = 0.25





.. _wavevector_file:

Wavevector datafile ``kpoints.dat``
****************************************

A wavevector datafile is a textfile describing wavevectors

.. math::
   \vec{k}^{(i)} = \sum_{d=1}^{D} n_d^{(i)} \vec{g}_d,


where :math:`\vec{g}` is the set of the reciprocal vectors.

From ``#`` mark to the end of the line is a comment and ignored.
A blank line is also ignored.
A "list" is written as spece separated elements in one line like ``2 2``.

The wavevector datafile includes the two sections, ``dim`` and ``kpoints``.

``dim``
   An integer denoting the dimension of the lattice.

``kpoints``
   A section describing wavevectors.

   - First line
      - The number of wavevectors.
   - Rest 
      - A list of integers denoting the wavevector.
        The first integer means the index of the wavevector.
        The rest integers means the coordinates of the wavevector, :math:`n_d`.

ベクトルの基底は逆格子ベクトルです。
``lattice.dat`` などで座標が ``a_d`` と指定されるような格子点 :math:`r` と、
``kpoints.dat`` で ``n_d`` で指定されるような波数 :math:`k` との内積は、
:math:``

正確には、格子の座標が :math:`\vec{r} = \sum r_d \vec{e}_d` で表現されて、
波数が :math:`\vec{k} = \sum k_d \vec{g}_d` で表現されているとき、これらの内積は
:math:`\vec{r}\cdot\vec{k} = \sum_d 2\pi r_d k_d / L_d` となります。
ここで :math:`L_d` は :math:`d` 番目の次元における格子のサイズです。

   
二次元の例を示します. ::

   dim
   2

   kpoints
   3
   0 0 0
   1 2 0
   2 4 0

.. _TOML: https://github.com/toml-lang/toml/blob/master/versions/ja/toml-v0.5.0.md
