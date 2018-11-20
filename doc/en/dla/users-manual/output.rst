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
  Result observables. ``<mean>`` denotes the expected value and ``<error>`` denotes the statistical error of ``<mean>``.
``I <text> = <value>``
  Other information.
``C <text>``
  Comments.

The following one is a result of an antiferromagnetic Heisenberg chain.
::

  C This is DSQSS ver.1.2.0

  P D       =            1
  P L       =            8
  P BETA    =      10.0000000000000000
  P NSET    =           10
  P NMCSE   =         1000
  P NMCSD   =         1000
  P NMCS    =         1000
  P SEED    =    198212240
  P NSEGMAX =        10000
  P NVERMAX =        10000
  P NCYC    =            7
  P ALGFILE = algorithm.xml
  P LATFILE = lattice.xml
  P CFINPFILE  = cf.xml
  P SFINPFILE  = sf.xml
  P CKINPFILE  = sf.xml
  P OUTFILE    = res.dat.000
  P CFOUTFILE  = cfout.dat.000
  P SFOUTFILE  = sfout.dat.000
  P CKOUTFILE  = ckout.dat.000
  P SIMULATIONTIME   =     0.000000
  R anv = 3.03805000e+00 1.25395375e-02
  R ene = -4.55991910e-01 1.20267537e-03
  R spe = -1.76672204e-02 4.09064489e-02
  R len = 1.20014021e+01 4.78403202e-02
  R xmx = 3.00035053e-01 1.19600800e-03
  R amzu = -2.00000000e-04 1.08972474e-04
  R bmzu = -2.00000000e-04 1.08972474e-04
  R smzu = 1.32382500e-03 1.40792745e-04
  R xmzu = 1.32382500e-02 1.40792745e-03
  R amzs = -9.25000000e-04 4.02247160e-03
  R bmzs = -2.03918502e-04 2.22828174e-03
  R smzs = 8.72503175e-01 8.93939492e-03
  R xmzs = 3.00500011e+00 2.99056535e-02
  R time = 9.01378000e-08 1.61529255e-09
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
  任意の演算子 :math:`Q` について, 虚時間方向の平均
  The average of an arbitrary operator :math:`Q` over the imaginary time,
  :math:`\displaystyle \frac{1}{\beta}\int_0^\beta \! \mathrm{d} \tau Q(\tau)`

:math:`M^z`
  The component of a local degree of freedom along with the quantized axis.
  For example, :math:`z` component of the local spin operator :math:`S^z` for spin systems
  and the number operator :math:`n` for the Bose-Hubbard models.

:math:`M^\pm`
  The ladder operator.
  :math:`M^{\pm} \equiv S^\pm` for spin systems, and
  the creation/annihilation operators :math:`M^+ \equiv b^\dagger` / :math:`M^- \equiv b` for the Bose-Hubbard models.

:math:`M^x`
  The off-diagonal order parameter.
  :math:`M^x \equiv (S^+ + S^-)/2` for spin systems and
  :math:`M^x \equiv (b + b^\dagger)` for the Bose-Hubbard models.

:math:`T`
  Temperature.

:math:`\beta`
  Inverse temperature.

:math:`h`
  Conjugate field to the operator :math:`M^z` .
  The longitudinal magnetic field for spin systems and the chemical potential for the Bose-Hubbard models.

:math:`\left\langle Q \right\rangle`
  The expectation value of an arbitrary operator :math:`Q` over the grand canonical ensemble.

メイン出力
*****************
メイン出力ファイルは, 入力パラメータファイルの ``outfile`` キーワードで指定した名前で出力されます.

``anv``
  平均バーテックス数.

  :math:`\displaystyle \frac{\langle N_v \rangle}{N_s}`
``ene``
  エネルギー密度.

  :math:`\displaystyle \epsilon \equiv \frac{1}{N_s}\left(E_0 - T\langle N_v\rangle\right)`
``spe``
  比熱.

  :math:`\displaystyle C_V \equiv \frac{\partial \epsilon}{\partial T}`
``len``
  平均ワーム長さ.
``xmx``
  横感受率.
``amzu``
  「磁化」(uniform, :math:`\tau=0`).

  :math:`\displaystyle m^z \equiv \frac{1}{N_s} \sum_i^{N_s} M^z_i` としたときの
  :math:`\left\langle m^z \right\rangle` .

``bmzu``
  「磁化」(uniform, :math:`\tau` 平均). :math:`\left\langle \tilde{m}^z \right\rangle` .

``smzu``
  構造因子(uniform).

  :math:`\displaystyle S^{zz}(\vec{k}=0) \equiv \frac{1}{N_s}
  \sum_{i, j} e^{i \vec{k}\cdot(\vec{r}_i-\vec{r}_j)} \left[
  \left\langle M^z_i M^z_j\right\rangle
  - \left\langle M_i^z \right\rangle \left\langle M_j^z \right\rangle 
  \right] \Bigg|_{\vec{k}=0}
  =
  N_s \left[ \left\langle (m^z)^2\right\rangle
  - \left\langle m^z\right\rangle^2 \right]`

``xmzu``
  縦感受率(uniform).

  :math:`\displaystyle \chi^{zz}(\vec{k}=0,  \omega=0) \equiv
  \frac{\partial \left\langle \tilde{m}^z \right\rangle}{\partial h} =
  \beta N_s\left[ \left\langle \left(\tilde{m}^z\right)^2\right\rangle
  - \left\langle \tilde{m}^z\right\rangle^2 \right]`

``amzs``
  「磁化」("staggered", :math:`\tau=0`)

  :math:`\displaystyle m_s^z \equiv \frac{1}{N_s} \sum_i^{N_s} M_i^z 
  \cos\left( 2\pi \frac{\text{mtype}(i)}{N_\text{mtype}} \right)` 
  としたときの :math:`\left\langle m_s^z \right\rangle` .
  ここで :math:`\text{mtype}(i)` は :math:`i` サイトの測定種類（格子ファイル参照）, 
  :math:`N_\text{mtype}` は測定種類の総数.

``bmzu``
  「磁化」("staggered", :math:`\tau` 平均). :math:`\left\langle \tilde{m}_s^z \right\rangle` .

``smzs``
  構造因子 ("staggered").

  :math:`\displaystyle S^{zz}(\vec{k}_s) = N_s \left[ \left\langle (m_s^z)^2 \right\rangle - \left\langle m_s^z \right\rangle^2 \right]`

``xmzs``
  縦感受率 ("staggered").

  :math:`\displaystyle \chi^{zz}(\vec{k}_s,  \omega=0) 
  = \beta N_s \left[\left\langle (\tilde{m}_s^z)^2 \right\rangle - \left\langle \tilde{m}_s^z \right\rangle^2 \right]`

構造因子出力ファイル
****************************************
構造因子出力ファイルは, 入力パラメータファイルの ``sfoutfile`` キーワードで指定した名前で出力されます.
このファイルには虚時間構造因子

.. math::
  S^{zz}(\vec{k}, \tau) \equiv
  \left\langle M^z(\vec{k},  \tau) M^z(-\vec{k},  0) \right\rangle
  - \left\langle M^z(\vec{k},  \tau)\right\rangle \left\langle M^z(-\vec{k},  0)\right\rangle

が出力されます. 
波数 :math:`\vec{k}` や虚時間 :math:`\tau` の値は, 物理量名を用いて
::

  R C0t0 = 1.32500000e-03 1.40929454e-04
  R C0t1 = 1.32500000e-03 1.40929454e-04
  R C1t0 = 7.35281032e-02 3.18028565e-04

のように ``C<k>t<t>`` という形で区別されます.
ここで ``<k>`` は構造因子入力ファイルの ``kindex`` (``SF`` タグの最終要素) で指定される波数のインデックスで, 
``<t>`` は離散化した虚時間のインデックス.

実空間表示温度グリーン関数出力ファイル
****************************************
実空間表示温度グリーン関数出力ファイルは, 入力パラメータファイルの ``cfoutfile`` キーワードで指定した名前で出力されます.
このファイルには温度グリーン関数

.. math::
  G(\vec{r}_{ij}, \tau) \equiv \left\langle M_i^+(\tau) M_j^- \right\rangle

が出力されます. 変位 :math:`\vec{r}_{ij}` や虚時間 :math:`\tau` の値は構造因子と同様に, 
``C<k>t<t>`` という形で物理量名によって区別されます.
ここで ``<k>`` は実空間温度グリーン関数入力ファイルの ``kind`` (``CF`` タグの第一要素) で指定される変位のインデックスで, 
``<t>`` は離散化した虚時間のインデックス.

波数表示温度グリーン関数出力ファイル
****************************************
波数表示温度グリーン関数出力ファイルは, 入力パラメータファイルの ``ckoutfile`` キーワードで指定した名前で出力されます.
このファイルには温度グリーン関数

.. math::
  G(\vec{k}, \tau) \equiv \left\langle M^+(\vec{k},  \tau) M^-(-\vec{k}, 0) \right\rangle

が出力されます.
波数 :math:`\vec{k}` や虚時間 :math:`\tau` の値は構造因子と同様に, 
``C<k>t<t>`` という形で物理量名によって区別されます.
ここで ``<k>`` は波数表示温度グリーン関数入力ファイルの ``kindex`` (``SF`` タグの最終要素) で指定される変位のインデックスで, 
``<t>`` は離散化した虚時間のインデックス.
