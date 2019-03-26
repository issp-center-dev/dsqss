.. highlight:: none

Utility tools to generate the input files of DSQSS/DLA
=======================================================

DSQSS/DLA needs several XML-formatted input files:
lattice file, algorithm file, structure file, real space temperature Green's function file, and momentum space temperature Green's function file.
By writing these files, you can simulate any model on any lattice.
Since they are a little complicated, DSQSS/DLA prepares utility tools for generating them.

Simple mode tool ``dla_pre``
***********************************
``dla_pre`` is a utility tool to generate :ref:`dla_expert_files` from :ref:`simple_mode_file`. ::

  $ dla_pre [-p paramfile] <inputfile>

The meanings of the parameters are following:

``paramfile``
  Name of the output parameter file (default: ``qmc.inp``.)

``inputfile``
  Name of the input file.
  For details of the input file, see :ref:`simple_mode_file`.


Names of the XML files such as the lattice XML file will be detected from the input file.


.. _dla_lat_gen:

Lattice file generator ``dla_latgen``
*************************************
``dla_latgen`` is a utility tool to generate :ref:`lattice_data_file` and :ref:`lattice_toml_file` from :ref:`simple_mode_file`.::

  $ dla_latgen [-o datafile] [-t TOML] [-g GNUPLOT] input

The meanings of the parameters are following:

``datafile``
  Name of the output lattice datafile (default: ``lattice.dat``.)
  If empty, this does not generate the lattice datafile.

``TOML``
  Name of the output lattice XML file (default: empty.)
  If empty, this does not generate the lattice XML file.

``GNUPLOT``
  Name of the output lattice Gnuplot file (default: empty.)
  If empty, this does not generate the lattice Gnuplot file.
  Users can see the generated lattice by load the lattice Gnuplot file in ``gnuplot``.

``inputfile``
  Name of the input file.
  For details of the input file, see :ref:`std_toml_lattice`.

An example of the input::

   # 8 sites chain
   [lattice]
   lattice = "hypercubic"
   dim = 1
   L = 8

   # square with 4x4 sites
   [lattice]
   lattice = "hypercubic"
   dim = 2
   L = 4

   # ladder with 8x2 sites
   [lattice]
   lattice = "hypercubic"
   dim = 2
   L = [8,2]
   bc = [true, false]


Hamiltonian file generator ``dla_hamgen``
***************************************************

``dla_hamgen`` is a utility tool generating :ref:`hamiltonian_file` from :ref:`simple_mode_file`.::

  $ dla_hamgen [-o filename] <inputfile>

The meanings of the parameters are following:

``filename``
  Name of the output file (default: ``hamiltonian.toml``.)

``inputfile``
  Name of the input file.
  For details of the input file, see :ref:`std_toml_hamiltonian`

An example of the input ::

   # S=1/2 AF Heisenberg model
   [hamiltonian]
   model = "spin"
   M = 1
   Jz = -1.0
   Jxy = -1.0
    
   # S=1 J1 AF J2 FM XY model under the field
   [hamiltonian]
   model = "spin"
   M = 2
   Jxy = [-1.0, 1.0]
   h = 1.0

   # hardcore boson
   [hamiltonian]
   model = "boson"
   M = 1
   t = 1.0
   V = 1.0

   # softcore boson (upto N=2)
   [hamiltonian]
   model = "boson"
   M = 2
   t = 1.0
   U = 1.0
   V = 1.0
   mu = 1.0


Parameter file generator  ``dla_pgen``
******************************************
``dla_pgen`` is a utility tool to generate :ref:`expert_param_file` from :ref:`simple_mode_file`.
::

  $ dla_pgen [-o filename] <inputfile>

The meanings of the parameters are following:

``filename``
  Name of the output file (default: ``param.in``.)

``inputfile``
  Name of the input file.
  For details of the input file, see :ref:`std_toml_parameter`.


Wavevector file generator ``dla_wvgen``
*********************************************
``dla_wvgen`` is a utility tool to generate :ref:`wavevector_file` from :ref:`simple_mode_file`.
::

  $ dla_wvgen [-o filename] [-s size] <inputfile>

The meanings of the parameters are following:

``filename``
  Name of the output file (default: ``kpoints.dat``.)

``size``
  The size of lattice specified as integers separated by spaces (e.g. ``-s "4 4"``.) 
  If omitted, the size of lattice will be read from ``[lattice]`` table of the input file.

``inputfile``
  Name of the input file.
  For details of the input file, see :ref:`simple_mode_kpoints`.


Expert files generator ``dla_alg``
*************************************
``dla_alg`` is a utility tool to generate the expert input files, 
:ref:`lattice_xml_file`, :ref:`algorithm_xml_file`, :ref:`wavevector_xml_file`, and :ref:`relative_coordinate_xml_file`
from the standard input files,
:ref:`lattice_data_file`, :ref:`lattice_toml_file`, :ref:`hamiltonian_file`, and :ref:`wavevector_file`.
::

   $ dla_alg [-l LAT] [-h HAM] [-L LATXML] [-A ALGXML]
             [--without_lattice] [--without_algorithm] [-k KPOINT]
             [--wv WV] [--disp DISP] [--distance-only]
             [--kernel KERNEL]


The meanings of the parameters are following:

``LAT``
  Name of the input lattice data/TOML file (default: ``lattice.dat``.)

``HAM``
  Name of the input Hamiltonian TOML file (default: ``hamiltonian.toml``.)

``LATXML``
  Name of the output lattice XML file (default: ``lattice.xml``.)

``ALGXML``
  Name of the output algorithm XML file (default: ``algorithm.xml``.)

``without_lattice``
  If set, the lattice XML file will not be generated.
  Even this case, the lattice data/TOML file ``LAT`` is required to generate other files.

``without_algorithm``
  If set, the algorithm XML file will not be generated.

``KPOINT``
  Name of the input wavevector file.
  If omitted, the wavevector XML file will not be generated.

``WV``
  Name of the output wavevector XML file (default: ``wavevector.xml``.)

``DISP``
  Name of the output relative coordinate XML file.
  If omitted, the relative coordinate XML file will not be generated.

``--distance-only``
  If set, the relative coordinate XML uses the norm of the displacement vector :math:`r_{ij}` instead of the vector itself :math:`\vec{r}_ij`.

``KERNEL``
  Name of the algorithm to calculate scattering probabilities of worm heads at vertices (default: ``suwa todo``.)
  For available algorithms, see :ref:`simple_mode_algorithm`.
