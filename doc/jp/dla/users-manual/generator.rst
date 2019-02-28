.. highlight:: none

DLA の入力ファイル生成ツール
=============================

DLA は入力ファイルとして格子定義ファイル,アルゴリズム定義ファイル,
構造因子定義ファイル,実空間表示温度グリーン関数定義ファイル,波数空間表示温度グリーン関数定義ファイルを,それぞれXML 形式ファイルとして受け取ります.
これらをうまく定義することで,計算機資源の許す範囲で任意の格子や模型を計算できますが,
手で定義するには複雑になっています.
そのため,超立方格子やハイゼンベルグ模型などのよく使われるような格子・模型については生成ツールが用意されています.

格子生成ツール ``dla_latgen``
************************************
``dla_latgen`` は `TOML`_ 形式の入力ファイルから格子 datファイルを生成するツールです. ::

  $ dla_latgen [-o filename] <inputfile>

パラメータは以下の通り.

``filename``
   出力ファイル名.デフォルトは ``lattice.dat`` です.

``inputfile``
  入力ファイル名. 

実行すると filename で指定した名前の格子定義ファイルが生成されます.

入力は TOML 形式です.
このファイルには ``lattice``, ``dim``, ``L``, ``bc``  のキーを持つ ``lattice`` テーブルが必要です.

``lattice``
   格子の種類.  超立方格子 "hypercubic" が利用可能です.

``dim``
   空間次元. 

``L``
   格子の大きさ. 整数の配列か整数で指定します.
   ``dim`` で指定した空間次元より少ない場合, 足りない要素は最後の要素で自動的に埋められます.

``bc``
   格子の境界条件. ブール値の配列で指定します.
   ``true`` が周期的境界条件を, ``false`` が開境界条件を示します.

入力ファイル例
::

   # 1次元鎖, 8 sites
   [lattice]
   lattice = "hypercubic"
   dim = 1
   L = 8

   # 2次元正方格子, 4x4 sites
   [lattice]
   lattice = "hypercubic"
   dim = 2
   L = 4

   # leg はしご格子, 8x2 sites
   [lattice]
   lattice = "hypercubic"
   dim = 2
   L = [8,2]
   bc = [true, false]


ハミルトニアン生成ツール ``dla_hamgen``
*****************************************

``dla_hamgen`` は `TOML`_ 形式の入力ファイルからハミルトニアン TOML ファイルを生成するツールです.

.. math:
   \mathcal{H} = -J  \sum_{\langle i, j \rangle} S_i \cdot S_j - h \sum_i S_i^z

を表すハミルトニアンファイルを生成するツールです.
::

  $ dla_hamgen [-o filename] <inputfile>

パラメータは以下の通り.

``filename``
   出力ファイル名.デフォルトは ``hamiltonian.toml`` です.

``inputfile``
  入力ファイル名. 

実行すると filename で指定した名前を持つファイルが生成されます.

入力は TOML 形式です.
このファイルには少なくとも ``model``, ``M`` のキーを持つ ``hamiltonian`` テーブルが必要です.

``model``
   生成する模型の種類.  XXZ 模型 ``spin`` と ボーズハバード模型 ``boson`` が利用可能です.

``M``
   サイトあたりの取りうる状態数-1.
   XXZ 模型では局所スピンの大きさ :math:`2S` を, ボーズハバード模型では粒子数カットオフを指定します.

XXZ 模型

.. math:
   \mathcal{H} = \sum_{\langle i, j \rangle} -J_z S_i^z S_j^z -\frac{J_{xy}}{2} \left( S_i^+ S_j^- + S_i^- S_j^+ \right)
   + D \sum_i \left(S_i^z\right)^2
   - h \sum_i S_i^z

に特有のパラメータは次の通り.

``Jz``, ``Jxy``
   交換相互作用. 相互作用の種類が複数ある場合は, 配列で指定する.
   正が強磁性的相互作用を, 負が反強磁性的相互作用を意味する.

``D``
   オンサイトのスピン異方性パラメータ. サイトの種類が複数ある場合は, 配列で指定する.

``h``
   オンサイトの磁場. サイトの種類が複数ある場合は, 配列で指定する.

ボーズハバード 模型

.. math:
   \mathcal{H} = \sum_{\langle i, j \rangle} \left[ -t b_i^\dagger \cdot b_j + h.c. + V n_i n_j \right] + \sum_i \left[ \frac{U}{2} n_i(n_i-1) - \mu n_i \right]

に特有のパラメータは次の通り.

``t``
   粒子のホッピングパラメータ. 種類が複数ある場合は, 配列で指定する.

``V``
   オフサイトの相互作用. 相互作用の種類が複数ある場合は, 配列で指定する.

``U``
   オンサイトの相互作用. 相互作用の種類が複数ある場合は, 配列で指定する.

``h``
   化学ポテンシャル. サイトの種類が複数ある場合は, 配列で指定する.

入力ファイル例
::

   # S=1/2 AF Heisenberg model
   [hamiltonian]
   model = "spin"
   M = 1
   Jz = -1.0
   Jxy = -1.0
    
   # S=1 J1 AF J2 FM XY model under the field
   [hamiltonian]
   model = "spin"
   M = 2
   Jxy = [-1.0, 1.0]
   h = 1.0

   # hardcore boson
   [hamiltonian]
   model = "boson"
   M = 1
   t = 1.0
   V = 1.0

   # softcore boson (upto N=2)
   [hamiltonian]
   model = "boson"
   M = 2
   t = 1.0
   U = 1.0
   V = 1.0
   mu = 1.0

波数ファイル生成ツール ``dla_sfgen``
*************************************
``dla_sfgen`` は `TOML`_ 形式の入力ファイルから波数ファイル ``kpoints.dat`` を生成するツールです.
::

  $ dla_sfgen [-o filename] [-s size] <inputfile>

パラメータは以下の通り.

``filename``
   出力ファイル名.デフォルトは ``hamiltonian.toml`` です.

``size``
   格子サイズ. 数字を空白区切りで並べた文字列で指定します (e.g. ``-s "4 4"`` .)
   指定しない場合は, 入力ファイルの ``[lattice]`` テーブルから読み取ります.

``inputfile``
  入力ファイル名. 

実行すると filename で指定した名前の波数ファイルが生成されます.

入力は TOML 形式です.
このファイルには ``ksteps`` キーを持つ ``kpoints`` テーブルが必要です.

``ksteps``
   波数の増分を表します.  整数の配列か整数で指定します.
   空間次元よりも要素数が少ない場合、足りない要素は指定された最後の要素で埋められます.


アルゴリズム生成ツール ``dla_alg``
*************************************
``dla_alg`` は格子 dat/TOML ファイル, ハミルトニアン TOML ファイル, 波数ファイル から
格子 XML ファイル, アルゴリズム XML ファイル, 波数 XML ファイル, 変位 XML ファイルを生成するツールです.
::

   $ dla_alg [-l LAT] [-h HAM] [-L LATXML] [-A ALGXML]
             [--without_lattice] [--without_algorithm] [-k KPOINT] [--sf SF]
             [--ntau NTAU] [--taucutoff TAUCUTOFF] [--cf CF]
             [--distance-only] [--displacement-origin DISPLACEMENT_ORIGIN]
             [--kernel KERNEL]

パラメータは以下の通り.

``HFILE``
  読み込むハミルトニアンファイル.省略した場合は ``hamiltonian.xml`` が指定されます.

``AFILE``
  書き出されるアルゴリズム定義ファイル.省略した場合は ``algorithm.xml`` が指定されます.

.. _TOML: https://github.com/toml-lang/toml/blob/master/versions/ja/toml-v0.5.0.md
