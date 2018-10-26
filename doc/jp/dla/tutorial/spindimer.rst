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
そのためのユーティリティツールが dsqss_pre.py です.
これは単一の入力ファイルから, DSQSS/DLA および DSQSS/PMWA の入力ファイルを生成するPython スクリプトです.
まず, dsqss_pre.py の入力ファイルとして, 次の内容を持つテキストファイル std.in を準備します(sample/dla/01_spindimer/std.in).
::

  [System]
  solver = DLA
  [Hamiltonian]
  model_type = spin
  M =  1                 # S=1/2
  J = -0.5               # coupling constant, negative for AF, not 1 but 1/2 due to PBC
  F = 0.0                # magnetic field
  [Lattice]
  lattice_type = square  # hypercubic, periodic
  D = 1                  # dimension
  L = 2                  # number of sites along each direction
  Beta = 100             # inverse temperature
  [Parameter]
  nset = 5               # set of Monte Carlo sweeps
  npre = 10              # MCSteps to estimate hyperparameter
  ntherm = 10            # MCSweeps for thermalization
  nmcs = 10              # MCSweeps for measurement

なお, このツールで作成される格子は周期的境界条件に従うために, 
2サイト系でもボンドが2本生成されます.
そのため,  コメント(``#`` 以下の文章) にもある通り, 相互作用として -1 ではなく -0.5 を指定しています.

このファイルを dsqss_pre.py に与えます.
::

  $ python $DSQSS_ROOT/bin/dsqss_pre.py -i std.in

この結果, パラメータファイル param.in, 格子定義ファイル lattice.xml, アルゴリズム定義ファイル algorithm.xml と,
アルゴリズム定義ファイル作成のための中間ファイル hamiltonian.xml が作成されます.

計算の実行
**********

入力ファイルを作成したら, DSQSS/DLA による計算を実行します.
::

  $ $DSQSS_ROOT/bin/dla_H param.in


なお, 計算を実行するときにMPI を用いることで, 乱数並列計算が可能です.
::

  $ mpiexec -np 4 $DSQSS_ROOT/bin/dla_H param.in

並列数 (今回は4) だけ独立に計算を行い, モンテカルロサンプル数を増やすことで計算精度を向上できます.


計算結果の解釈
****************

計算結果は出力ファイル sample.log に書き出されます.
サイトあたりのエネルギーは ``ene`` という名前で記されており, たとえば ``grep`` コマンドで
::

  $ grep ene sample.log
  R ene = -3.75780000e-01 8.89848302e-04

と取得できます. 
2つある数字はそれぞれ期待値と統計誤差です.
反強磁性ハイゼンベルグダイマーの基底状態でのサイトあたりエネルギーは :math:`-3|J|/8 = -0.375|J|` なので, 問題なく計算されていることがわかります.
