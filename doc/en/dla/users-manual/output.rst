.. -*- coding: utf-8 -*-
.. highlight:: none

Output of DSQSS/DLA
====================

Format
*******
DSQSS/DLA generates the result of a simulation as a plain-text file.
The first character stands for the meaning of the line.

``P <name> = <value>``
  Parameters read from the input files.
``R <name> = <mean> <error>``
  Results of observables. ``<mean>`` denotes the expected value and ``<error>`` denotes the statistical error of ``<mean>``.
``I <text> = <value>``
  Other information.
``C <text>``
  Comments.

The following one is a result of an antiferromagnetic Heisenberg chain.
::

  C This is DSQSS ver.v2.0.0

  I N_PROC = 1
  P D       =            1
  P L       =            8
  P BETA    =      10.0000000000000000
  P NSET    =           10
  P NPRE    =         1000
  P NTHERM  =         1000
  P NDECOR  =         1000
  P NMCS    =         1000
  P SEED    =    198212240
  P NSEGMAX =        10000
  P NVERMAX =        10000
  P BETA    = 10.000000000000
  P NTAU    =           10
  P NCYC    =            7
  P ALGFILE = algorithm.xml
  P LATFILE = lattice.xml
  P WVFILE = wv.xml
  P DISPFILE  = disp.xml
  P OUTFILE    = sample.log
  P CFOUTFILE  = cf.dat
  P SFOUTFILE  = sf.dat
  P CKOUTFILE  = ck.dat
  P SIMULATIONTIME   =     0.000000
  R sign = 1.00000000e+00 0.00000000e+00
  R anv = 3.03805000e+00 1.25395375e-02
  R ene = -4.55991910e-01 1.20267537e-03
  R spe = -1.76672204e-02 4.09064489e-02
  R som = -1.76672204e-01 4.09064489e-01
  R len = 1.20014021e+01 4.78403202e-02
  R xmx = 3.00035053e-01 1.19600800e-03
  R amzu = -2.00000000e-04 1.08972474e-04
  R bmzu = -2.00000000e-04 1.08972474e-04
  R smzu = 1.32382500e-03 1.40792745e-04
  R xmzu = 1.32382500e-02 1.40792745e-03
  R ds1 = -1.32954309e-03 7.87178338e-04
  R wi2 = 2.31040000e+01 3.83762890e-01
  R rhos = 1.44400000e-01 2.39851806e-03
  R rhof = inf nan
  R comp = 2.43165481e+35 1.71412709e+35
  R amzs0 = -2.00000000e-04 1.08972474e-04
  R bmzs0 = 1.65625000e-04 1.76161818e-05
  R smzs0 = 1.32382500e-03 1.40792745e-04
  R xmzs0 = 1.32382500e-02 1.40792745e-03
  R amzs1 = -9.25000000e-04 4.02247160e-03
  R bmzs1 = 1.09209375e-01 1.12051866e-03
  R smzs1 = 8.72503175e-01 8.93939492e-03
  R xmzs1 = 3.00500011e+00 2.99056535e-02
  R time = 1.03679300e-07 1.22794234e-09
  I [the maximum number of segments]          = 123
  I [the maximum number of vertices]          = 66
  I [the maximum number of reg. vertex info.] = 3


Notations
-----------

:math:`N_s`
  The number of sites.

:math:`Q(\vec{k})`
  The Fourier transformation of an arbitrary operator on a site :math:`i`, :math:`Q_i`.

  :math:`\displaystyle Q(\vec{k}) \equiv \frac{1}{\sqrt{N_s}} \sum_i^{N_s} Q_i e^{-i\vec{r}_i\cdot\vec{k}}`

:math:`Q(\tau)`
  An arbitrary operator at imaginary time :math:`\tau`.

  :math:`\displaystyle Q(\tau) \equiv \exp\left[\tau \mathcal{H}\right] Q(\tau=0) \exp\left[-\tau \mathcal{H}\right]`

:math:`\tilde{Q}`
  The average of an arbitrary operator :math:`Q` over the imaginary time,
  :math:`\displaystyle \frac{1}{\beta}\int_0^\beta \! \mathrm{d} \tau Q(\tau)`

:math:`M^z`
  The component of a local degree of freedom along with the quantized axis.
  For example, :math:`z` component of the local spin operator :math:`S^z` for spin systems
  and the number operator :math:`n` for the Bose-Hubbard models.

:math:`M^\pm`
  The ladder operator.
  :math:`M^{\pm} \equiv S^\pm` for spin systems, and
  the creation/annihilation operators :math:`M^+ \equiv b^\dagger` , :math:`M^- \equiv b` for the Bose-Hubbard models.

:math:`M^x`
  The off-diagonal order parameter.
  :math:`M^x \equiv (S^+ + S^-)/2` for spin systems and
  :math:`M^x \equiv (b + b^\dagger)` for the Bose-Hubbard models.

:math:`T`
  The temperature.

:math:`\beta`
  The inverse temperature.

:math:`E_0`
  The imaginary time average of the expectation value of the unperturbed Hamiltonian
  :math:`\displaystyle \frac{1}{\beta}\int d\tau \langle \phi(\tau)|\mathcal{H}_0|\phi(\tau)\rangle`.

:math:`N_v`
  The number of vertices, i.e., the order of the perturbation.

:math:`h`
  The conjugate field to the operator :math:`M^z` .
  The longitudinal magnetic field for spin systems and the chemical potential for the Bose-Hubbard models.

:math:`\left\langle Q \right\rangle`
  The expectation value of an arbitrary operator :math:`Q` over the grand canonical ensemble.

Main results
*****************
Main results are written in a file with the name specified by ``outfile`` keyword in the input parameter file.

NOTICE: In general, Monte Carlo simulations have systematic errors of :math:`O(1/N)` with respect to the number of samples :math:`N` (``nmcs``) for the expectation values including nonlinear functions of the sample average like the specific heat and the susceptibility.
For example, in the region where the specific heat becomes very small, e.g., in the low-temperature region below the energy gap, the calculated value may be negative.
For precise analysis, we need to take into account not only the statistical errors but also the systematic errors.

``sign``
  The sign of the weights.

  :math:`\sum_i W_i / \sum_i |W_i`

``anv``
  The number of the vertices per site.

  :math:`\displaystyle \frac{\langle N_v \rangle}{N_s}`
``ene``
  The energy density (energy per site)

  :math:`\displaystyle \epsilon \equiv \frac{1}{N_s}\left(E_0 - T\langle N_v\rangle\right)`
``spe``
  The specific heat

  :math:`\displaystyle C_V \equiv \frac{\partial \epsilon}{\partial T} = \frac{1}{N_s T^2} \left[\left\langle\left(E_0 - TN_v\right)^2\right\rangle - \left\langle\left(E_0 - TN_v\right)\right\rangle^2 - T^2\left\langle N_v \right\rangle\right]`

``som``
  The ratio of the specific heat and the temperature.

  :math:`\displaystyle \gamma \equiv \frac{C_V}{T} = \beta C_V`

``len``
  The mean length of worm

``xmx``
  The transverse susceptibility

``amzu``
  The "magnetization" (uniform, :math:`\tau=0`).

  :math:`\left\langle m^z \right\rangle` , where
  :math:`\displaystyle m^z \equiv \frac{1}{N_s} \sum_i^{N_s} M^z_i`

``bmzu``
  The "magnetization" (uniform, average over :math:`\tau`). :math:`\left\langle \tilde{m}^z \right\rangle` .

``smzu``
  The structure factor (uniform).

  :math:`\displaystyle S^{zz}(\vec{k}=0) \equiv \frac{1}{N_s}
  \sum_{i, j} e^{i \vec{k}\cdot(\vec{r}_i-\vec{r}_j)} \left[
  \left\langle M^z_i M^z_j\right\rangle
  - \left\langle M_i^z \right\rangle \left\langle M_j^z \right\rangle 
  \right] \Bigg|_{\vec{k}=0}
  =
  N_s \left[ \left\langle (m^z)^2\right\rangle
  - \left\langle m^z\right\rangle^2 \right]`

``xmzu``
  The longitudinal susceptibility (uniform).

  :math:`\displaystyle \chi^{zz}(\vec{k}=0,  \omega=0) \equiv
  \frac{\partial \left\langle \tilde{m}^z \right\rangle}{\partial h} =
  \beta N_s\left[ \left\langle \left(\tilde{m}^z\right)^2\right\rangle
  - \left\langle \tilde{m}^z\right\rangle^2 \right]`

``amzsK``
  The "magnetization" ("staggered", :math:`\tau=0`)

  :math:`\left\langle m_s^z \right\rangle` where
  :math:`\displaystyle m_K^z \equiv \frac{1}{N_s} \sum_i^{N_s} M_i^z \cos\left( \vec{k}\cdot\vec{r_i} \right)` .

  :math:`K` is an index of wavevector :math:`k` specified in the wavevector XML file.

``bmzu``
  The "magnetization" ("staggered", average over :math:`\tau`).
  :math:`\left\langle \tilde{m}_K^z \right\rangle` .

``smzs``
  The structure factor ("staggered").

  :math:`\displaystyle S^{zz}(\vec{k}) = N_s \left[ \left\langle (m_K^z)^2 \right\rangle - \left\langle m_K^z \right\rangle^2 \right]`

``xmzs``
  The longitudinal susceptibility ("staggered").

  :math:`\displaystyle \chi^{zz}(\vec{k},  \omega=0) 
  = \beta N_s \left[\left\langle (\tilde{m}_K^z)^2 \right\rangle - \left\langle \tilde{m}_K^z \right\rangle^2 \right]`

``ds1``
  The derivative of the "magnetization" (``amzu``) with respect to the temperature.

  :math:`\displaystyle T\frac{\partial \left\langle \tilde{m}^z \right\rangle}{\partial T} = -\beta\frac{\partial \left\langle \tilde{m}^z \right\rangle}{\partial\beta}`

``wi2``
  The winding number.

  :math:`\displaystyle W^2 = \sum_{d=1}^D L_d^2 \left\langle W_d^2 \right\rangle`

``rhos``
  The superfluid density.

  :math:`\displaystyle \rho_s = \frac{W^2}{2DV\beta}`


``rhof``
  The superfluid fraction.

  :math:`\displaystyle \frac{\rho_s}{\left\langle m^z \right\rangle}`


``comp``
  The compressibility.

  :math:`\displaystyle \frac{\chi^{zz}(\vec{k}=0,\omega=0)} {\left\langle \tilde{m}^z \right\rangle^2}`


``time``
  The time in a Monte Carlo sweep (in seconds.)


Structure factor output
****************************************
The structure factor is written into a file with the name specified by ``sfoutfile`` keyword in the input file.
The structure factor is defined as the following:

.. math::
  S^{zz}(\vec{k}, \tau) \equiv
  \left\langle M^z(\vec{k},  \tau) M^z(-\vec{k},  0) \right\rangle
  - \left\langle M^z(\vec{k},  \tau)\right\rangle \left\langle M^z(-\vec{k},  0)\right\rangle

Wave vector :math:`\vec{k}` and imaginary time :math:`\tau` are specified by the name ``C<k>t<t>`` as the following:
::

  R C0t0 = 1.32500000e-03 1.40929454e-04
  R C0t1 = 1.32500000e-03 1.40929454e-04
  R C1t0 = 7.35281032e-02 3.18028565e-04

where ``<k>`` is an index of the wave vector specified by ``kindex`` (the last element of each ``RK`` tag) in the wavevector XML file
and ``<t>`` is an index of the discretized imaginary time.

Real space temperature Green's function output
**********************************************
The real space temperature Green's function is written into a file with the name specified by ``cfoutfile`` keyword in the input file.
The real space temperature Green's function is defined as the following:

.. math::
  G(\vec{r}_{ij}, \tau) \equiv \left\langle M_i^+(\tau) M_j^- \right\rangle

Displacement :math:`\vec{r}_{ij}` and imaginary time :math:`\tau` are specified by the name ``C<k>t<t>`` as the same way of structure factor,
where ``<k>`` is an index of the displacement specified by ``kind`` (the first element of each ``R`` tag) in the relative coordinate XML file,
and ``<t>`` is an index of the discretized imaginary time.

NOTE: The current version, this works only for :math:`S=1/2` model.

Momentum space temperature Green's function output
**************************************************
The momentum space temperature Green's function is written into a file with the name specified by ``ckoutfile`` keyword in the input file.
The momentum space temperature Green's function is defined as the following:

.. math::
  G(\vec{k}, \tau) \equiv \left\langle M^+(\vec{k},  \tau) M^-(-\vec{k}, 0) \right\rangle

Wave vector :math:`\vec{r}_{ij}` and imaginary time :math:`\tau` are specified by the name ``C<k>t<t>`` as the same way of structure factor,
where ``<k>`` is an index of the displacement specified by ``kind`` (the last element of each ``RK`` tag) in the wavevector XML file,
and ``<t>`` is an index of the discretized imaginary time.

NOTE: The current version, this works only for :math:`S=1/2` model.
