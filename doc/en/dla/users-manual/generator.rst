.. highlight:: none

Input file generators for DSQSS/DLA
====================================

DSQSS/DLA takes several input files;
lattice XML file, algorithm XML file, and wavevector XML file,
and users can simulate any model on any lattice (graph) by defining these files properly.
They are, however, too complicated to be made by hand.
To help users, DSQSS/DLA offers utility tools for generating these files for widely used lattices and models
such as a hypercubic lattice and the Heisenberg model.

.. _dla_pre:

Simple mode tool ``dla_pre``
***********************************
``dla_pre`` is a utility tool for generating :ref:`dla_expert_files` from :ref:`simple_mode_file`. ::

  $ dla_pre [-p paramfile] <inputfile>

The meanings of the parameters are following.

``paramfile``
  The name of the parameter file to be generated (default: ``param.in``.)

``inputfile``
  The name of the input file.
  For details of the input file, see :ref:`simple_mode_file` .


The names of the XML files such as the lattice XML file are automatically determined from parameters.


.. _dla_lat_gen:

Lattice file generator ``dla_latgen``
*****************************************
``dla_latgen`` is a utility tool for generating :ref:`lattice_data_file` or :ref:`lattice_toml_file` from :ref:`simple_mode_file` . ::

  $ dla_latgen [-o datafile] [-t TOML] [-g GNUPLOT] input

The meanings of the parameters are following.

``datafile``
  The name of the generated lattice data file (default: ``lattice.dat`` .)
  If empty, ``dla_latgen`` never generate any lattice data file.

``TOML``
  The name of the generated lattice TOML file (default: empty.)
  If empty, ``dla_latgen`` never generate any lattice TOML file.

``GNUPLOT``
  The name of the generated lattice Gnuplot file (default: empty.)
  If empty, ``dla_latgen`` never generate any lattice Gnuplot file.
  Users can see the generated lattice by ``load`` the lattice Gnuplot file in ``gnuplot``.

``inputfile``
  The name of the input file.
  For details of the input file, see :ref:`std_toml_lattice` .

Examples
::

  # 1d chain with 8 sites
  [lattice]
  lattice = "hypercubic"
  dim = 1
  L = 8

::

  # 2d square, 4x4 sites
  [lattice]
  lattice = "hypercubic"
  dim = 2
  L = 4

::

  # two leg ladder, 8x2 sites
  [lattice]
  lattice = "hypercubic"
  dim = 2
  L = [8,2]
  bc = [true, false]



Hamiltonian file generator ``dla_hamgen``
**************************************************
``dla_hamgen`` is a utility tool for generating :ref:`hamiltonian_file` from :ref:`simple_mode_file` ::

  $ dla_hamgen [-o filename] <inputfile>

The meanings of the parameters are following.

``filename``
  The name of the generated Hamiltonian file (default: ``hamiltonian.toml`` .)

``inputfile``
  The name of the input file.
  For details of the input file, see :ref:`std_toml_hamiltonian` .

Example ::

   # S=1/2 AF Heisenberg model
   [hamiltonian]
   model = "spin"
   M = 1
   Jz = -1.0
   Jxy = -1.0

::
    
   # S=1 J1 AF J2 FM XY model under the field
   [hamiltonian]
   model = "spin"
   M = 2
   Jxy = [-1.0, 1.0]
   h = 1.0

::

   # hardcore boson
   [hamiltonian]
   model = "boson"
   M = 1
   t = 1.0
   V = 1.0

::

   # softcore boson (upto N=2)
   [hamiltonian]
   model = "boson"
   M = 2
   t = 1.0
   U = 1.0
   V = 1.0
   mu = 1.0


Parameter file generator ``dla_pgen``
******************************************
``dla_pgen`` is utility tool for generating :ref:`expert_param_file` from :ref:`simple_mode_file` .
::

  $ dla_pgen [-o filename] <inputfile>

The meanings of the parameters are following.

``filename``
  The name of the generated parameter file (default: ``param.in`` .)

``inputfile``
  The name of the input file. 
  For details of the input file, see :ref:`std_toml_parameter` .


Wavevector file generator ``dla_wvgen``
**********************************************
``dla_wvgen`` is a utility tool for generating :ref:`wavevector_file` from :ref:`simple_mode_file` .
::

  $ dla_wvgen [-o filename] [-s size] <inputfile>

The meanings of the parameters are following.

``filename``
  The name of the generated wavevector file (default: ``kpoints.dat`` .)

``size``
  Space separated integers denoting the lattice size (e.g., ``-s "4 4"`` .)
  If omitted, it will be detected from the ``[lattice]`` table of the input TOML file.

``inputfile``
  The name of the input file.
  For details of the input file, see :ref:`std_toml_hamiltonian` .



Algorithm file generator ``dla_alg``
*********************************************
``dla_alg`` is a utility tool for generating
:ref:`lattice_xml_file`, :ref:`algorithm_xml_file`, :ref:`wavevector_xml_file`, and :ref:`relative_coordinate_xml_file`
from :ref:`lattice_data_file`, :ref:`lattice_toml_file`, :ref:`hamiltonian_file`, and :ref:`wavevector_file` .
::

   $ dla_alg [-l LAT] [-h HAM] [-L LATXML] [-A ALGXML]
             [--without_lattice] [--without_algorithm] [-k KPOINT]
             [--wv WV] [--disp DISP] [--distance-only]
             [--kernel KERNEL]

The meanings of the parameters are following.

``LAT``
  The name of the lattice dat/TOML file (default: ``lattice.dat``.) 
  The type whether dat or TOML is automatically detected.

``HAM``
  The name of the Hamiltonian TOML file (default: ``hamiltonian.toml`` .)

``LATXML``
  The name of the generated lattice XML file (default: ``lattice.xml`` .)

``ALGXML``
  The name of the generated algorithm XML file (default: ``algorithm.xml`` .)

``without_lattice``
  If set, ``dla_alg`` never generate a lattice XML file.
  Even in this case, the lattice dat/TOML file is still required.

``without_algorithm``
  If set, ``dla_alg`` never generate an algorithm XML file.

``KPOINT``
  The name of the wavevector file.
  If omit, ``dla_alg`` never generate a wavevector XML file.

``WV``
  The name of the generated wavevector XML file (default: ``wavevector.xml`` .)

``DISP``
  The name of the generated relative coordinate XML file.
  If omit, ``dla_alg`` never generate a relative coordinate XML file.

``--distance-only``
  If set, ``dla_alg`` groups pairs of sites by absolute distance instead of relative coordinate.

``KERNEL``
  The name of the algorithm for calculating the scattering probability of a worm head at a vertex (default: ``"suwa todo"`` .)
  For details, see :ref:`simple_mode_algorithm` .
