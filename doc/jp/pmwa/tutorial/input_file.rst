.. -*- coding: utf-8 -*-
.. highlight:: none

DSQSS/PMWAの入力ファイル
==============================

DSQSS/DLAとDSQSS/PMWAの入力ファイルでは, 共通するパラメータが多く存在します.
ここではDSQSS/DLAと使用方法が異なる, もしくは新規に追加されたパラメータについて記載します.

- 計算条件のパラメータ

.. csv-table::
     :header-rows: 1
     :widths: 2,1,1,4

     Parameter, type, default, 備考
     RUNTYPE, int, , "計算モード(0: 通常計算,1: リスタート計算)"
     CB, int , 0, "初期配置 (0: Vacuum,1: Checker-Board,2: Random)"
     NSET, int, 10, "モンテカルロ計算の繰り返し数"
     NMCS, int, 1000, "測定する際のモンテカルロステップ (Number of Monte Carlo Steps)"
     NPRE, int, 1000, "ハイパーパラメータ決定のためのモンテカルロステップ数 (Number of Pre-calculation)"
     NTHERM, int, 1000, "空回しモンテカルロステップ数 (Number of Monte Carlo Steps for Thermalization)"
     NDECOR, int, 1000, "測定間の間隔数 (Decorrelation)"
     SEED, int, 13, "0以上の場合は実際に使用するシード,負の場合は乱数でシードを与える."
     NVERMAX, int, 100000000, "バーテックスの最大数,-1の場合は上限なし."
     NWORMAX, int, 1000, "ワームの最大数,-1の場合は上限なし."
     SFINPFILE, str, , "入力された場合,本ファイルで指定されたStructure Factorsを計算する."
     SFOUTFILE, str, sf.out, "入力された場合,Structure Factorsを指定したファイルに出力する."
     Step_x, int, 1, "相関関数を計算する場合の空間幅を与える."
     Step_k, int, 1, "波数表示の相関関数を計算する場合の波数空間幅を与える."
     CFOUTFILE, str, cf.out,"入力された場合,相関関数を指定したファイルに出力する."

- 模型関連のパラメータ

.. csv-table::
     :header-rows: 1
     :widths: 2,1,4

     Parameter, type, 備考 
     beta, double, "逆温度."
     t, double, "ボソン系では :math:`t` を, スピン系では :math:`J_{xy}` を表す."
     U, double, "ボソン系で :math:`U` を表す. スピン系では使用されない."
     V, double, "ボソン系で :math:`V` を, スピン系では :math:`J_{z}` を表す."
     MU, double,  "ボソン系では :math:`\mu` を, スピン系では :math:`H` を表す."
     G, double, "ボソン系では :math:`\Gamma` を, スピン系では :math:`\Gamma/2` を表す."
     NMAX, \-, "1に固定"

- 格子関連のパラメータ

.. csv-table::
     :header-rows: 1
     :widths: 2,1,4

     Parameter, type, default, 備考 
     NLdiv, int, 1, Lの分割数 (各次元についてそれぞれ NLdiv分割します)
     NBdiv, int, 1, Bの分割数
     NFIELD, int, 0, 磁場の種類の数(基本的には0に設定)


入力ファイル例を以下に示します.
::

  RUNTYPE = 0
  NSET  = 10
  NMCS  = 1000
  NPRE = 1000
  NTHERM = 1000
  NDECOR = 1000
  SEED  = 31415
  NC    = 0
  NVERMAX = 10000000
  NWORMAX = 1000
  algfile   = algorithm.xml
  latfile   = lattice.xml
  outfile   = sample.log
  CB      = 2
  G       = 0.3
  U       = 0
  V       = 3
  t       = 1
  MU      = 2
  NMAX    = 1

