.. highlight:: none

.. _sec_dla_input:

DLA の入力ファイル
========================

入力ファイル一覧
************************

.. csv-table::
    :header-rows: 0
    :widths: 1,4

    qmc.inp, "モンテカルロの繰り返し回数など,計算制御のためのパラメータファイル."
    lattice.xml, "格子の定義ファイル."
    algorithm.xml, "アルゴリズム（ワームの散乱確率など）の定義ファイル."
    sf.xml, "構造因子の波数定義ファイル."
    cf.xml, "実空間温度グリーン関数の座標定義ファイル."


スタンダードモード入力ファイル ``std.toml``
********************************************
スタンダードモード入力ファイルは `TOML`_ 形式のテキストファイルです。
``dla_pre`` や ``dla_hamgen`` などのツールの入力ファイルとして使います。


.. _std_toml_parameter:

``parameter``
+++++++++++++++++

逆温度やモンテカルロステップ数などの計算条件を指定するテーブルです。
``dla_pre`` で使用されます。

``parameter`` テーブルに属するキーのリストと意味を次に示します。

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    beta, double, --, "逆温度."
    npre, int, 1000, "モンテカルロスイープ中のワーム対生成回数を求める事前計算のためのモンテカルロ試行回数."
    ntherm, int, 1000, "熱平衡化のためのモンテカルロスイープ数."
    ndecor, int, 1000, "セット間の自己相関を取り除くためのモンテカルロスイープ数."
    nmcs, int, 1000, "物理量計算のためのモンテカルロスイープ数."
    nset, int, 10, "モンテカルロ計算の繰り返し数."
    simulationtime, int,  0.0, "計算時間（単位は秒）."
    seed, int, 198212240, "疑似乱数の種."
    nvermax, int,  10000, "最大バーテックス数."
    nsegmax, int,  10000, "最大セグメント数."
    algfile, int,  algorithm.xml, "アルゴリズム定義ファイル名."
    latfile, string, lattice.xml, "格子定義ファイル名."
    sfinpfile, string, --,  "構造因子定義ファイル名. 空文字列の場合, 構造因子は計算されない."
    cfinpfile, string,  --, "実空間表示温度グリーン関数定義ファイル名. 空文字列の場合, 実空間表示温度グリーン関数は計算されない."
    ckinpfile, string,  --, "波数空間表示温度グリーン関数定義ファイル名. 空文字列の場合, 波数空間表示温度グリーン関数は計算されない."
    outfile, string, sample.log, "メイン出力ファイル名."
    sfoutfile, string, sf.dat, "構造因子出力ファイル名."
    cfoutfile, string, cf.dat, "実空間表示温度グリーン関数出力ファイル名."
    ckoutfile, string, ck.dat, "波数空間表示温度グリーン関数出力ファイル名."


.. _std_toml_lattice:

``lattice``
+++++++++++++++
格子の情報を指定するテーブルです。 ``dla_pre``, ``dla_latgen`` で使用されます。

``lattice`` テーブルに属するキーのリストと意味を次に示します。

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    lattice, string, --, "格子の種類. 超立方格子 'hypercubic' が利用可能です."
    dim, int, --, 空間次元.
    L, list(int) or int, --, "格子の大きさ. 整数の配列か整数で指定します.  空間次元より要素が少ない場合, 足りない要素は最後の要素で自動的に埋められます."
    bc, list(bool) or bool, true, "各次元に対する、格子の境界条件. ブール値の配列かブール値で指定します.  ``true`` が周期的境界条件を, ``false`` が開境界条件を示します."


.. _std_toml_hamiltonian:

``hamiltonian``
++++++++++++++++++++

ハミルトニアンの情報を指定するテーブルです。
``dla_pre``, ``dla_hamgen`` で使用されます。

``hamiltonian`` テーブルに属するキーのリストと意味を次に示します。

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    model, string, --, "模型の種類. XXZ 模型 'spin' とボーズハバード模型 'boson' が利用可能です."
    M, int, 1, "サイトあたりの取りうる状態数-1.  XXZ 模型では局所スピンの大きさ :math:`2S` を, ボーズハバード模型では粒子数カットオフを指定します."


XXZ 模型

.. math::
  \mathcal{H} = \sum_{\langle i, j \rangle} -J_z S_i^z S_j^z -\frac{J_{xy}}{2} \left( S_i^+ S_j^- + S_i^- S_j^+ \right)
  + D \sum_i \left(S_i^z\right)^2
  - h \sum_i S_i^z

に特有のパラメータは次の通り.

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    Jz, list(float) or float, 0.0, "交換相互作用. 相互作用の種類が複数ある場合は, 配列で指定する.  正が強磁性的相互作用を, 負が反強磁性的相互作用を意味する."
    Jxy, list(float) or float, 0.0, "交換相互作用. 相互作用の種類が複数ある場合は, 配列で指定する.  正が強磁性的相互作用を, 負が反強磁性的相互作用を意味する."
    D, list(float) or float, 0.0, "オンサイトのスピン異方性パラメータ. サイトの種類が複数ある場合は, 配列で指定する."
    h, list(float) or float, 0.0, "磁場. サイトの種類が複数ある場合は, 配列で指定する."


ボーズハバード 模型

.. math::
   \mathcal{H} = \sum_{\langle i, j \rangle} \left[ -t b_i^\dagger \cdot b_j + h.c. + V n_i n_j \right] + \sum_i \left[ \frac{U}{2} n_i(n_i-1) - \mu n_i \right]

に特有のパラメータは次の通り.

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    t, list(float) or float, 0.0, "ホッピングパラメータ. 相互作用の種類が複数ある場合は, 配列で指定する."
    V, list(float) or float, 0.0, "オフサイトの粒子間相互作用. 相互作用の種類が複数ある場合は, 配列で指定する.  正が斥力ポテンシャル, 負が引力ポテンシャルを意味する."
    U, list(float) or float, 0.0, "オンサイトの粒子間相互作用. サイトの種類が複数ある場合は, 配列で指定する. 正が斥力ポテンシャル, 負が引力ポテンシャルを意味する."
    mu, list(float) or float, 0.0, "化学ポテンシャル. サイトの種類が複数ある場合は, 配列で指定する."


パラメータファイル ``qmc.inp``
**********************************
パラメータファイルは次に示すような書式のプレーンテキストファイルです.

- 1行あたり1パラメータを, ``<name> = <value>`` という形式で表します.
- ファイル名以外は大文字小文字を区別しません.
- 空行,空白は無視されます.
- "#" から行末まではコメントとして無視されます.

パラメータのリストと意味を以下に示します.

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    beta, double, --, "逆温度."
    npre, int, 1000, "モンテカルロスイープ中のワーム対生成回数を求める事前計算のためのモンテカルロ試行回数."
    ntherm, int, 1000, "熱平衡化のためのモンテカルロスイープ数."
    ndecor, int, 1000, "セット間の自己相関を取り除くためのモンテカルロスイープ数."
    nmcs, int, 1000, "物理量計算のためのモンテカルロスイープ数."
    nset, int, 10, "モンテカルロ計算の繰り返し数."
    simulationtime, int,  0.0, "計算時間（単位は秒）. 詳細は後述."
    seed, int, 198212240, "疑似乱数の種."
    nvermax, int,  10000, "最大バーテックス数."
    nsegmax, int,  10000, "最大セグメント数."
    algfile, int,  algorithm.xml, "アルゴリズム定義ファイル名."
    latfile, string, lattice.xml, "格子定義ファイル名."
    sfinpfile, string, --,  "構造因子定義ファイル名. 空文字列の場合, 構造因子は計算されない."
    cfinpfile, string,  --, "実空間表示温度グリーン関数定義ファイル名. 空文字列の場合, 実空間表示温度グリーン関数は計算されない."
    ckinpfile, string,  --, "波数空間表示温度グリーン関数定義ファイル名. 空文字列の場合, 波数空間表示温度グリーン関数は計算されない."
    outfile, string, sample.log, "メイン出力ファイル名."
    sfoutfile, string, sf.dat, "構造因子出力ファイル名."
    cfoutfile, string, cf.dat, "実空間表示温度グリーン関数出力ファイル名."
    ckoutfile, string, ck.dat, "波数空間表示温度グリーン関数出力ファイル名."
    runtype, int, 0, 計算手法. "互換性および将来の拡張のために用意されている."

- simulationtime について

  - simulationtime > 0.0 のとき

    - 指定秒数が経過するか, 計算が完了したとき, 途中経過をチェックポイントファイルに書き出した後, プログラムを終了します.
    - 計算開始時にチェックポイントファイルがある場合, そのファイルを読み込んだ後に計算を再開します.
    - チェックポイントファイルの名前は outfile で指定されるメイン出力ファイル名の末尾に .cjob をつけたものです.

  - simulationtime <= 0.0 のとき

    - チェックポイントファイルは無視され, 書き出しも読み込みも行われません.



格子データファイル ``lattice.dat``
**************************************
格子データファイルは空間の情報, たとえばサイトの数やサイト同士のつながりかたなどを定義するための, 
テキストファイルです.
``dla_alg`` の入力ファイルとして扱われます。

``#`` から行末まではコメントとして読み飛ばされます。また、空白行も飛ばされます。
``name``, ``lattice``, ``directions``, ``sites``, ``interactions`` の5つのセクションから構成されます。

``name``
   格子の名前です。 1行のテキストからなります。 コメントや識別用であり、実際の計算では利用されません。

``lattice``
   格子の情報を指定するセクションです。

   - 1行目
      - 格子の次元を表す整数。
   - 2行目
      - 格子のサイズを表す、空白区切りで次元の数だけ並べられた整数の組。
   - 3行目
      - 格子の境界条件を表す、空白区切りで次元の数だけ並べられた整数の組。
        1 で周期的境界条件を、 0 で開放端境界条件を示します。
   - 4行目以降
      - 格子の基本並進ベクトルを表す、空白区切りで次元の数+1 だけ並べられた数の組。
        最初の整数はベクトルの番号を、残りの数はベクトルの要素を示します。

``directions``
   ボンド（二体相互作用）の向きを指定するセクションです。

   - 1行目
      - ボンドの向きの総数。
   - 2行目以降
      - 各向きを表すベクトルを表す、空白区切りで次元の数+1 だけ並べられた数の組。
        最初の整数は向きの番号を、残りの数はベクトルの要素を示します。
        ``lattice`` で指定された基本ベクトルを基底とします。

``sites``
   サイトの情報を指定するセクションです。

   - 1行目
      - 格子に含まれるサイトの総数を表す整数。
   - 2行目以降
      - 各サイトの情報を表す、空白区切りで次元の数+3 だけ並べられた数の組。

        - 1列目
            - サイト番号
        - 2列目
            - サイトタイプ
        - 3列目
            - 測定タイプ
        - 4列目以降
            - サイトの座標。 ``lattice`` で指定された基本ベクトルを基底とします。

``interactions``
   相互作用の空間情報を指定するセクションです。

   - 1行目
      - 格子に含まれる多体相互作用の総数を表す整数。
   - 2行目以降
      - 各相互作用の情報を表す、空白区切りで関与サイト数+5=N 個だけ並べられた数の組。

        - 1列目
            - 相互作用番号
        - 2列目
            - 相互作用タイプ
        - 3列目
            - 関与サイト数
        - 4-列目
            - 関与するサイトの番号
        - N-1列目
            - ボンドが周期境界をまたぐ場合には1, そうでない場合には0
        - N列目
            - ボンドの方向番号

二次元正方格子の例を示します。 ::

   name
   2 dimensional hypercubic lattice

   lattice
   2 # dim
   4 4 # size
   1 1 # 0:open boundary, 1:periodic boundary
   0 1.0 0.0 # latvec_0
   1 0.0 1.0 # latvec_1

   directions
   2 # ndirections
   # id, coords...
   0 1.0 0.0 
   1 0.0 1.0 

   sites
   16 # nsites
   # id, type, mtype, coord...
   0 0 0 0 0
   1 0 1 1 0
   2 0 0 2 0
   3 0 1 3 0
   4 0 1 0 1
   5 0 0 1 1
   6 0 1 2 1
   7 0 0 3 1
   8 0 0 0 2
   9 0 1 1 2
   10 0 0 2 2
   11 0 1 3 2
   12 0 1 0 3
   13 0 0 1 3
   14 0 1 2 3
   15 0 0 3 3

   interactions
   32 # nints
   # id, type, nbody, sites..., edge_flag, direction
   0 0 2 0 1 0 0
   1 0 2 0 4 0 1
   2 0 2 1 2 0 0
   3 0 2 1 5 0 1
   4 0 2 2 3 0 0
   5 0 2 2 6 0 1
   6 0 2 3 0 1 0
   7 0 2 3 7 0 1
   8 0 2 4 5 0 0
   9 0 2 4 8 0 1
   10 0 2 5 6 0 0
   11 0 2 5 9 0 1
   12 0 2 6 7 0 0
   13 0 2 6 10 0 1
   14 0 2 7 4 1 0
   15 0 2 7 11 0 1
   16 0 2 8 9 0 0
   17 0 2 8 12 0 1
   18 0 2 9 10 0 0
   19 0 2 9 13 0 1
   20 0 2 10 11 0 0
   21 0 2 10 14 0 1
   22 0 2 11 8 1 0
   23 0 2 11 15 0 1
   24 0 2 12 13 0 0
   25 0 2 12 0 1 1
   26 0 2 13 14 0 0
   27 0 2 13 1 1 1
   28 0 2 14 15 0 0
   29 0 2 14 2 1 1
   30 0 2 15 12 1 0
   31 0 2 15 3 1 1

格子TOML ファイル ``lattice.toml``
*************************************
格子TOML ファイルはユニットセルと基本並進ベクトルを用いて空間の情報を定義するための, 
`TOML`_ 形式のファイルです.
``dla_alg`` の入力ファイルとして扱われます。

``parameter`` と ``unitcell`` のふたつのテーブルから構成されます。

``parameter``
   格子の情報を記述するテーブルです。

   ``parameter.name``
      格子の名前です。実際の計算には用いられません。

   ``parameter.L``
      格子の大きさを表す整数配列です。

   ``parameter.bc``
      格子の境界条件を表すブーリアンの配列です。
      ``true`` が周期的境界条件を、 ``false`` が開放端境界条件を示します。

   ``parameter.basis``
      格子の基本並進ベクトルを表す2次元配列（配列の配列）です。

``unitcell``
   ユニットセルの情報を記述するテーブルです。

   ``unitcell.sites``
      ユニットセル内のサイトを示すテーブルの配列です。

      ``unitcell.sites.siteid``
         サイトのユニットセル内での識別番号です。

      ``unitcell.sites.type``
         サイトタイプを示す整数です。

      ``unitcell.sites.type``
         測定タイプを示す整数です。

      ``unitcell.sites.coord``
         ユニットセル内におけるサイトの局所座標を示す配列です。

   ``unitcell.bonds``
      ユニットセル内のボンドを示すテーブルの配列です。

      ``unitcell.bonds.bondid``
         ボンドのユニットセル内での識別番号です。

      ``unitcell.bonds.type``
         ボンドタイプを示す整数です。

      ``unitcell.bonds.source``
         ボンドの始点サイトの情報を表すテーブルです。

         ``unitcell.bonds.source.siteid``
            サイトのユニットセル内での識別番号です。

      ``unitcell.bonds.target``
         ボンドの終点サイトの情報を表すテーブルです。

         ``unitcell.bonds.target.siteid``
            サイトのユニットセル内での識別番号です。

         ``unitcell.bonds.target.offset``
            始点サイトの属するユニットセルから見た、
            終点サイトの属するユニットセルの相対座標です。


二次元正方格子の例を示します.
::

   [parameter]
   name = "square lattice"
   dim = 2
   L = [4,4]
   bc = [true, true]
   basis = [[1,0], [0,1]]

   [unitcell]

   [[unitcell.sites]]
   siteid = 0
   type = 0
   mtype = 0
   coord = [0,0]

   [[unitcell.bonds]]
   bondid = 0
   type = 0
   source = { siteid = 0 }
   target = { siteid = 0, offset = [1,0] }
   [[unitcell.bonds]]
   bondid = 1
   type = 0
   source = { siteid = 0 }
   target = { siteid = 0, offset = [0,1] }


格子XMLファイル ``lattice.xml``
**************************************
格子ファイルは空間の情報, たとえばサイトの数やサイト同士のつながりかたなどを定義するための, 
XML ライクな形式で記述されるテキストファイルです.
これは一般に複雑になりますが, より単純な格子データファイルや格子TOML ファイルから
``dla_alg`` を用いて生成することができます。

格子ファイルはただ一つの要素 Lattice を持ち, すべての情報は Lattice 要素の内容として含まれます.

Lattice
  ファイル全体の要素.
  ほかのすべての要素は Lattice のサブ要素です.

Lattice/Comment
  省略可能. コメント文を示し, 計算には使用されません.

Lattice/Dimension
  格子の次元.

Lattice/LinearSize
  ユニットセルを単位とした, 各次元の格子の長さ.
  内容として, スペース区切りの正整数を Lattice/Dimension で指定した数だけ並べたものをとります.
  ::

    <LinearSize> 3 4 </LinearSize>
    # ユニットセルが第1次元方向に3個, 第2次元方向に4個並んでいる場合

Lattice/NumberOfCells
  ユニットセルの総数. LinearSize で指定した各次元方向のサイズの積.

Lattice/NumberOfSites
  サイトの総数. ユニットセルの総数と1セル内のサイト数の積.

Lattice/NumberOfInteractions
  相互作用項の総数. 二体相互作用のみの場合は,いわゆる「ボンド数」.

Lattice/NumberOfSiteTypes
  サイトの種類数.

Lattice/NumberOfInteractionTypes
  相互作用の種類数.

Lattice/BondDimension
  Winding number を測定する際に定義する要素.

Lattice/NumberOfEdgeInteractions
  Winding number を測定する際に定義する要素. 格子の周期的境界をまたぐボンドの総数を指定します.

Lattice/S
  サイト情報. Lattice/NumberOfSites で指定したサイト数だけ指定する必要があります.
  内容として, 「サイト番号」, 「サイトタイプ」, 「測定タイプ」の3つの整数をスペース区切りで持ちます.
  サイトタイプの詳細は別途アルゴリズム定義ファイルの中で定義します.
  ::

    <S> 3 0 1 </S>
    # サイト番号が3のサイトはサイトタイプが0で, 測定タイプは1である.

Lattice/I
  相互作用情報. Lattice/NumberOfInteractions で指定した相互作用数だけ指定する必要があります.
  内容として, 「相互作用番号」, 「相互作用タイプ」, 「相互作用サイト数」, 「相互作用サイト番号」を指定するために, 
  相互作用サイト数+3個の整数をスペース区切りで持ちます.
  相互作用タイプの詳細 ― たとえば相互作用の大きさ ― は別途アルゴリズム定義ファイルの中で定義します.
  サイト番号の順序は, アルゴリズム定義ファイルの Algorithm/Vertex/InitialConfiguration 要素で用いられるサイトの並び順と整合させる必要があります.
  ::

    <I> 5 1 2 8 12 </I>
    # 相互作用番号が5である相互作用は相互作用タイプが1で, 2つのサイトが関与し, 
    # それらのサイト番号は8と12である.


ハミルトニアンTOML ファイル ``hamiltonian.toml``
***************************************************
ハミルトニアン定義ファイルは局所ハミルトニアン, 例えばボンドハミルトニアン, を指定する,
`TOML`_ 形式で記述されるテキストファイルです.
``dla_alg`` の入力として, アルゴリズム定義ファイルを作成するために用いる補助入力ファイルとなっています.
ハイゼンベルグ模型などのよく用いられる模型については,  
補助ツール ``dla_hamgen`` が用意されています.

``name``
   ハミルトニアンの名前です。シミュレーション中で使われることはありません。

``sites``
   サイトハミルトニアンの情報を記述するテーブルの配列です。

   ``sites.id``
      サイトタイプを示す整数です。

   ``sites.N``
      局所自由度が取りうる状態の数を示す整数です。
      例えば :math:`S=1/2` スピンでは 2 です。

   ``sites.elements``
      サイトハミルトニアンの行列要素を示すテーブルの配列です。

      ``sites.elements.istate``
         ハミルトニアンが作用する前の状態番号です。

      ``sites.elements.fstate``
         ハミルトニアンが作用した後の状態番号です。

      ``sites.elements.value``
         ハミルトニアンの行列要素の値です。

   ``sites.sources``
      ワームを導入するためのソースハミルトニアンの行列要素を示すテーブルの配列です。

      ``sites.sources.istate``
         ハミルトニアンが作用する前の状態番号です。

      ``sites.sources.fstate``
         ハミルトニアンが作用した後の状態番号です。

      ``sites.sources.value``
         ハミルトニアンの行列要素の値です。

``interactions``
   多体相互作用の情報を記述するテーブルの配列です。

   ``interactions.id``
      相互作用タイプを示す整数です。

   ``interactions.nbody``
      相互作用に関与するサイトの数を示す整数です。

   ``interactions.N``
      相互作用に関与するサイトそれぞれで局所自由度が取りうる状態の数です。

   ``interactions.elements``
      相互作用ハミルトニアンの行列要素を記述するテーブルの配列です。

      ``interactions.elements.istate``
         相互作用ハミルトニアンが作用する前のサイトの状態を指定する整数の配列です。

      ``interactions.elements.fstate``
         相互作用ハミルトニアンが作用した後のサイトの状態を指定する整数の配列です。

      ``interactions.elements.value``
         相互作用ハミルトニアンの行列要素の値です。

:math:`S=1/2` 反強磁性ハイゼンベルグ模型の例を示します。 ::

   name = "S=1/2 XXZ model"
   [[sites]]
   id = 0
   N = 2
   [[sites.elements]]
   istate = [ 0,]
   fstate = [ 0,]
   value = 0.5

   [[sites.elements]]
   istate = [ 1,]
   fstate = [ 1,]
   value = -0.5

   [[sites.sources]]
   istate = [ 0,]
   fstate = [ 1,]
   value = 0.5

   [[sites.sources]]
   istate = [ 1,]
   fstate = [ 0,]
   value = 0.5


   [[interactions]]
   id = 0
   nbody = 2
   N = [ 2, 2,]
   [[interactions.elements]]
   istate = [ 0, 0,]
   fstate = [ 0, 0,]
   value = 0.25

   [[interactions.elements]]
   istate = [ 0, 1,]
   fstate = [ 0, 1,]
   value = -0.25

   [[interactions.elements]]
   istate = [ 0, 1,]
   fstate = [ 1, 0,]
   value = 0.5

   [[interactions.elements]]
   istate = [ 1, 0,]
   fstate = [ 1, 0,]
   value = -0.25

   [[interactions.elements]]
   istate = [ 1, 0,]
   fstate = [ 0, 1,]
   value = 0.5

   [[interactions.elements]]
   istate = [ 1, 1,]
   fstate = [ 1, 1,]
   value = 0.25



アルゴリズム定義ファイル ``algorithm.xml``
************************************************

アルゴリズム定義ファイルは相互作用ごとのワームの散乱確率などを定義する,  
XML ライクな形式で記述されるテキストファイルです.
これは一般に複雑になりえるので, より簡単なハミルトニアン定義ファイルから自動生成するためのツール ``dla_alg`` が用意されています.

アルゴリズム定義ファイルはただ一つの要素 Algorithm を持ち, すべての情報は Algorithm 要素の内容として含まれます.

Algorithm
  ファイル全体の要素名.サブ要素として,  General,  Site,  Interaction,  Vertex があります.
  ワームの生成・消滅・散乱の仕方を定義します.

Algorithm/Comment
  省略可能. コメント文を示し, 計算には使用されません.

Algorithm/General
  サブ要素として,  NSType,  NIType,  NVType,  NXMax,  WDiag があります.
  サイトの種類数や相互作用の種類数など, アルゴリズム定義の基本パラメータを設定します.
  ::

    <Algorithm>
      <General>
        <NSType>  1 </NSType>
        <NIType>  1 </NIType>
        <NVType>  2 </NVType>
        <NXMax>   2 </NXMax>
        <WDiag>   0.25 </WDiag>
      </General>
      ...
    </Algorithm>

Algorithm/General/NSType
  異なるサイト型の個数を指定する整数値.

Algorithm/General/NIType
  異なる相互作用型の個数を指定する整数値.

Algorithm/General/NVType
  異なるバーテックス型の個数を指定する整数値.

Algorithm/General/NXMax
  各サイトが取りうる状態の数の最大値.
  例えば大きさ :math:`S` のスピン系ならば :math:`2S+1` .

Algorithm/General/WDiag
  ユーザが改変する measure_specific.cc 以外では用いられないので, その中で使われない場合には指定する必要はありません.
  （標準の measure_specific.cc では, ワームの行程長から相関関数を求めるときの比例係数として用いられています.
  この量に興味がない場合は, 任意の数を指定してください.）

Algorithm/Site
  1つのサイト型を定義します.具体的には, そのサイト型をもつサイトに対する操作を定義します.
  サイトにワームを生成消滅する過程もここで定義します.
  サブ要素として,  SType,  NumberOfStates,  VertexTypeOfSource,  InitialConfiguration があります.
  ::

    <Algorithm>
      ...
      <Site>
        <STYPE> 0 </STYPE>
        <NumberOfStates> 2 </NumberOfStates>
        <VertexTypeOfSource> 0 </VertexTypeOfSource>
        <InitialConfiguration>
           ...
        </InitialConfiguration>
        <InitialConfiguration>
           ...
        </InitialConfiguration>
      </Site>
      ...
    </Algorithm>

Algorithm/Site/SType
  定義されるサイト型の識別番号.

Algorithm/Site/NumberOfStates
  サイトが取りうる状態の数.

Algorithm/Site/VertexTypeOfSource
  挿入される可能性のあるバーテックスのタイプ.

Algorithm/Site/InitialConfiguration
  初期条件の定義. 初期条件ごとのワーム対の生成消滅過程を定義もこの要素のなかで行われます.
  サブ要素として, State, NumberOfChannels, Channel があります.
  ::

    <Algorithm>
      ...
      <Site>
        ...
        <InitialConfiguration>
          <State> 0 </State>
          <NumberOfChannels> 2 </NumberOfChannels>
          <Channel> 0 1 0.5 </Channel>
          <Channel> 1 1 0.5 </Channel>
        </InitialConfiguration>
        ...
      </Site>
      ...
    </Algorithm>

Algorithm/Site/InitialConfiguration/State
  ワーム対が生成される前（もしくは消滅後）のサイトの状態.

Algorithm/Site/InitialConfiguration/NumberOfChannels
  可能性のある終状態（チャネル）の数.

Algorithm/Site/InitialConfiguration/Channel
  各チャネルの定義. 整数値, 整数値, 浮動小数点値の3つの並びで指定.

  - 第1の値はワーム生成後のヘッドの向き（0は虚時間方向負の向き, 1は正の向き.）.
  - 第2の値はワーム生成後のヘッドとテールの間の状態.
  - 第3の値はそのような終状態をとる確率.

  終状態としてワーム対を生成しない場合は, その Channel の 第1と第2の整数値はともに -1とする.

Algorithm/Interaction
  １つの相互作用型を定義します.
  サブ要素として IType, VType, NBody, EBase, VertexDensity, Sign があります.
  ::

    <Algorithm>
      ...
      <Interaction>
        <IType> 0 </IType>
        <VType> 1 </VType>
        <NBody> 2 </NBody>
        <EBase> 0.125 </EBase>
        <VertexDensity> 0 0 0.25 </VertexDensity>
        <VertexDensity> 1 1 0.25 </VertexDensity>
        <Sign> 0 1 1 0 -1.0 </Sign>
        <Sign> 1 0 0 1 -1.0 </Sign>
      </Interaction>
      ...
    </Algorithm>

Algorithm/Interaction/IType
  相互作用の型の識別番号.

Algorithm/Interaction/VType
  挿入する可能性のあるバーテックスの型の識別番号. バーテックス型の内容は Vertex/Algorithm で定義します.

Algorithm/Interaction/NBody
  相互作用に関与するサイトの数（ゼーマン項のような1体相互作用であれば1 で, 交換相互作用のような2体相互作用であれば2. 3以上を指定することも可能）.

Algorithm/Interaction/EBase
  エネルギーオフセットの値. シミュレーション自体には影響しませんが, 最終的なエネルギーの値を出すときに使用されます.

Algorithm/Interaction/VertexDensity
  関与するサイトの状態ごとに挿入するバーテックスの密度を指定します.
  Algorithm/Interaction/NBody 個の整数値と, 1個の浮動小数点値の並びで指定.
  整数値は, 関与する各サイトの状態（順序は格子定義ファイルの I で指定するサイト番号の順序と対応します）.
  浮動小数点値は密度.

Algorithm/Interaction/Sign
  その相互作用における局所重みの符号, すなわち :math:`\textrm{Sgn}(\langle f | -\mathcal{H} | i \rangle)` を指定します.
  :math:`2\times` Algorithm/Interaction/NBody 個の整数値と, 1個の浮動小数点値の並びで指定.
  整数値は, 関与する各サイトのそれぞれについて, 相互作用演算子が適用される前と後の状態で,
  浮動小数点値は重みの符号.

  例えば, ``<Sign> 0 1 1 0 -1.0 </Sign>`` は :math:`\langle 1 0 | \left(-\mathcal{H}\right) | 0 1 \rangle < 0` を意味します.

Algorithm/Vertex
  1つのバーテックスの型を定義します. バーテックスとしては, 通常の2体, 3体, ……の相互作用を記述するもの（ ``VCategory=2`` ）と, 
  ワームヘッドがテールと消滅する場合のテール（ ``VCategory=1`` ）があります.
  Algorithm/Interaction の要素になりえるのは, 前者です.
  （このほか, 時間方向の周期境界（ ``VCategory=0`` ）も1体のバーテックスとして扱っていますが, これをユーザが定義する必要はありません.）
  サブ要素として VType,  VCategory,  NBody,  NumberOfInitialConfigurations,  InitialConfiguration があります.
  ::

    <Algorithm>
      ...
      <Vertex>
        <VTYPE> 0 </VTYPE>
        <VCATEGORY> 1 </VCATEGORY>
        <NBODY> 1 </NBODY>
        <NumberOfInitialConfigurations> 4 </NumberOfInitialConfigurations>
        <InitialConfiguration>
          ...
        </InitialConfiguration>
        ...
        <InitialConfiguration>
          ...
        </InitialConfiguration>
      </Vertex>
      ...
    </Algorithm>

Algorithm/Vertex/VType
  バーテックス型の識別番号. バーテックス型の定義ごとに異なる番号である必要があります.

Algorithm/Vertex/VCategory
  1がワームテール, 2が相互作用.

Algorithm/Vertex/NBody
  相互作用に関与するサイトの個数.
  テールの場合には1.

Algorithm/Vertex/NumberOfInitialConfigurations
  バーテックスの可能な初期状態数.

Algorithm/Vertex/InitialConfiguration
  特定のバーテックス初期状態に対するワームの可能なアクションを定義します.
  従って, この要素は, Algorithm/Vertex/NumberOfInitialConfigurations の値と同じ数だけ存在する必要があります.
  サブ要素として,  State,  IncomingDirection,  NewState,  NumberOfChannels,  Channel があります.
  ::

    <Algorithm>
      ...
      <Vertex>
        ...
        <InitialConfiguration>
          <State>  1 0 0 1 </State>
          <IncomingDirection> 0 </IncomingDirection>
          <NewState> 0 </NewState>
          <NumberOfChannels> 1 </NumberOfChannels>
          <Channel>    3    0       1.0000000000000000 </Channel>
        </InitialConfiguration>
        ...
      </Vertex>
      ...
    </Algorithm>

  この例で定義されているのは, 「バーテックスの左下(0), 左上(1), 右下(2), 右上(3)の脚の状態がそれぞれ1, 0, 0, 1 であって, そこに, 左下（脚0の方向）から, その脚の状態を 0 に変化させるような ワームヘッドが入射した場合」のアクションであり, 
  その内容は,  「確率1で, そのワームヘッドを 脚3の方向に散乱させて,  その方向の足の状態を 0 に変更する」ことを表しています. （つまり, この散乱が起こった場合, 散乱後のバーテックスの状態は 0, 0, 0, 0 になる.）

Algorithm/Vertex/InitialConfiguration/State
  ワームヘッドが入ってくる前のバーテックスの初期状態を指定します.
  具体的にはバーテックスの各脚の状態を指定します.
  足の本数は,  Algorithm/Vertex/NBody で指定される数 (=m) の2倍なので,  2m 個数の整数値をスペースで区切ったものを入力します.
  その順序として, 脚は対応するサイトの順序に並べられ, 
  同じサイトに対応する2本の脚については, 虚数時間の小さい側が先に来ます.
  （サイトの並び順は任意でよいが,  格子定義ファイルの Lattice/I 要素で 指定されているサイトの並び順はここで用いられたサイトの順序と整合している必要があります.）
  各整数はバーテックスの足の状態を示す 0 から n-1 までの値. （ここで, n は対応するサイトの,  Algorithm/Site/NumberOfStates で指定される値.）

Algorithm/Vertex/InitialConfiguration/IncomingDirection
  入射するワームヘッドが入射前に乗っている脚の番号. 
  対応する足が Algorithm/Vertex/InitialConfiguration/State の記述において何番目に出てくるかを 0 から 2m-1 の整数値で指定.

Algorithm/Vertex/InitialConfiguration/NewState
  ワームヘッドが通過したあとの Algorithm/Vertex/InitialConfiguration/IncomingDirection の足の状態. 0 から n-1 の整数値で指定.

Algorithm/Vertex/InitialConfiguration/NumberOfChannels
  可能な散乱チャネルの個数.

Algorithm/Vertex/InitialConfiguration/Channel
  散乱チャネルの定義.
  Algorithm/Vertex/InitialConfiguration/NumberOfChannels の個数だけこの要素を用意する必要があります.
  2つの整数値と1つの浮動小数点値をスペースで区切ったもので指定.

  - 第1の整数値は, 散乱後のワームヘッドが乗っている足の番号を 0 から 2m-1 の値で指定したもの.
  - 第2の整数値は, ワームヘッドが飛び去ったあとのその足の状態を 0 から n-1 の値で指定したもの.
  - 第3の浮動小数点値は, そのチャネルを選ぶ確率.

  特別な場合として, ワームヘッドがテールに衝突して消滅する場合があり, この場合は 第1引数と第2引数に -1 を指定します.

波数データファイル ``kpoints.dat``
*******************************************

波数データファイルは, 波数ベクトル

.. math::
   \vec{k}^{(i)} = \sum_{d=1}^{D} n_d^{(i)} \vec{g}_d

の :math:`\vec{n}^{(i)}` を指定するテキストファイルです。

``dim``
   格子の次元を示す整数です。

``kpoints``
   波数ベクトルを指定するセクションです。

   - 1行目
      - 波数ベクトルの総数。
   - 2行目以降
      - 波数ベクトルを表す、空白区切りで次元の数+1 だけ並べられた数の組。
        最初の整数は波数ベクトルの番号を、残りの数はベクトルの要素 :math:`n_d` を示します。

   
二次元の例を示します. ::

   dim
   2

   kpoints
   3
   0 0 0
   1 2 0
   2 4 0

構造因子定義ファイル ``sf.xml``
************************************************

構造因子定義ファイルは, 動的構造因子

.. math::
    S^{zz}(\vec{k},\tau) \equiv
      \left\langle M^z(\vec{k},\tau)M^z(-\vec{k},0) \right\rangle - \left\langle M^z(\vec{k},\tau)\right\rangle \left\langle M^z(-\vec{k},0)\right\rangle 

を計算するための波数や虚時間刻みの情報がXML ライクな形式で記述されるテキストファイルです.
構造因子定義ファイル作成のための補助ツール ``sfgene`` が用意されています.

格子ファイルはただ一つの要素 StructureFactor を持ち, すべての情報は StructureFactor 要素の内容として含まれます.

StructureFactor
  ファイル全体の要素名.
  サブ要素として, Ntau, NumberOfElements, CutoffOfNtau, NumberOfInverseLattice, SF があります.

StructureFactor/Comment
  省略可能.
  コメント文を示し, 計算には使用されません.

StructureFactor/Ntau
  虚時間軸の分割数.

StructureFactor/CutoffOfNtau
  動的構造因子の虚時間引数 :math:`\tau` の最大値.
  StructureFactor/Ntau 以下の整数で指定します.

StructureFactor/NumberOfInverseLattice
  波数 :math:`\vec{k}` の数.

StructureFactor/NumberOfElements
  波数と座標の組み合わせの総数. 
  StructureFactor/NumberOfInverseLattice と Lattice/NumberOfSites の積.

StructureFactor/SF
  内積 :math:`\vec{r}\cdot\vec{k}` の情報.
  StructureFactor/NumberOfElements で指定した数だけ指定する必要があります.
  内容として,
  「 :math:`\cos(\theta)` の値」,
  「 :math:`\sin(\theta)` の値」,
  「サイト番号」,
  「波数番号」 の4つの数字をスペース区切りで持ちます.
  ここで :math:`\theta` はサイト番号で示されるサイトの座標 :math:`\vec{r}` と波数番号で示される波数 :math:`\vec{k}` との内積です.

実空間表示温度グリーン関数定義ファイル ``cf.xml``
****************************************************

実空間表示温度グリーン関数定義ファイルは,実空間表示温度グリーン関数

.. math::
  G(\vec{r}_{ij},\tau) \equiv \left\langle M_i^+(\tau) M_j^- \right\rangle

を計算するための相対座標 :math:`\vec{r}_{ij}` の情報がXML ライクな形式で記述されるテキストファイルです.
実空間表示温度グリーン関数定義ファイル作成のための補助ツール ``cfgene`` が用意されています.

格子ファイルはただ一つの要素 CorrelationFunction を持ち, すべての情報は CorrelationFunction 要素の内容として含まれます.

CorrelationFunction
  ファイル全体の要素名.サブ要素として, Ntau, NumberOfKinds, CF があります.

CorrelationFunction/Comment
  省略可能.
  コメント文を示し, 計算には使用されません.

CorrelationFunction/Ntau
  虚時間軸の分割数.

CorrelationFunction/NumberOfKinds
  取りうる相対座標の数.

CorrelationFunction/CF
  CorrelationFunction/NumberOfKinds で指定した数だけ指定する必要があります.
  内容として,
  「相対座標のインデックス」, 「サイト i のインデックス」, 「サイト j のインデックス」 の3つの整数をスペース区切りで持ちます.


波数表示温度グリーン関数定義ファイル ``ck.xml``
************************************************

波数表示温度グリーン関数定義ファイルは,波数表示温度グリーン関数

.. math::
  G(\vec{k},\tau) \equiv \left\langle M^+(\vec{k}, \tau) M^-(-\vec{k},0) \right\rangle

を計算するための波数や虚時間刻みの情報がXML ライクな形式で記述されるテキストファイルです.

要素名を含めて, 動的構造因子定義ファイルと全く同じ構造を持つため, 流用が可能です.

.. _TOML: https://github.com/toml-lang/toml/blob/master/versions/ja/toml-v0.5.0.md
