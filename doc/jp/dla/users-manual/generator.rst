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
パラメータリストは :ref:`std_toml_lattice` を参照してください。

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
パラメータリストは :ref:`std_toml_hamiltonian` を参照してください。

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

``LAT``
   読み込む格子 dat/TOML ファイル.省略した場合は ``lattice.dat`` が指定されます.

``HAM``
   読み込むハミルトニアン TOML ファイル.省略した場合は ``hamiltonian.toml`` が指定されます.

``LATXML``
   書き出される格子定義ファイル. 省略した場合は ``lattice.xml`` が指定されます.

``ALGXML``
   書き出されるアルゴリズム定義ファイル.省略した場合は ``algorithm.xml`` が指定されます.

``without_lattice``
   設定した場合、格子定義ファイルは書き出されません。
   なお、格子ファイル ``LAT`` そのものは、アルゴリズムなどの導出に必要なために読み込まれます。

``without_algorithm``
   設定した場合、アルゴリズム定義ファイルは書き出されません。

``KPOINT``
   読み込む波数ファイル.省略した場合は ``kpoints.dat`` が指定されます.

``SF``
   書き出される構造因子定義ファイル. 省略した場合は ``sf.xml`` が指定されます.

``NTAU``
   構造因子などの計算に用いる虚時間の離散化数（トロッター数）

``TAUCUTOFF``
   構造因子などの計算における虚時間方向のカットオフ。

``CF``
   書き出される変位定義ファイル。省略した場合は ``cf.xml`` が指定されます。

``--distance-only``
   指定した場合、変位定義において変位 :math:`\vec{r}_{ij}` ではなくその絶対値 :math:`r_{ij}` でグループ化します。

``DISPLACEMENT_ORIGIN``
   変位定義について、変位ベクトルの始点を指定したサイト番号で固定します。
   省略した場合は、すべてのサイト対に関して変位が定義されます。

``KERNEL``
   バーテックスにおけるワームヘッドの散乱確率の導出に使うアルゴリズム。省略した場合、 ``suwa todo`` が用いられます。

   ``suwa todo``
      詳細釣り合いを破る諏訪・藤堂アルゴリズムを用います。
      (H. Suwa and S. Todo, PRL 105, 120603 (2010))
   
   ``reversible suwa todo``
      詳細釣り合いを満たす諏訪・藤堂アルゴリズムを用います。 (arXiv:1106.3562)

   ``heat bath``
      熱浴法を用います。

   ``metropolice``
      メトロポリスアルゴリズムを用います。

.. _TOML: https://github.com/toml-lang/toml/blob/master/versions/ja/toml-v0.5.0.md
