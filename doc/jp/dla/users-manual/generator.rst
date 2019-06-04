.. highlight:: none

DLA の入力ファイル生成ツール
=============================

DLA は入力ファイルとして格子定義ファイル,アルゴリズム定義ファイル,
波数ベクトル定義ファイルを, それぞれXML 形式ファイルとして受け取ります.
これらをうまく定義することで, 計算機資源の許す範囲で任意の格子や模型を計算できますが,
手で定義するには複雑になっています.
そのため,超立方格子やハイゼンベルグ模型などのよく使われるような格子・模型については生成ツールが用意されています.


.. _dla_pre:

シンプルモードツール ``dla_pre``
***********************************
``dla_pre`` は :ref:`simple_mode_file` から :ref:`dla_expert_files` を生成するツールです. ::

   $ dla_pre [-p paramfile] <inputfile>

パラメータは以下の通り.

``paramfile``
  出力されるパラメータファイル名.デフォルトは ``qmc.inp`` です.

``inputfile``
  入力ファイル名. ファイル形式の詳細は :ref:`simple_mode_file` を参照してください.


格子XML ファイルなど, 出力される XML ファイルの名前はパラメータから自動生成されます. 


.. _dla_lat_gen:

格子生成ツール ``dla_latgen``
************************************
``dla_latgen`` は :ref:`simple_mode_file` から :ref:`lattice_data_file` や :ref:`lattice_toml_file` を生成するツールです. ::

  $ dla_latgen [-o datafile] [-t TOML] [-g GNUPLOT] input

パラメータは以下の通り.

``datafile``
   出力される格子データファイルの名前. デフォルトは ``lattice.dat`` です.
   空文字列の場合, 格子データファイルは出力されません.

``TOML``
   出力される格子TOMLファイルの名前. デフォルトは空文字列で, ファイルは出力されません.

``GNUPLOT``
   出力される格子Gnuplotファイルの名前. デフォルトは空文字列で, ファイルは出力されません.
   出力されたファイルを``gnuplot`` で ``load`` することで, 格子を可視化することができます.

``inputfile``
  入力ファイル名. ファイル形式の詳細は :ref:`std_toml_lattice` を参照してください.

実行すると datafile や TOML で指定した名前の格子定義ファイルが生成されます.

入力ファイル例
::

   # 1次元鎖, 8 sites
   [lattice]
   lattice = "hypercubic"
   dim = 1
   L = 8

::

   # 2次元正方格子, 4x4 sites
   [lattice]
   lattice = "hypercubic"
   dim = 2
   L = 4

::

   # 2本脚 はしご格子, 8x2 sites
   [lattice]
   lattice = "hypercubic"
   dim = 2
   L = [8,2]
   bc = [true, false]


ハミルトニアン生成ツール ``dla_hamgen``
*****************************************

``dla_hamgen`` は :ref:`simple_mode_file` から :ref:`hamiltonian_file` を生成するツールです. ::

  $ dla_hamgen [-o filename] <inputfile>

パラメータは以下の通り.

``filename``
   出力ファイル名.デフォルトは ``hamiltonian.toml`` です.

``inputfile``
  入力ファイル名.  ファイル形式は :ref:`std_toml_hamiltonian` を参照してください.

実行すると filename で指定した名前を持つファイルが生成されます.

入力ファイル例 ::

   # S=1/2 AF Heisenberg model
   [hamiltonian]
   model = "spin"
   M = 1
   Jz = -1.0
   Jxy = -1.0

::
    
   # S=1 J1 AF J2 FM XY model under the field
   [hamiltonian]
   model = "spin"
   M = 2
   Jxy = [-1.0, 1.0]
   h = 1.0

::

   # hardcore boson
   [hamiltonian]
   model = "boson"
   M = 1
   t = 1.0
   V = 1.0

::

   # softcore boson (upto N=2)
   [hamiltonian]
   model = "boson"
   M = 2
   t = 1.0
   U = 1.0
   V = 1.0
   mu = 1.0


パラメータファイル生成ツール ``dla_pgen``
******************************************
``dla_pgen`` は :ref:`simple_mode_file` から :ref:`expert_param_file` を生成するツールです.
::

  $ dla_pgen [-o filename] <inputfile>

パラメータは以下の通り.

``filename``
   出力ファイル名.デフォルトは ``param.in`` です.

``inputfile``
  入力ファイル名.  ファイル形式は :ref:`std_toml_parameter` を参照してください.



波数ファイル生成ツール ``dla_wvgen``
*************************************
``dla_wvgen`` は :ref:`simple_mode_file` から :ref:`wavevector_file` を生成するツールです.
::

  $ dla_wvgen [-o filename] [-s size] <inputfile>

パラメータは以下の通り.

``filename``
   出力ファイル名.デフォルトは ``kpoints.dat`` です.

``size``
   格子サイズ. 数字を空白区切りで並べた文字列で指定します (e.g. ``-s "4 4"`` .)
   指定しない場合は, 入力ファイルの ``[lattice]`` テーブルから読み取ります.

``inputfile``
  入力ファイル名.  ファイル形式は :ref:`simple_mode_kpoints` を参照してください.

実行すると filename で指定した名前の波数ファイルが生成されます.


アルゴリズム生成ツール ``dla_alg``
*************************************
``dla_alg`` は
:ref:`lattice_data_file`, :ref:`lattice_toml_file`, :ref:`hamiltonian_file`, :ref:`wavevector_file` から
:ref:`lattice_xml_file`, :ref:`algorithm_xml_file`, :ref:`wavevector_xml_file`, :ref:`relative_coordinate_xml_file` を生成するツールです.
::

   $ dla_alg [-l LAT] [-h HAM] [-L LATXML] [-A ALGXML]
             [--without_lattice] [--without_algorithm] [-k KPOINT]
             [--wv WV] [--disp DISP] [--distance-only]
             [--kernel KERNEL]

パラメータは以下の通り.

``LAT``
   読み込む格子 dat/TOML ファイル.省略した場合は ``lattice.dat`` が指定されます.

``HAM``
   読み込むハミルトニアン TOML ファイル.省略した場合は ``hamiltonian.toml`` が指定されます.

``LATXML``
   書き出される格子定義ファイル. 省略した場合は ``lattice.xml`` が指定されます.

``ALGXML``
   書き出されるアルゴリズム定義ファイル.省略した場合は ``algorithm.xml`` が指定されます.

``without_lattice``
   設定した場合, 格子定義ファイルは書き出されません. 
   なお, 格子ファイル ``LAT`` そのものは, アルゴリズムなどの導出に必要なために読み込まれます. 

``without_algorithm``
   設定した場合, アルゴリズム定義ファイルは書き出されません. 

``KPOINT``
   読み込む波数ファイル.省略した場合は波数ベクトルXML ファイルは出力されません.

``WV``
   書き出される波数ベクトルXMLファイル. 省略した場合は ``wavevector.xml`` が指定されます.

``DISP``
   書き出される相対座標定義ファイル. 省略した場合は相対座標XML ファイルは出力されません.

``--distance-only``
   指定した場合, 変位定義において変位 :math:`\vec{r}_{ij}` ではなくその絶対値 :math:`r_{ij}` でグループ化します. 

``KERNEL``
   バーテックスにおけるワームヘッドの散乱確率の導出に使うアルゴリズム. 省略した場合,  ``suwa todo`` が用いられます. 
   利用できるアルゴリズムは :ref:`simple_mode_algorithm` を参照してください.
