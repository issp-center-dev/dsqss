DSQSS/DLA による反強磁性ハイゼンベルグダイマーのエネルギー計算
===============================================================

このチュートリアルでは, :math:`S=1/2` 反強磁性ハイゼンベルグダイマー :math:`\mathcal{H}= J \vec{S}_1 \cdot \vec{S}_2` の基底状態エネルギー計算をすることで,
DSQSS/DLA の使い方を学びます.

DSQSS/DLA による計算は,

1. 入力ファイルの準備
2. 計算の実行
3. 計算結果の解釈

の3段階に分かれます.


入力ファイルの準備
********************

DSQSS/DLA を実行するには,

#. パラメータファイル
#. 格子定義ファイル
#. アルゴリズム定義ファイル

の3つの入力ファイルが必要です.
そのため, まずはこれらの入力ファイルを作成します.
そのためのユーティリティツールが dla_pre.py です.
これは単一の入力ファイルから, DSQSS/DLA  の入力ファイルを生成するPython スクリプトです.
まず, dla_pre.py の入力ファイルとして, 次の内容を持つテキストファイル std.toml を準備します(sample/dla/01_spindimer/std.toml).
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


このファイルを dla_pre.py に与えます.
::

  $ $DSQSS_ROOT/bin/dla_pre.py std.toml

この結果, パラメータファイル param.in, 格子定義ファイル lattice.xml, アルゴリズム定義ファイル algorithm.xml が生成されます。

計算の実行
**********

入力ファイルを作成したら, DSQSS/DLA による計算を実行します.
::

  $ $DSQSS_ROOT/bin/dla param.in


なお, 計算を実行するときにMPI を用いることで, 乱数並列計算が可能です.
::

  $ mpiexec -np 4 $DSQSS_ROOT/bin/dla param.in

並列数 (今回は4) だけ独立に計算を行い, モンテカルロサンプル数を増やすことで計算精度を向上できます. [#fn_ompi_macos]_


計算結果の解釈
****************

計算結果は出力ファイル sample.log に書き出されます.
サイトあたりのエネルギーは ``ene`` という名前で記されており, たとえば ``grep`` コマンドで
::

  $ grep ene sample.log
  R ene = -3.74380000e-01 5.19493985e-03

と取得できます. 
2つある数字はそれぞれ期待値と統計誤差です.
反強磁性ハイゼンベルグダイマーの基底状態でのサイトあたりエネルギーは :math:`-3|J|/8 = -0.375|J|` なので, 問題なく計算されていることがわかります.

.. only:: html

   .. rubric:: 脚注

.. [#fn_ompi_macos] macOS 上のOpenMPI を使う場合, プログラム終了時にエラーメッセージが出ることがあります（ ``No such file or directory (errno 2)`` ）. DSQSS/DLA の実行自体に影響はありませんが, これを抑制したい場合は,  ``--mca shmem posix`` オプションを ``mpiexec`` に付与してください.
