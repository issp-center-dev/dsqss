.. -*- coding: utf-8 -*-
.. highlight:: none

入力ファイル
==============================

DLAとPMWAの入力ファイルでは、共通するパラメータが多い。
ここでDLAと使用方法が異なるもしくは新規に追加されたパラメータについて記載する。

- 計算条件のパラメータ

.. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Parameter, type, 備考
     RUNTYPE, int, 計算モード(0: 通常計算、1: リスタート計算)
     CB, int , 初期配置 (0: Vacuum、1: Checker-Board、2: Random)
     NTEST, int, ワーム数 (Number of Test)
     NMCSE, int, 空回しをする数 (Number of Monte Carlo Steps Empty)
     NMCS, int, 測定する際のモンテカルロステップ (Number of Monte Carlo Steps)
     NMCSD, int, 測定間の間隔数 (Number of Monte Carlo Depart?)
     SEED, int, 0以上の場合は実際に使用するシード、負の場合は乱数でシードを与える。
     NVERMAX, int, バーテックスの最大数 (デフォルト数 :math:`10^8` )、-1の場合は上限なし。
     NWORMAX, int, ワームの最大数 (デフォルト数 :math:`10^3` )、-1の場合は上限なし。
     SFINPFILE, str, 入力された場合、本ファイルで指定されたStructure Factorsを計算する。
     SFOUTFILE, str, 入力された場合、Structure Factorsを指定したファイルに出力する(デフォルトはsf.out)。
     Step_x, int, 相関関数を計算する場合の空間幅を与える(デフォルト:1)。
     Step_k, int, 波数表示の相関関数を計算する場合の波数空間幅を与える(デフォルト:1)。
     CFOUTFILE, str, 入力された場合、相関関数を指定したファイルに出力する(デフォルトはcf.out)。

ここで、NVERMAX、NWORMAXについては自動でリサイズして決定する(ver. 1.2.0で実装)。

- 相互作用関連のパラメータ

.. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Parameter, type, 備考 
     tb, double , ボソン系では :math:`t_b` を、スピン系では :math:`J_{xy}` を表す。
     UBB, double, ボソン系で :math:`U_{BB}` を表す。スピン系では使用されない。
     VB1, double, ボソン系で :math:`V_{B1}` を、スピン系では :math:`J_{z}` を表す。
     VB2, double, ボソン系で :math:`V_{B2}` を表す。スピン系では使用されない。
     MU, double,  ボソン系では :math:`\mu` を、スピン系では :math:`H` を表す。
     G, double, ボソン系では :math:`\Gamma` を、スピン系では :math:`\Gamma/2` を表す。
     NMAX, \-, 1に固定


以下、入力ファイル例を記載する::

	RUNTYPE = 0
	NSET  = 10
	NMCSE = 10000
	NMCSD = 10000
	NMCS  = 10000
	NTEST = 10000
	SEED  = 31415
	NC    = 0
	NVERMAX = 10000000
	NWORMAX = 1000
	algfile   = algorithm.xml
	latfile   = lattice.xml
	outfile   = sample.log
	CB      = 2
	G       = 0.3
	UBB     = 0
	VB1     = 3
	VB2     = 0
	tb      = 1
	MU      = 2
	NMAX    = 1

