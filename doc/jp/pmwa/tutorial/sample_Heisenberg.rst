.. -*- coding: utf-8 -*-
.. highlight:: none

DSQSS/PMWAによるスピン鎖のエネルギー計算
========================================
このチュートリアルでは, S=1/2の反強磁性ハイゼンベルグ鎖の基底状態エネルギー計算をすることで、DSQSS/PMWAの使い方を学びます. 

DSQSS/PMWAによる計算は,

1. 入力ファイルの準備
2. 計算の実行
3. 計算結果の解釈

の3段階に分かれます.

入力ファイルの準備
******************************************

DSQSS/PMWAを実行するには,

1. 格子定義ファイル
2. パラメータファイル

の2つの入力ファイルが必要です.
そのため, まずはこれらの入力ファイルを作成します.
そのためのユーティリティツールが ``dsqss_pre.py`` です.
これは単一の入力ファイルから, DSQSS/DLA および DSQSS/PMWA の入力ファイルを生成するPython スクリプトです.
まず, ``dsqss_pre.py`` の入力ファイルとして, 次の内容を持つテキストファイル ``std.in`` を準備します.

::

  [System]
  solver = PMWA
  [Hamiltonian]
  model_type = spin
  Jxy = -1.0
  Jz = -1.0
  Gamma = 0.1
  [Lattice]
  lattice_type = square
  D = 1
  L = 16
  Beta = 100
  [Parameter]
  CB = 1
  SEED = 31415
  NSET = 10
  NMCS = 100
  NPRE = 100
  NTHERM = 100
  NDECOR = 100

自分の好きなエディタで書くか, ``sample/pmwa/1DDimer`` ディレクトリにあるものを利用してください.
このファイルを ``dsqss_pre.py`` に与えます.

::

  $ $DSQSS_ROOT/bin/dsqss_pre.py -i std.in

この結果, パラメータファイル param.in, 格子定義ファイル lattice.xmlが作成されます.

計算の実行
**********

入力ファイルを作成したら, ``DSQSS/PMWA`` による計算を実行します.
::

  $ $DSQSS_ROOT/bin/pmwa_H param.in


なお, 計算を実行するときにMPI を用いることで, 乱数並列計算が可能です(入力ファイルの指定により空間分割, 虚時間方向の分割を行うこともできます. 詳細はDLAのユーザーマニュアルをご覧ください).

::

  $ mpiexec -np 4 $DSQSS_ROOT/bin/pmwa_H param.in

並列数 (今回は4) だけ独立に計算を行い, モンテカルロサンプル数を増やすことで計算精度を向上できます.

計算結果の解釈
****************

計算結果は出力ファイル sample.log に書き出されます.
エネルギーは
::

   $ grep ene sample.log
   R ene  = -0.5705441 0.0003774399737579577
   
となります. なお, DSQSS/PMWAの場合は横磁場を必ず入れる必要があります.
そのため, ゼロ磁場にするには外挿する必要があるので, ご注意ください.
