.. -*- coding: utf-8 -*-
.. highlight:: none

DSQSS/PMWAの入力ファイルの作成
==============================

PMWAを実行するには,テキスト形式の入力ファイルが必要です.
入力ファイルでは計算条件を指定するパラメータと
ハミルトニアンを指定するパラメータの二種類があります.

以下, 入力ファイル例を記載します
::

	RUNTYPE = 0
	NSET   = 10
	NMCS   = 10000
	NPRE   = 10000
	NTHERM = 10000
	NDECOR = 10000
	SEED  = 31415
	NC    = 0
	NVERMAX = 10000000
	NWORMAX = 1000
	latfile   = lattice.xml
	outfile   = sample.log
	CB      = 2
	G       = 0.3
	U       = 0
	V       = 3
	t       = 1
	MU      = 2
	NMAX    = 1

各パラメータの意味は下記の通りです.

- 計算条件のパラメータ

  .. csv-table::
     :header-rows: 1
     :widths: 2,1,4

     Parameter, type, 備考
     RUNTYPE, int, "計算モード(0: 通常計算,1: リスタート計算)"
     CB, int , "初期配置 (0: Vacuum,1: Checker-Board,2: Random)"
     NSET, int, モンテカルロ計算の繰り返し数
     NMCS, int, 物理量計算に用いるモンテカルロスイープ数
     NPRE, int, 1スイープあたりのワーム生成試行回数を決めるための事前計算につかうモンテカルロステップ数
     NMCSE, int, 初期緩和に用いるモンテカルロスイープ数
     NMCSD, int, 測定間のモンテカルロスイープ数
     SEED, int, "0以上の場合は実際に使用するシード,負の場合は乱数でシードを与える."
     NVERMAX, int, "バーテックスの最大数 (デフォルト数 :math:`10^8` ),-1の場合は上限なし."
     NWORMAX, int, "ワームの最大数 (デフォルト数 :math:`10^3` ),-1の場合は上限なし."
     SFINPFILE, str, "入力された場合,本ファイルで指定されたStructure Factorsを計算する."
     SFOUTFILE, str, "入力された場合,Structure Factorsを指定したファイルに出力する(デフォルトはsf.out)."
     Step_x, int, 相関関数を計算する場合の空間幅を与える(デフォルト:1).
     Step_k, int, 波数表示の相関関数を計算する場合の波数空間幅を与える(デフォルト:1).
     CFOUTFILE, str, "入力された場合,相関関数を指定したファイルに出力する(デフォルトはcf.out)."

ここで,NVERMAX,NWORMAXについては自動でリサイズして決定します(ver. 1.2.0で実装).

- 相互作用関連のパラメータ

PMWAではハードコアボソン系(サイトに最大1つのボソン)とS=1/2のXXZ模型について計算可能です.ハードコアボソン系のハミルトニアンは

.. math::
   {\cal H} &= -t_{b} \sum_{\langle i, j\rangle}b_i^{\dagger} b_j + U_{BB}\sum_i n_i(n_i -1)
   +V_{B1}\sum_{\langle i, j\rangle} n_i n_j \\ 
   &+\mu\sum_i n_i-\Gamma\sum_i(b_i^{\dagger}+b_i),

で与えられます.ここで :math:`\langle i,j \rangle` は最近接のペアを表します.
S=1/2のXXZ模型は

.. math::
   {\cal H}^{XXZ} = -J_{xy} \sum_{\langle i, j\rangle}(S_i^x S_j^x + S_i^y S_j^y)-J_z\sum_{\langle i, j\rangle}S_i^zS_j^z-H \sum_{i}S_{i}^z -\Gamma \sum_i S_i^x,

で与えられます.
入力ファイルで指定するパラメータと上記式のパラメータは以下のように対応しています.

  .. csv-table::
     :header-rows: 1
     :widths: 1,1,1

     Parameter, Boson, Spin
     t, :math:`t_b`, :math:`J_{xy}` 
     U, :math:`U_{BB}`, `-`
     V, :math:`V_{B1}`, :math:`J_{z}`
     MU, :math:`\mu`, :math:`H`
     G, :math:`\Gamma`, :math:`\Gamma/2` 

各パラメータはdouble型で指定します.
