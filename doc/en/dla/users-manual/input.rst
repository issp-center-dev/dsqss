.. highlight:: none

.. _sec_dla_input:

Input files for DSQSS/DLA
=========================

The list of input files
************************

.. csv-table::
    :header-rows: 0
    :widths: 1,4

    qmc.inp, "Parameter list for the simulation, e.g., the number of Monte Carlo sets."
    lattice.xml, "Definition of the lattice."
    algorithm.xml, "Definition of the algorithm (e.g., scattering rate of a worm)."
    sf.xml, "Indication of wave vectors for structure factors. (optional)"
    cf.xml, "Indexing directions between all the sites. (optional)"

Parameter file ``qmc.inp``
**********************************
The parameter file is a plain-text file with the following format,

- One line stands for one parameter by the key-value style, ``<name> = <value>``.
- Cases are insensitive except for file names.
- White spaces and blank lines are ignored.
- Parts between "#" symbol and the linebreak are ignored as comments.

The list of parameters are the following,

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    name, type, default, description
    beta, double, --, "Inverse temperature. Overwrite a setting in lattice.xml."
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
    sfinpfile, string, --,  "A structure factor file. If it is an empty string, structure factors will not be calculated."
    cfinpfile, string,  --, "A real space temperature Green's function file. If it is an empty string, real space temperature Green's functions will not be calculated."
    ckinpfile, string,  --, "A momentum space temperature Green's function file. If it is an empty string, momentum space temperature Green's functions will not be calculated."
    outfile, string, sample.log, "The name of the main result file."
    sfoutfile, string, sf.dat, "The name of the structure factor result file."
    cfoutfile, string, cf.dat, "The name of the real space temperature Green's function output file."
    ckoutfile, string, ck.dat, "The name of the momentum space temperature Green's function output file."
    runtype, int, 0, "Method. This remains for backward compatibility."

- About simulationtime

  - When simulationtime > 0.0

    - DSQSS/DLA loads the checkpoint file and resumes the simulation if the checkpoint file exists.
        - If not, DSQSS/DLA starts a new simulation.
    - After the time specified by "simulationtime" (in seconds) has elapsed, DSQSS/DLA saves the state of the simulation into the checkpoint file and halts the simulation.
    - The name of the checkpoint file is that of the main result file with a suffix ".cjob".

  - When simulationtime <= 0.0

    - The checkpoint file is ignored. DSQSS/DLA never saves nor loads it.


Lattice file ``lattice.xml``
**************************************

A lattice file is a textfile written in XML format.
This defines timespace to be simulation, for example, the number of sites, the connections between sites, the inverse temperature, and so on.
This file can be very complicated, so DSQSS has a utility tool ``lattgene`` to generate lattice files describing common lattices, such as a hypercubic lattice.

The lattice file has a unique element named "Lattice". The other elements belong to "Lattice" as children.

Lattice
  The root element.

Lattice/Comment
  (Optional) Comment. DSQSS ignores this element.

Lattice/Dimension
  The dimension of the lattice.

Lattice/Beta
  The inverse temperature.
  This is overwritten by the parameter file.

Lattice/LinearSize
  The size of the lattice in units of the unitcell.
  This takes space-separated positive integers as many as specified by "Lattice/Dimension".
  ::

    <LinearSize> 3 4 </LinearSize>
    # unitcells are arranged in 3x4.

Lattice/NumberOfCells
  The number of unitcells.

Lattice/NumberOfSites
  The number of sites.

Lattice/NumberOfInteractions
  The number of the interactions.
  When all the interactions are two-body ones, this is nothing but the number of bonds.

Lattice/NumberOfSiteTypes
  The number of site types.

Lattice/NumberOfInteractionTypes
  The number of interaction types.

Lattice/BondDimension
  Parameter for the winding number.

Lattice/NumberOfEdgeInteractions
  Parameter for the Winding number.
  The number of bonds connecting sites over the lattice's boundary.

Lattice/S
  Site information.
  "Lattice" should includes this element as many as the number specified by "Lattice/NumberOfSites".
  This takes three positive integers, "index of site", "site type", and "measure type".
  The detail of site type is defined in an algorithm file.
  ::

    <S> 3 0 1 </S>
    # the site with index 3 has the site type of 0 and the measure type of 1.

Lattice/I
  Interaction information.
  "Lattice" should includes this element as many as the number specified by "Lattice/NumberOfInteractions".
  This takes space-separated integers, "index of the interaction", "interaction type", "the number of sites involved in the interaction", "indices of involved sites".
  The details of interaction type, e.g., the strength, are defined in an algorithm file.
  The order of the indices of sites should be compatible with the order of sites specified in "Algorithm/Vertex/InitialConfiguration" in the algorithm file.
  ::

    <I> 5 1 2 8 12 </I>
    # the interaction with index 5 has the interaction type of 1 and connects 2 sites, 8 and 12.


Algorithm file ``algorithm.xml``
***********************************

An algorithm file is a textfile written in XML format.
This defines the details of interactions, for example, the scattering probability of a worm head.
This file can be very complicated, so DSQSS has a utility tool ``dla_alg`` to generate algorithm files from more simple file, the Hamiltonian file introduced later.

The algorithm file has a unique element named "Algorithm". The other elements belong to "Algorithm" as children.


Algorithm
  The root element.
  This has children, "General", "Site", "Interaction", and "Vertex".

Algorithm/Comment
  (Optional) Comment. DSQSS ignores this.

Algorithm/General
  General parameters such as the number of site types.
  This has children, "NSType", "NIType", "NVType", "NXMax", and "WDiag".
  ::

    <Algorithm>
      <General>
        <NSType>  1 </NSType>
        <NIType>  1 </NIType>
        <NVType>  2 </NVType>
        <NXMax>   2 </NXMax>
        <WDiag>   0.25 </WDiag>
      </General>
      ...
    </Algorithm>

Algorithm/General/NSType
  The number of site types.

Algorithm/General/NIType
  The number of interaction types.

Algorithm/General/NVType
  The number of vertex types.

Algorithm/General/NXMax
  The maximum number of states on a site.
  For example, :math:`2S+1` for a spin system with local spin :math:`S`.

Algorithm/General/WDiag
  User can use this value for user's own purpose in "measure_specific.cc".
  In the original "measure_specific.cc" uses this value as a coefficient to measure correlation functions from the length of worms.

Algorithm/Site
  This defines a site type, for example, the weight of worm heads on a site.
  This has children "SType", "NumberOfStates", "VertexTypeOfSource", and "InitialConfiguration".

  ::

    <Algorithm>
      ...
      <Site>
        <STYPE> 0 </STYPE>
        <NumberOfStates> 2 </NumberOfStates>
        <VertexTypeOfSource> 0 </VertexTypeOfSource>
        <InitialConfiguration>
           ...
        </InitialConfiguration>
        <InitialConfiguration>
           ...
        </InitialConfiguration>
      </Site>
      ...
    </Algorithm>

Algorithm/Site/SType
  The index of site type.

Algorithm/Site/NumberOfStates
  The number of states of the site.

Algorithm/Site/VertexTypeOfSource
  The index of the vertex to be inserted here.

Algorithm/Site/InitialConfiguration
  The process of pair creation/annihilation of worm heads.
  This has children, "State", "NumberOfChannels", and "Channel"
  ::

    <Algorithm>
      ...
      <Site>
        ...
        <InitialConfiguration>
          <State> 0 </State>
          <NumberOfChannels> 2 </NumberOfChannels>
          <Channel> 0 1 0.5 </Channel>
          <Channel> 1 1 0.5 </Channel>
        </InitialConfiguration>
        ...
      </Site>
      ...
    </Algorithm>

Algorithm/Site/InitialConfiguration/State
  The state index of the site without worms (before creation or after annihilation).

Algorithm/Site/InitialConfiguration/NumberOfChannels
  The number of the channels (result of creation/annihilation).

Algorithm/Site/InitialConfiguration/Channel
  Channels.
  This takes two integers and one floating number.

  - First figure denotes the direction of the worm head ( 0 for negative and 1 for positive in the imaginary time direction).
  - Second figure denotes the state between worms.
  - Third figure denotes the probability of this channel.

  If the result has no worm heads, let both the first and the second integers be -1.

Algorithm/Interaction
  This defines an interaction.
  This has children, "IType", "VType", "NBody", "EBase", and "VertexDensity".
  ::

    <Algorithm>
      ...
      <Interaction>
        <IType> 0 </IType>
        <VType> 1 </VType>
        <NBody> 2 </NBody>
        <EBase> 0.125 </EBase>
        <VertexDensity> 0 0 0.25 </VertexDensity>
        <VertexDensity> 1 1 0.25 </VertexDensity>
      </Interaction>
      ...
    </Algorithm>

Algorithm/Interaction/IType
  The index of the interaction.

Algorithm/Interaction/VType
  The index of the vertex to be inserted.

Algorithm/Interaction/NBody
  The number of sites involved in this interaction.
  An onebody interaction such as the Zeeman term has 1 and a twobody interaction such as the exchange coupling has 2.
  Three or higher body interaction can be treated.

Algorithm/Interaction/EBase
  The offset of the local energy.
  This value does not contribute to the simulation, but to the value of energy in the final result.

Algorithm/Interaction/VertexDensity
  The density of vertex to be inserted.
  This takes integers as many as "Algorithm/Interaction/NBody" and one preceding floating number.
  The integers denote the states of sites (the order should be compatible with the order of sites in "I" of the lattice file).
  The last floating number represents the density.

Algorithm/Vertex
  This defines a vertex.
  This has children, "VType", "VCategory", "NBody", "NumberOfInitialConfigurations", and "InitialConfiguration".
  Vertices belongs to a category specified by "Algorithm/Vertex/VCategory".
  ::

    <Algorithm>
      ...
      <Vertex>
        <VTYPE> 0 </VTYPE>
        <VCATEGORY> 1 </VCATEGORY>
        <NBODY> 1 </NBODY>
        <NumberOfInitialConfigurations> 4 </NumberOfInitialConfigurations>
        <InitialConfiguration>
          ...
        </InitialConfiguration>
        ...
        <InitialConfiguration>
          ...
        </InitialConfiguration>
      </Vertex>
      ...
    </Algorithm>

Algorithm/Vertex/VType
  The index of the vertex.

Algorithm/Vertex/VCategory
  0. Boundary of imaginary time. Users need not define this.
  1. Worm tail.
  2. Interaction.

Algorithm/Vertex/NBody
  The number of sites involved.

Algorithm/Vertex/NumberOfInitialConfigurations
  The number of initial states.

Algorithm/Vertex/InitialConfiguration
  This defines scattering results of a worm head for each initial states.
  "Algorithm/Vertex" should has this elements as many as the number specified by "Algorithm/Vertex/NumberOfInitialConfigurations".
  This has children, "State", "IncomingDirection", "NewState", "NumberOfChannels", "Channel".
  ::

    <Algorithm>
      ...
      <Vertex>
        ...
        <InitialConfiguration>
          <State>  1 0 0 1 </State>
          <IncomingDirection> 0 </IncomingDirection>
          <NewState> 0 </NewState>
          <NumberOfChannels> 1 </NumberOfChannels>
          <Channel>    3    0       1.0000000000000000 </Channel>
        </InitialConfiguration>
        ...
      </Vertex>
      ...
    </Algorithm>

 This example represents the following scenario;

  - Initial states of bottom-left(0), top-left(0), bottom-right(2), and top-right(3) are 1, 0, 0, and 1, respectively.
  - A worm head comes from bottom-left(0) and changes the state of this leg to 0.
  - The worm head will be scattered to leg(3) and the state of outgoing leg will be changed to 0 with the probability 1.

Algorithm/Vertex/InitialConfiguration/State
  The initial states of the legs of the vertex.
  Since the number of the legs is as twice as the number specified by "Algorithm/Vertex/NBody", say :math:`m`,
  this takes :math:`2m` integers.
  Legs are in the same order as the corresponding sites.
  For two legs on the same site, the leg with the smaller imaginary time comes first.

Algorithm/Vertex/InitialConfiguration/IncomingDirection
  The index of the leg from which a worm head comes.

Algorithm/Vertex/InitialConfiguration/NewState
  The state of the "Algorithm/Vertex/InitialConfiguration/IncomingDirection" leg after a worm head comes.

Algorithm/Vertex/InitialConfiguration/NumberOfChannels
  The number of scattering channels (final results).

Algorithm/Vertex/InitialConfiguration/Channel
  A scattering channel.
  This takes two integers and one floating number.

  - First figure denotes the **index** of the leg where the scattered worm head goes out.
  - Second figure denotes the **state** of the leg where the scattered worm head goes out after the scattering.
  - Last figure denotes the probability of this channel.

  For the special case, the pair-annihilation of worm heads, let both the first and the second integer be -1.

Hamiltonian file ``hamiltonian.xml``
************************************************

A Hamiltonian file is a textfile written in XML format.
This defines the local Hamiltonians, e.g., a bond Hamiltonian.
This file is used as an input of ``dla_alg`` in order to generate ``algorithm.xml`` .
DSQSS has utility tools ``hamgen_H`` and ``hamgen_B`` to generate hamiltonian files describing the Heisenberg spin model and the Bose-Hubbard model.

The Hamiltonian file has a unique element named "Hamiltonian". The other elements belong to "Hamiltonian" as children.

Hamiltonian
  The root element.
  This has children, "General", "Site", "Source", and "Interaction".

Hamiltonian/General
  GEneral parameters such as the number of site types.
  This has children, "NSTYPE", "NITYPE", "NXMAX", and "Comment".
  ::

     <Hamiltonian>
        <General>
          <Comment> SU(2) Heisenberg model with S=1/2 </Comment>
          <NSTYPE> 1 </NSTYPE>
          <NITYPE> 1 </NITYPE>
          <NXMAX>  2 </NXMAX>
        </General>
       ...
     </Hamiltonian>

Hamiltonian/General/Comment
  (Optional) Comment. DSQSS ignores this.

Hamiltonian/General/NSTYPE
  The number of site types.

Hamiltonian/General/NITYPE
  The number of interaction types.

Hamiltonian/General/NXMAX
  The maximum number of states on a site.
  For example, :math:`2S+1` for a spin system with local spin :math:`S`.

Hamiltonian/Site
  This defines a site type, for example, the number of states.
  This has children "STYPE", "TTYPE", and "NX".
  ::

    <Hamiltonian>
      ...
      <Site>
        <STYPE> 0 </STYPE>
        <TTYPE> 0 </TTYPE>
        <NX>   2 </NX>
      </Site>
      ...
    </Hamiltonian>

Hamiltonian/Site/STYPE
  The index of site type.

Hamiltonian/Site/TTYPE
  The index of the source type (type of pair creation/annihilation of wormheads.)

Hamiltonian/Site/NX
  The number of states of the site.


Hamiltonian/Source
  This defines a source type, that is, the pair-creation/annihilation of wormheads.
  This has children "TTYPE", "STYPE", and "Weight".
  ::

      <Source>
        <TTYPE> 0 </TTYPE>
        <STYPE> 0 </STYPE>
        <Weight> 0 1       0.5000000000000000 </Weight>
        <Weight> 1 0       0.5000000000000000 </Weight>
      </Source>
   
Hamiltonian/Source/TTYPE
   The index of the source type.

Hamiltonian/Source/STYPE
   The index of the site type.

Hamiltonian/Source/Weight
  The weight of the creation/annihilation operator.
  This takes two integers and one floating number.
  The integers denote the states of the site before and after applying the operator, respectively.
  The floating number denotes the matrix element.

  For example, ``0 1 0.5`` means :math:`\langle 1 | \mathcal{H} | 0 \rangle = 0.5`.

Hamiltonian/Interaction
  This defines an interaction type.
  This has children "ITYPE", "STYPE", "NBODY", and "Weight".
  ::

    <Hamiltonian>
      ...
      <Interaction>
        <ITYPE> 0 </ITYPE>
        <NBODY> 2 </NBODY>
        <STYPE> 0 0 </STYPE>
        <Weight> 0 0 0 0      -0.2500000000000000 </Weight>
        <Weight> 1 1 0 0       0.2500000000000000 </Weight>
        <Weight> 1 0 0 1       0.5000000000000000 </Weight>
        <Weight> 0 1 1 0       0.5000000000000000 </Weight>
        <Weight> 0 0 1 1       0.2500000000000000 </Weight>
        <Weight> 1 1 1 1      -0.2500000000000000 </Weight>
      </Interaction>
      ...
    </Hamiltonian>

Hamiltonian/Interaction/ITYPE
  The index of the interaction type.

Algorihtn/Interaction/NBODY
  The number of sites involved in this interaction.
  An onebody interaction such as the Zeeman term has 1 and a twobody interaction such as the exchange coupling has 2.
  Three or higher body interaction can be treated.

Hamiltonian/Interaction/ITYPE
  The indices of sites involved in this interaction.
  This takes NBODY integers.

Hamiltonian/Interaction/Weight
  The matrix elements of the local Hamiltonian.

  This takes integers as many as :math:`2\times` NBODY and one preceding floating number.
  The integers denote the states of sites before and after applying the local Hamiltonian.
  The last floating number denotes the matrix element producted by :math:`-1`.
  For offdiagonal elements, this value should be positive [#fn_reweighting]_.

  For example, ``0 0 1 1 0.25`` means :math:`\langle 0 1 | \mathcal{H} | 0 1 \rangle = -0.25`
  and ``0 1 1 0 0.5`` means :math:`\left| \langle 1 0 | \mathcal{H} | 0 1 \rangle \right| = 0.5`.

Structure factor file ``sf.xml``
*********************************

A structure factor file is a textfile written in a XML-like format.
This defines wave vectors and the discretization of imaginary time to calculate the dynamical structure factor

.. math::
    S^{zz}(\vec{k},\tau) \equiv
      \left\langle M^z(\vec{k},\tau)M^z(-\vec{k},0) \right\rangle - \left\langle M^z(\vec{k},\tau)\right\rangle \left\langle M^z(-\vec{k},0)\right\rangle .

DSQSS has a utility tool to generate a structure factor file, ``sfgene``.

A structure factor file has only one element, "StructureFactor", and the other elements are children of this.

StructureFactor
  The root element.
  This has children, "Ntau", "NumberOfElements", "CutoffOfNtau", "NumberOfInverseLattice", and "SF".

StructureFactor/Comment
  (Optional) Comment. DSQSS ignores this.

StructureFactor/Ntau
  The number of discretization of the imaginary time axis.

StructureFactor/CutoffOfNtau
  The maximum of the imaginary time distance of the dynamical structure factor, :math:`\tau`.
  This takes an integer in :math:`0, 1, \dots, \mathrm{Ntau}`.

StructureFactor/NumberOfInverseLattice
  The number of wave vectors, :math:`\vec{k}`

StructureFactor/NumberOfElements
  The number of the combination of wave vectors and sites.

StructureFactor/SF
  The phase factor :math:`z = \exp{\vec{r}\cdot\vec{k}}` for a pair of a wave vector and a site.
  This takes four figures, ":math:`\mathrm{Re}z`", ":math:`\mathrm{Im}z`", "the index of the site", "the index of the wave vector".
  "StructureFactor" should has this elements as many as the number specified by "StructureFactor/NumberOfElements".

Real space temperature Green's function file ``cf.xml``
********************************************************

Real space temperature Green's function file is a textfile written in a XML-like format.
This defines relative coordinate between two sites, :math:`\vec{r}_{ij}`, to calculate real space temperature Green's function,

.. math::
  G(\vec{r},\tau) \equiv \frac{1}{N^2}\sum_{i,j}\left\langle M_i^+(\tau) M_j^- \right\rangle \delta(\vec{r}-\vec{r}_{ij}) .

More precisely, this groups all the pair of sites by the relative coordinates.

DSQSS has a utility tool to generate a real space temperature Green's function file, ``cfgene``.

A real space temperature Green's function file has only one element, "CorrelationFunction", and the other elements belong to this as children.

CorrelationFunction
  The root element.
  This has children, "Ntau", "NumberOfKinds", and "CF".

CorrelationFunction/Comment
  (Optional) Comment. DSQSS ignores this.

CorrelationFunction/Ntau
  The number of discretization of the imaginary time axis.

CorrelationFunction/NumberOfKinds
  The number of relative coordinates.

CorrelationFunction/CF
  This takes three integers, "the index of the relative coordinate", "the index of the site :math:`i`", and "the index of the site :math:`j`".
  "CorrelationFunction" should has this elements as many as the number specified by "CorrelationFunction/NumberOfKinds".


Momentum space temperature Green's function file ``ck.xml``
************************************************************

A momentum space temperature Green's function file is a textfile written in a XML-like format.
This defines wave vectors and the discretization of imaginary time to calculate the momentum space temperature Green's function,

.. math::
  G(\vec{k},\tau) \equiv \left\langle M^+(\vec{k}, \tau) M^-(-\vec{k},0) \right\rangle .

Since this file has the format as same as that of the structure factor file including the names of elements,
users can use the same file.



.. only:: html

   .. rubric:: Footnote

.. [#fn_reweighting]
  In other words, we always perform a simulation of the "absolute" system.
  We plans to implement the negative-sign reweighting and remove this limitation in DSQSS v2.

