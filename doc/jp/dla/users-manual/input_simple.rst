.. highlight:: none

DLA のシンプルモードファイル
=================================


.. _simple_mode_file:

シンプルモードファイル ``std.toml``
********************************************
シンプルモードファイルは `TOML`_ 形式のテキストファイルです。
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



.. _simple_mode_kpoints:

``kpoints``
+++++++++++++
波数の情報を指定するテーブルです。
``dla_pre`` および ``dla_sfgen`` で使用されます.

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    ksteps, list(int) or int, 1, "波数の増分."


.. _simple_mode_algorithm:

``algorithm``
+++++++++++++++
ワームの散乱確率の計算アルゴリズムなどを指定するテーブルです.
``dla_pre`` で使用されます.

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    パラメータ名, 型, デフォルト値, 説明
    kernel, string, 'suwa todo', "バーテックスにおけるワームの散乱過程について、その遷移確率を計算するために用いる手法。"


``kernel`` として指定できる手法は次の通り。

``suwa todo``
   詳細釣り合いを破る諏訪・藤堂アルゴリズム.
   (H. Suwa and S. Todo, PRL 105, 120603 (2010))

``reversible suwa todo``
   詳細釣り合いを満たす諏訪・藤堂アルゴリズム. (arXiv:1106.3562)

``heat bath``
   熱浴法.

``metropolice``
   メトロポリスアルゴリズム.


.. _TOML: https://github.com/toml-lang/toml/blob/master/versions/ja/toml-v0.5.0.md
