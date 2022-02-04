DSQSS/DLA のスタンダードモードによる反強磁性ハイゼンベルグダイマーの模型や格子の定義
===============================================================

このチュートリアルでは, :math:`S=1/2` 反強磁性ハイゼンベルグダイマーに外部磁場が印加されている系 :math:`\mathcal{H}= \sum_{\langle i,j \rangle}-J_z S^z_i S^z_j -\frac{J_{xy}}{2}(S^+_i S^-_j +S^-_i S^+_j)-h\sum_i S^z_i ,~J_z<0, J_xy<0, h>0` の格子の概形を計算をすることで,
DSQSS/DLA のスタンダードモード使い方を学びます.

DSQSS/DLA のスタンダードモードによる計算は,

1. シンプルモードの入力ファイルの準備
2. スタンダードモードの入力ファイルの作成
3. 計算の実行

の3段階に分かれます.


シンプルモードの入力ファイルの準備
********************

DSQSS/DLA を実行するには,

#. ハミルトニアンの情報
#. 格子の情報
#. 逆温度やモンテカルロステップ数などのパラメータの情報

を指定したTOML形式のシンプルモードファイル std.tomlが必要です.
そのため, まずはこの入力ファイルを準備します.
反強磁性ハイゼンベルグダイマーのファイルは以下の場所に用意されています
(sample/dla/01_spindimer/std.toml)
::

   [hamiltonian]
   model = "spin"
   M =  1                 # S=1/2
   Jz = -1.0              # coupling constant, negative for AF
   Jxy = -1.0             # coupling constant, negative for AF
   h = 0.0                # magnetic field
   [lattice]
   lattice = "hypercubic" # hypercubic, periodic
   dim = 1                # dimension
   L = 2                  # number of sites along each direction
   bc = false             # open boundary
   [parameter]
   beta = 100             # inverse temperature
   nset = 5               # set of Monte Carlo sweeps
   npre = 10              # MCSteps to estimate hyperparameter
   ntherm = 10            # MCSweeps for thermalization
   nmcs = 100             # MCSweeps for measurement
   seed = 31415           # seed of RNG

このファイルにz方向の外部磁場の印加の効果を付け加え, 格子の次元とサイトの数をそれぞれ2と4にします
::

   [hamiltonian]
   model = "spin"
   M =  1                 # S=1/2
   Jz = -1.0              # coupling constant, negative for AF
   Jxy = -1.0             # coupling constant, negative for AF
   h = 0.5                # magnetic field
   [lattice]
   lattice = "hypercubic" # hypercubic, periodic
   dim = 2                # dimension
   L = 4                  # number of sites along each direction
   bc = false             # open boundary
   [parameter]
   beta = 100             # inverse temperature
   nset = 5               # set of Monte Carlo sweeps
   npre = 10              # MCSteps to estimate hyperparameter
   ntherm = 10            # MCSweeps for thermalization
   nmcs = 100             # MCSweeps for measurement
   seed = 31415           # seed of RNG
   

スタンダードモードの入力ファイルの作成
**********

このファイルを dla_hamgen に与えます.
::

  $ dla_hamgen std.toml

この結果, サイトハミルトニアンや多体相互作用の情報を記述するTOML形式のテキストファイル hamiltonian.toml が生成されます.


次に, std.toml を dla_latgen に与えます.
::

  $ dla_latgen -t lattice.toml std.toml


この結果, ユニットセルと基本並進ベクトルを用いて空間の情報を記述するTOML形式のテキストファイルlattice.toml が生成されます.


計算の実行
****************

lattice.toml を入力ファイルとして, gnuplotファイル lattice.pltを作成できます::
::

  $ dla_latgen -g lattice.plt lattice.toml

lattce.plt を gnuplot に与えると2次元の格子の概形が得られます::


.. figure:: ../../../image/dla/tutorial/spinchain.*
  :name: fig_spinchain
  :alt: スピン鎖の帯磁率

  反強磁性ハイゼンベルグスピン鎖の帯磁率の温度依存性. 青が :math:`S=1/2` で 赤が :math:`S=1` の結果.







