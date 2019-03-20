.. -*- coding: utf-8 -*-
.. highlight:: none

使用方法
---------------

DSQSSでは予め定義されたモデルを簡易実行できるよう, 
入力ファイル作成ツール ``dsqss_pre.py`` を用意しています.
ここでは, このツールを利用したDSQSSの実行方法について簡単に説明します.
なお, DSQSS/DLAでは, 複雑な模型をユーザーが定義し実行することも出来ます.
詳細については :ref:`chap_dla_usermanual` をご覧ください.

入力ファイルの作成
*******************************

``dsqss_pre.py`` の実行には, テキストベースの入力ファイルが必要です.
以下に, 入力ファイルの例を記載します.
::

 [System]
 solver = DLA
 [Hamiltonian]
 model_type = spin
 M = 1
 J = 1.0
 F = 0.0
 [Lattice]
 lattice_type = square
 D = 1
 L = 8
 Beta = 10
 [Parameter]
 NPRE = 1000
 NTHERM = 1000
 NMCS = 1000
 NSET = 10
 SEED = 31415
 NVERMAX = 10000000
 algfile = algorithm.xml
 latfile = lattice.xml
 outfile = sample.log
 
入力ファイルでは, 4つの区分で大別されるパラメータを指定します.
``keyword = parameter`` の形式で指定します.

1. Systemセクション
 
 ``solver`` をkeywordとして各種ソルバーを設定します.
 ソルバーは ``DLA`` と ``PMWA`` の2種類から選択することが可能です.
 
2. Hamiltonianセクション

 系の種別 (スピンもしくはボソン系)とハミルトニアンを構成するパラメータを指定します.
 
 .. csv-table::
   :header-rows: 1
   :widths: 2, 1, 1, 1, 4

   Parameter, Solver, type, default, 備考
   ``model_type`` , 共通, str, spin, "モデルのタイプは ``spin`` もしくは ``boson`` から選択します."

 - ``model_type = spin`` の場合
 
 .. csv-table::
   :header-rows: 1
   :widths: 2, 1, 1, 1, 4
	 
   Parameter, Solver, type, default, 備考 
   ``M`` , DLA, int, 1, "局在スピンの大きさ :math:`S` の2倍に等しい整数."
   ``F`` , DLA, double, 0.0, "サイトにかかるボンドあたりの磁場 :math:`F=h/z` . :math:`z` はサイトの配位数で, 例えば正方格子なら :math:`z=4` ."
   ``J`` , DLA, double, 1.0, "交換相互作用. 正で強磁性, 負で反強磁性."
   ``Jxy``, PMWA, double, 0.0, "交換相互作用 :math:`J_{xy}` . 正で強磁性, 負で反強磁性."
   ``Jz``, PMWA, double, 0.0, "交換相互作用 :math:`J_{z}` . 正で強磁性, 負で反強磁性."
   ``h``, PMWA, double, 0.0, "縦磁場 :math:`h` . 正で強磁性, 負で反強磁性."
   ``Gamma`` , PMWA, double, 0.0, "横磁場 :math:`\Gamma` . 正で強磁性, 負で反強磁性."
 
 - ``model_type = boson`` の場合
 
 .. csv-table::
   :header-rows: 1
   :widths: 2, 1, 1, 1, 4
	 
   Parameter, Solver, type, default, 備考
   ``t`` , 共通, double, 1.0, "粒子のホッピングパラメータ."
   ``U`` , 共通, double, 0.0, "サイト内二体相互作用. 正が斥力."
   ``V`` , 共通, double, 0.0, "最近接二体相互作用. 正が斥力."
   ``M`` , DLA, int, 1, "サイトあたりに占めることのできる粒子数の最大値."
   ``F`` , DLA, double, 0.0, "ボンドあたりの化学ポテンシャル :math:`F=\mu/z` . :math:`z` はサイトの配位数で, 例えば正方格子なら :math:`z=4` ."
   ``mu``, PMWA, double, 0.0, "化学ポテンシャル."
   ``Gamma`` , PMWA, double, 0.0, "ソースターム( :math:`b_i^{\dagger}+b_i` の係数)."
 
 ハミルトニアンを構成するパラメータの詳細については, 
 ``DSQSS/DLA`` については ``hamgen_B`` もしくは ``hamgen_H`` の入力ファイルの説明を, 
 ``DSQSS/PMWA`` では入力ファイルの説明をご覧ください.

3. Latticeセクション

 格子形状と逆温度を指定するセクションです. 

 .. csv-table::
   :header-rows: 1
   :widths: 2, 1, 1, 4
	 
   Parameter, type, default, 備考
   ``lattice_type`` , str, square, "格子形状を超格子 ``square`` もしくは三角格子 ``triangular`` から選択します (格子形状の種類は順次追加予定). ``PMWA`` では ``square`` のみ選択可能です."
   ``D`` , int, , "格子の次元. "
   ``L`` , int, , "格子のサイズ. , で区切って``D`` 次元空間のサイズを指定します. 例えば, 二次元 :math:`2 \times 4` の格子の場合は ``L = 2, 4`` として指定します. "
   ``Beta`` , double, 10.0, "逆温度"
   ``NLdiv`` , int, 1, "( ``DSQSS/PMWA`` のみ使用)： ``L`` で指定された格子の分割数(各次元について等分割します)."
   ``NBdiv``, int, 1, "( ``DSQSS/PMWA`` のみ使用)： ``Beta`` の分割数."
 
 各パラメータのより詳細な説明は, 対応する実行ファイルの説明をご覧ください.
 
 
4. Parameterセクション

 計算条件を指定するセクションで, ``DSQSS/DLA`` および ``DSQSS/PMWA`` の入力ファイルと共通のキーワードを使用してパラメータを設定します.
 定義可能なパラメータの詳細については各ソルバーの入力ファイルを参考にしてください.

実行方法
*******************************

入力ファイル作成後は以下のコマンドを入力することで, ソルバー用の入力ファイルが作成されます(以下では入力ファイル名を ``std.in`` としています).
::

 $ dsqss_pre.py -i std.in


``DSQSS/DLA`` の場合は, ``algorithm.xml, hamiltonian.xml, lattice.xml, param.in`` が出力されます.
``DSQSS/PMWA`` の場合には, ``lattice.xml, param.in`` が出力されます.
ソルバーの実行方法は, ``dsqss_pre.py`` の標準出力の最後に
::

 Please type: xxxxxx

 
として, ``xxxxxx`` に実行すべきコマンドが記載されます(例えば, ``DLA_H param.in``).
MPIを利用する場合には, ``xxxxxx`` の前に ``mpirun -np 8`` などをつけて実行してください。
なお, DSQSSでは並列する乱数の数, PMWAでは並列する乱数の数と総分割数 (空間分割数と虚時間分割数の積)の積をプロセス数に指定します.
コマンドの実行結果の詳細については各ソルバーのチュートリアル・出力結果に記載してありますので, そちらを参照してください.


モンテカルロ計算の流れ
*****************************

:numref:`fig_nmcs` にモンテカルロ計算の流れを示します。

.. figure:: ../../image/dsqss/nmcs.*
   :name: fig_nmcs
   :alt: モンテカルロ計算の流れとパラメータの図解

   モンテカルロ計算のおおまかな流れと各種回数パラメータの図解

DSQSS では, ワームヘッド対が生成されてから消滅するまでを 1 MCステップと呼び,
:math:`N_\text{cyc}` MCステップをまとめて 1 MCスイープと呼びます.
(ワームヘッド対の生成そのものに失敗した場合も 1 MC ステップです.)
:math:`N_\text{cyc}` の値ははじめの ``NPRE`` MC ステップで決定されます.

:math:`N_\text{cyc}` が決まったら, 初期緩和フェーズとして ``NTHERM`` MC スイープのシミュレーションが行われ,
引き続き ``NMCS`` MC スイープのシミュレーションが物理量測定フェーズとして行われます.

ある物理量測定フェーズと次の物理量測定過程フェーズとの間には,
自己相関軽減フェーズとして ``NDECOR`` MC スイープのシミュレーションが行われます.

初期緩和フェーズと物理量測定フェーズ, あるいは自己相関軽減フェーズと物理量測定フェーズの2つのフェーズをあわせたものが
1 セット となり,
全体のシミュレーションは ``NSET`` 個のセットを含みます.
物理量の期待値 :math:`\langle Q \rangle` と誤差 :math:`\sigma_Q` は,
``NSET`` 個のセットそれぞれから得られる物理量の平均および標準誤差として得られます.
