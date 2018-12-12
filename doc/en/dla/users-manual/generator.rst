.. highlight:: none

Utility tools to generate the input files of DSQSS/DLA
=======================================================

DSQSS/DLA needs several XML-formatted input files:
lattice file, algorithm file, structure file, real space temperature Green's function file, and momentum space temperature Green's function file.
By writing these files, you can simulate any model on any lattice.
Since they are a little complicated, DSQSS/DLA prepares utility tools for generating them.

Hypercubic lattice generator ``lattgene_C``
*******************************************
``lattgene_C`` is an utility tool to generate a lattice file describing a :math:`D` dimensional hypercubic lattice with the periodic boundary condition.::

  $ lattgene_C [-o filename] D L1 L2 ... LD BETA

The meaning of parameters are following:

``D``
  Dimension of lattice

``L1 L2 ... LD``
  Linear length of lattice in each dimension.

``BETA``
  Inversed temperature.

``filename``
  Name of lattice file (default: ``lattice.xml`` ).

Example::

  ## Chain with 8 sites. Inversed temperature is 10.0.
  $ lattgene_C 1 8 10.0

  ## Square lattice with 6x6 sites. Inversed temperature is 10.0
  ## Name of the generated file is lat.xml
  $ lattgene_C -o lat.xml 2 6 6 10.0

Triangular lattice generator ``lattgene_T``
*******************************************
``lattgene_T`` is an utility tool to generate a lattice file describing a triangular lattice with the periodic boundary condition. ::

  $ lattgene_T [-o filename] L1 L2 BETA

The meaning of parameters are following:

``L1 L2``
  Linear length of lattice in each dimension.

``BETA``

  Inversed temperature.

``filename``
  Name of lattice file (default: ``lattice.xml`` ).

Example::

  ## Triangular lattice with 6x6 sites. Inversed temperature is 10.0
  $ lattgene_T 1 8 10.0


Heisenberg spin Hamiltonian generator ``hamgen_H``
***************************************************

``hamgen_H`` is an utility tool generating a hamiltonian file describing Heisenberg spin model

.. math:
   \mathcal{H} = -J  \sum_{\langle i, j \rangle} S_i \cdot S_j - h \sum_i S_i^z

::

  $ hamgen_H [-o filename] M J F

The meaning of parameters are following:

``M``
  Twice as the length of the local spin, :math:`2S`

``J``
  Coupling constant. Positive for ferromagnetic and negative for antiferromagnetic.

``F``
  Magnetic field on a site per a bond connected to the site, :math:`F = h/z` ,
  where :math:`z` is the coordination number, for example, :math:`z=4` for the square lattice.

``filename``
  Name of hamiltonian file (default: ``hamiltonian.xml`` ).

Example::

  ## S=1/2 antiferromagnetic Heisenberg model without magnetic field.
  $ hamgen_H 1 -1.0 0.0

  ## S=1 ferromagnetic Heisenberg model with magnetic field.
  ## Name of the generated file ham.xml
  $ hamgen_H -o ham.xml 2 1.0 1.0


Bose-Hubbard model generator ``hamgen_B``
******************************************

``hamgen_B`` is an utility tool generating a hamiltonian file describing Bose-Hubbard model

.. math:
   \mathcal{H} = \sum_{\langle i, j \rangle} \left[ -t b_i^\dagger \cdot b_j + V n_i n_j \right] + \sum_i \left[ \frac{U}{2} n_i(n_i-1) - \mu n_i \right]

::

  $ hamgen_B [-o filename] M t V U F

The meaning of parameters are following:

``M``
  Maximum number of sites on a site

``t``
  Hopping parameter

``V``
  Nearest neighbor interaction. Positive for repulsive potential and negative for attractive.

``U``
  Onsite interaction. Positive for repulsive potential and negative for attractive.

``F``
  Chemical potential on a site per a bond connected to the site, :math:`F = h/z` ,
  where :math:`z` is the coordination number, for example, :math:`z=4` for the square lattice.

``filename``
  Name of hamiltonian file (default: ``hamiltonian.xml`` ).

Algorithm file generator ``dla_alg``
*************************************
``dla_alg`` is an utility tool to convert a hamiltonian file to an algorithm file.::

  $ dla_alg HFILE AFILE

The meaning of parameters are following:

``HFILE``
  Hamiltonian file to be loaded (default: ``hamiltonian.xml`` ).

``AFILE``
  Algorithm file to be generated (default: ``algorithm.xml`` ).

Structure factor file generator ``sfgene``
*********************************************
``sfgene`` is an utility tool generating a structure factor file for a hypercubic lattice::

  $ sfgene [-o filename] D L_1 ... L_D Ntau Ntau_cutoff KTYPE

The meaning of parameters are following:

``D``
  Dimension of lattice

``L_1 ... L_D``
  Linear length of lattice in each dimension.

``Ntau``
  The number of discretization of imaginary time

``Ntau_cutoff``
  Maximum number of distance in imaginary time :math:`d\tau`

``KTYPE``
  Pattern of wave vectors :math:`k`

  - ``KTYPE==0``
    
    Wave vectors with :math:`k_x = n\pi/L_x, n = 0, 2, \dots, L` are calculated (:math:`k_y` and :math:`k_z` are zero for all wave vectors).

  - ``KTYPE==1``

    :math:`k/\pi = (0,0,0), (1,0,0), (0,1,0), (1,1,0), \dots, (1,1,1)` for three dimensional case.

``filename``
  Name of generated file (default: ``sf.xml`` ).

Real space temperature Green's function file generator ``cfgene``
*******************************************************************
``cfgene`` is an utility tool generating a real space temperature Green's function file for a hypercubic lattice::

  $ cfgene [-o filename] D L_1 ... L_D Ntau

The meaning of parameters are following:

``D``
  Dimension of lattice

``L_1 ... L_D``
  Linear length of lattice in each dimension.

``Ntau``
  The number of discretization of imaginary time

``filename``
  Name of generated file (default: ``sf.xml`` ).

