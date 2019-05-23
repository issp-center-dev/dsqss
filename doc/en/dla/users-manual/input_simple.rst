.. highlight:: none

Simple mode of DSQSS/DLA
=================================

A simple mode of DSQSS/DLA is the simplest workflow for DSQSS/DLA.
In this mode, users can simulate of a predefined model on a predefined lattice from one text file.
:numref:`fig_flow_dla_simple` shows a workflow of the simple mode.

.. figure:: ../../../image/dla/users-manual/flow_simple.*
  :name: fig_flow_dla_simple
  :alt: Simple mode of DSQSS/DLA.

  Simple mode of DSQSS/DLA. Ellipses are files and rectangles are tools.

.. _simple_mode_file:

Simple mode file ``std.toml``
********************************************
A simple mode file is a textfile written in `TOML`_ format.
This is used as an input file of several tools such as ``dla_pre``.

.. _std_toml_parameter:

``parameter``
+++++++++++++++++
A table specifying simulation parameters such as the inverse temperature.
This table is used in ``dla_pre`` and ``dla_pgen``.

Keys in the ``parameter`` table are following:

.. csv-table::
  :header-rows: 1
  :widths: 1,1,1,4

  name, type, default, description
  beta, double, --, "Inverse temperature."
  npre, int, 1000, "The number of Monte Carlo steps in the pre-calculation phase where the number of creation trials of a pair of worms in one Monte Carlo sweep is defined."
  ntherm, int, 1000, "The number of Monte Carlo sweeps to thermalize the system."
  ndecor, int, 1000, "The number of Monte Carlo sweeps to reduce autocorrelation time between two preceding sets."
  nmcs, int, 1000, "The number of Monte Carlo sweeps to calculate mean values of observables."
  nset, int, 10, "The number of Monte Carlo sets."
  simulationtime, double,  0.0, "Simulation time in second."
  seed, int, 198212240, "The seed of the random number generator."
  nvermax, int,  10000, "The maximum number of vertices."
  nsegmax, int,  10000, "The maximum number of world-line segments."
  algfile, string,  algorithm.xml, "The filename of an algorithm file."
  latfile, string, lattice.xml, "The filename of a lattice file."
  wvfile, string, --,  "A wavevector XML file. If it is an empty string, observables as functions of wavevector will not be calculated."
  dispfile, string,  --, "A relative coordinates XML file. If it is an empty string, observables as functions of relative coordinates will not be calculated."
  outfile, string, sample.log, "The name of the main result file."
  sfoutfile, string, sf.dat, "The name of the structure factor result file."
  cfoutfile, string, cf.dat, "The name of the real space temperature Green's function output file."
  ckoutfile, string, ck.dat, "The name of the momentum space temperature Green's function output file."

- About simulationtime

  - When simulationtime > 0.0

  - DSQSS/DLA loads the checkpoint file and resumes the simulation if the checkpoint file exists.
    - If not, DSQSS/DLA starts a new simulation.
  - After the time specified by "simulationtime" (in seconds) has elapsed, DSQSS/DLA saves the state of the simulation into the checkpoint file and halts the simulation.
  - The name of the checkpoint file is that of the main result file with a suffix ".cjob".

  - When simulationtime <= 0.0

    - The checkpoint file is ignored. DSQSS/DLA never saves nor loads it.


.. _std_toml_lattice:

``lattice``
+++++++++++++++
A table specifying information of lattice.
This is used in ``dla_pre`` and ``dla_latgen``.

Keys in the ``lattice`` table are following:

.. csv-table::
  :header-rows: 1
  :widths: 1,1,1,4

  name, type, default, description
  lattice, string, --, "The type of lattice."
  dim, int, --, Dimension.
  L, list(int) or int, --, "The size of the lattice. Specified by an integer or a list of integers. If the number of elements is less than the dimension, missing elements are filled by the last element of the given list."
  bc, list(bool) or bool, true, "The boundary condition of the lattice. Specified by a boolean or a list of booleans. ``true`` means the periodic boundary condition, ``false`` means the open boundary condition."

Available lattices are following.

hypercubic
  A hyper cubic lattice with arbitrary dimension.
  By using ``bc``, users can generate ladder or slab lattices.

triangular
  A two dimensional triangular lattice.

honeycomb
  A two dimensional honeycomb lattice.

kagome
  A two dimensional kagome lattice.

.. _std_toml_hamiltonian:

``hamiltonian``
++++++++++++++++++++

A table specifying information of Hamiltonian.
This table is used in ``dla_pre`` and ``dla_hamgen``.

Keys in the ``hamiltonian`` table are following:

.. csv-table::
  :header-rows: 1
  :widths: 1,1,1,4

  name, type, default, description
  model, string, --, "The type of the model. 'spin' means XXZ spin model and 'boson' means Bose-Hubbard model."
  M, int, 1, "For XXZ model twice as the length of the local spin, :math:`M=2S`, and for Bose-Hubbard model the cutoff of the number of particles on a site."


XXZ model

.. math::
  \mathcal{H} = \sum_{\langle i, j \rangle} -J_z S_i^z S_j^z -\frac{J_{xy}}{2} \left( S_i^+ S_j^- + S_i^- S_j^+ \right)
  + D \sum_i \left(S_i^z\right)^2
  - h \sum_i S_i^z

has the following parameters:

.. csv-table::
  :header-rows: 1
  :widths: 1,1,1,4

  name, type, default, description
  Jz, list(float) or float, 0.0, "The exchange interaction. Positive for ferromagnetic and negative for antiferromagnetic."
  Jxy, list(float) or float, 0.0, "The exchange interaction. Positive for ferromagnetic and negative for antiferromagnetic."
  D, list(float) or float, 0.0, "The onsite uniaxial anisotropy."
  h, list(float) or float, 0.0, "The magnetic field."


Bose-Hubbard model

.. math::
  \mathcal{H} = \sum_{\langle i, j \rangle} \left[ -t b_i^\dagger \cdot b_j + h.c. + V n_i n_j \right] + \sum_i \left[ \frac{U}{2} n_i(n_i-1) - \mu n_i \right]

has the following parameters:

.. csv-table::
  :header-rows: 1
  :widths: 1,1,1,4

  name, type, default, description
  t, list(float) or float, 0.0, "The hopping parameter."
  V, list(float) or float, 0.0, "The offsite interaction. Positive for repulsion and negative for attraction."
  U, list(float) or float, 0.0, "The onsite interaction. Positive for repulsion and negative for attraction."
  mu, list(float) or float, 0.0, "The chemical potential."


.. _simple_mode_kpoints:

``kpoints``
+++++++++++++
A table specifying information of wavevectors.
This table is used in ``dla_pre`` and ``dla_wvgen``.

Keys in the ``kpoints`` table are following:

.. csv-table::
  :header-rows: 1
  :widths: 1,1,1,4

  name, type, default, description
  ksteps, list(int) or int, 0, "Increments of wavenumber. If 0, half of lattice size instead of 0 is set."


.. _simple_mode_algorithm:

``algorithm``
+++++++++++++++
A table specifying algorithm for calculating scattering probability of wormheads.
This table is used in ``dla_pre``.

Keys in the ``algorithm`` table are following:

wv.. csv-table::
  :header-rows: 1
  :widths: 1,1,1,4

  name, type, default, description
  kernel, string, 'suwa todo',  "Algorithm for calculating the scattering probability of wormheads."


Available ``kernel`` s are following:

``suwa todo``
  Rejection minimized algorithm without detailed balance condition (irreversible) proposed by Suwa and Todo.
  (H. Suwa and S. Todo, PRL 105, 120603 (2010))

``reversible suwa todo``
  Rejection minimized algorithm with detailed balance condition (reversible) proposed by Suwa and Todo.
  (arXiv:1106.3562)

``heat bath``
  Heat bath method (Gibbs sampler).

``metropolice``
  Metropolice-Hasting algorithm.


.. _TOML: https://github.com/toml-lang/toml/blob/master/versions/en/toml-v0.5.0.md
