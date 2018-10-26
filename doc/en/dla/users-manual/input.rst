.. highlight:: none

DLA の入力ファイル
========================

入力ファイル一覧
************************

``qmc.inp``
  モンテカルロの繰り返し回数など、計算制御のためのパラメータファイル。
``lattice.xml``
  格子の定義ファイル。
``algorithm.xml``
  アルゴリズム（ワームの散乱確率など）の定義ファイル。
``sf.xml``
  構造因子の波数定義ファイル。
``cf.xml``
  実空間温度グリーン関数の座標定義ファイル。

パラメータファイル ``qmc.inp``
**********************************
パラメータファイルは次に示すような書式のプレーンテキストファイルである。

- 1行あたり1パラメータを、 ``<name> = <value>`` という形式で表す。
- 1行の文字数は最大256。
- ファイル名以外は大文字小文字を区別しない。
- 空行、空白は無視される。
- "#" から行末まではコメントとして無視される。

パラメータのリストと意味を以下に示す。 
名前のあとのカッコはデフォルト値。

``nmcse`` (``1000``)
  モンテカルロスイープあたりのワーム対生成回数を求めるための事前計算のためのモンテカルロ試行回数。

``nmcsd`` (``1000``)
  熱平衡化のためのモンテカルロスイープ数。

``nmcs`` (``1000``)
  物理量計算のためのモンテカルロスイープ数。

``nset`` (``10``)
  モンテカルロ計算の繰り返し数。

``simulationtime`` (``0.0``)
  計算時間（単位は秒）。

  - ``simulationtime > 0.0`` のとき

    - 指定秒数が経過するか、計算が完了したとき、途中経過をチェックポイントファイルに書き出した後、プログラムを終了する。
    - 計算開始時にチェックポイントファイルがある場合、そのファイルを読み込んだ後に計算を再開する。
    - チェックポイントファイルの名前は ``outfile`` で指定されるメイン出力ファイル名の末尾に ``.cjob`` をつけたもの。

  - ``simulationtime <= 0.0`` のとき

    - チェックポイントファイルは無視され、書き出しも読み込みも行われない。

``seed`` (``198212240``)
  疑似乱数の種。

``nvermax`` (``10000``)
  最大バーテックス数。

``nsegmax`` (``10000``)
  最大セグメント数。

``algfile`` (``algorithm.xml``)
  アルゴリズム定義ファイル名。

``latfile`` (``lattice.xml``)
  格子定義ファイル名。

``sfinpfile`` (``""``)
  構造因子定義ファイル名。空文字列、もしくは指定しない場合、構造因子は計算されない。

``cfinpfile`` (``""``)
  実空間表示温度グリーン関数定義ファイル名。空文字列、もしくは指定しない場合、実空間表示温度グリーン関数は計算されない。

``ckinpfile`` (``""``)
  波数空間表示温度グリーン関数定義ファイル名。空文字列、もしくは指定しない場合、波数空間表示温度グリーン関数は計算されない。

``outfile`` (``sample.log``)
  メイン出力ファイル名。

``sfoutfile`` (``sf.dat``)
  構造因子出力ファイル名。

``cfoutfile`` (``cf.dat``)
  実空間表示温度グリーン関数出力ファイル名。

``ckoutfile`` (``ck.dat``)
  波数空間表示温度グリーン関数出力ファイル名。

``runtype`` (``0``)
  計算手法。
  互換性および将来の拡張のために用意されている。


格子定義ファイル ``lattice.xml``
**************************************

格子ファイルは時空間の情報、たとえばサイトの数やサイト同士のつながりかた、逆温度の大きさなどを定義するための、
XML ライクな形式で記述されるテキストファイルである。
これは一般に複雑になりえるので、正方格子などのよく使われる格子については、
格子定義ファイル作成のための補助ツール ``lattgene`` が用意されている。

格子ファイルはただ一つの要素 ``Lattice`` を持ち、すべての情報は ``Lattice`` 要素の内容として含まれる。

``Lattice``
  ファイル全体の要素。ほかのすべての要素は ``Lattice`` のサブ要素となる。

``Comment``
  省略可能。コメント文を示し、計算には使用されない。

``Dimension``
  格子の次元。

``Beta``
  逆温度。

``LinearSize``
  ユニットセルを単位とした、各次元の格子の長さ。
  内容として、スペース区切りの正整数を ``Dimension`` で指定した数だけ並べたものを取る。 ::

    <LinearSize> 3 4 </LinearSize>
    # ユニットセルが第1次元方向に3個、第2次元方向に4個並んでいる場合

``NumberOfCells``
  ユニットセルの総数。 ``LinearSize`` で指定した各次元方向のサイズの積。

``NumberOfSites``
  サイトの総数。ユニットセルの総数と1セル内のサイト数の積。

``NumberOfInteractions``
  相互作用項の総数。二体相互作用のみの場合は、いわゆる「ボンド数」。

``NumberOfSiteTypes``
  サイトの種類数。

``NumberOfInteractionTypes``
  相互作用の種類数。

``BondDimension``
  Winding number を測定する際に定義する要素。

``NumberOfEdgeInteractions``
  Winding number を測定する際に定義する要素。格子の周期的境界をまたぐボンドの総数を指定する。

``S``
  サイト情報。 ``NumberOfSites`` で指定したサイト数だけ指定する必要がある。
  内容として、「サイト番号」、「サイトタイプ」、「測定タイプ」の3つの整数をスペース区切りで持つ。
  サイトタイプの詳細はアルゴリズム定義ファイルの中で定義される。 ::

    <S> 3 0 1 </S>
    # サイト番号が3のサイトはサイトタイプが0で、測定タイプは1である。

``I``
  相互作用情報。 ``NumberOfInteractions`` で指定した相互作用数だけ指定する必要がある。
  内容として、「相互作用番号」、「相互作用タイプ」、「相互作用サイト数」、「相互作用サイト番号」を指定するために、
  相互作用サイト数+3個の整数をスペース区切りで持つ。
  相互作用タイプの詳細 ― たとえば相互作用の大きさ ― はアルゴリズム定義ファイルの中で定義される。
  サイト番号の順序は、アルゴリズム定義ファイルの ``InitialConfiguration | Vertex`` 要素で用いられるサイトの並び順と整合させる必要がある。 ::

    <I> 5 1 2 8 12 </I>
    # 相互作用番号が5である相互作用は相互作用タイプが1で、2つのサイトが関与し、
    # それらのサイト番号は8と12である。


アルゴリズム定義ファイル ``algorithm.xml``
************************************************

アルゴリズム定義ファイルは相互作用ごとのワームの散乱確率などを定義する、
XML ライクな形式で記述されるテキストファイルである。
これは一般に複雑になりえるので、ハイゼンベルグ模型などのよく用いられる模型については、
アルゴリズム定義ファイル作成のための補助ツール ``hamgen`` や ``dla_alg`` が用意されている。

格子ファイルはただ一つの要素 ``Lattice`` を持ち、すべての情報は ``Lattice`` 要素の内容として含まれる。

``Algorithm``
  ファイル全体の要素名。サブ要素として、 ``General``, ``Site``, ``Interaction``, ``Vertex`` がある。
  ワームの生成・消滅・散乱の仕方を定義する。

``Comment``
  省略可能。コメント文を示し、計算には使用されない。

``General /Algorithm``
  サブ要素として、 ``NSType``, ``NIType``, ``NVType``, ``NXMax``, ``WDiag`` がある。
  サイトの種類数や相互作用の 種類数などアルゴリズム定義の基本パラメータを設定する。 ::

    <Algorithm>
      <General>
        <NSType>  1 </NSType>
        <NIType>  1 </NIType>
        <NVType>  2 </NVType>
        <NXMax>   2 </NXMax>
        <WDiag>        0.2500000000000000 </WDiag>
      </General>
      ...
    </Algorithm>

``NSType /General/Algorithm``
  異なるサイト型の個数を指定する整数値。

``NIType /General/Algorithm``
  異なる相互作用型の個数を指定する整数値。

``NVType /General/Algorithm``
  異なるバーテックス型の個数を指定する整数値。

``NXMax /General/Algorithm``
  各サイトが取りうる状態の数の最大値。例えば大きさ :math:`S` のスピン系ならば :math:`2S+1` 。

``WDiag /General/Algorithm``
  ユーザが改変する measure_specific.cc 以外では用いられないので、その中で使われない場合には指定する必要はない。
  （標準の measure_specific.cc では、ワームの行程長から相関関数を求めるときの比例係数として用いられている。
  この量に興味がない場合は、任意の数を指定しておけばよい。）

``Site /Algorithm``
  1つのサイト型を定義する。具体的には、そのサイト型をもつサイトに対する操作を定義する。 
  サイトにワームを生成消滅する過程もここで定義される。
  サブ要素として、 ``SType``, ``NumberOfStates``, ``VertexTypeOfSource``, ``InitialConfiguration`` がある。 ::

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

``SType /Site/Algorithm``
  定義されるサイト型の識別番号。

``NumberOfStates /Site/Algorithm``
  サイトが取りうる状態の数。

``VertexTypeOfSource / Site/Algorithm``
  挿入される可能性のあるバーテックスのタイプ。

``InitialConfiguration / Site/Algorithm``
  初期条件の定義。 初期条件ごとのワーム対の生成消滅過程を定義もこの要素のなかで行われる。
  サブ要素として、 ``State``, ``NumberOfChannels``, ``Channel`` がある。 ::

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

``State / InitialConfiguration/Site/Algorithm``
  ワーム対が生成される前（もしくは消滅後）のサイトの状態。

``NumberOfChannels /InitialConfiguration/Site/Algorithm``
  可能性のある終状態（チャネル）の数。

``Channel /InitialConfiguration/Site/Algorithm``
  各チャネルの定義。 整数値、整数値、浮動小数点値の3つの並びで指定。

  - 第1の値はワーム生成後のヘッドの向き（0は虚時間方向負の向き、1は正の向き。）。
  - 第2の値はワーム生成後のヘッドとテールの間の状態。
  - 第3の値はそのような終状態をとる確率。

  終状態としてワーム対を生成しない場合は、その Channel の 第1と第2の整数値はともに -1とする。

``Interaction /Algorithm``
  １つの相互作用型を定義する。
  サブ要素として ``IType``, ``VType``, ``NBody``, ``EBase``, ``VertexDensity`` がある。 ::

    <Algorithm>
      ...
      <Interaction>
        <IType> 0 </IType>
        <VType> 1 </VType>
        <NBody> 2 </NBody>
        <EBase> 0.125 </EBase>
        <VertexDensity> 0 0 0.25 </VertexDensity>
        <VertexDensity> 1 1 0.25 </VertexDensity>
      </Interaction>
      ...
    </Algorithm>

``IType /Interaction/Algorithm``
  相互作用の型の識別番号。

``VType /Interaction/Algorithm``
  挿入する可能性のあるバーテックスの型の識別番号。 バーテックス型の内容は ``Vertex/Algorithm`` で定義される。

``NBody /Interaction/Algorithm``
  相互作用に関与するサイトの数（ゼーマン項のような1体相互作用であれば1。交換相互作用のような2体相互作用であれば2。3以上を指定することも可能）。

``EBase /Interaction/Algorithm``
  エネルギーオフセットの値。シミュレーション自体には影響しないが、最終的なエネルギーの値を出すときに使用される。

``VertexDensity /Interaction/Algorithm``
  関与するサイトの状態ごとに挿入するバーテックスの密度を指定する。
  ``NBody`` 個の整数値と、1個の浮動小数点値の並びで指定。
  整数値は、関与する各サイトの状態（順序は格子定義ファイルの ``I`` で指定するサイト番号の順序と対応する）。
  浮動小数点値は密度。

``Vertex /Algorithm``
  １つのバーテックスの型を定義する。 バーテックスとしては、通常の2体、3体、。。。の相互作用を記述するもの（ ``VCategory=2`` ）と、
  ワームヘッドがテールと消滅する場合のテール（ ``VCategory=1`` ）がある。
  ``Interaction`` の要素になりえるのは、前者である。
  （このほか、時間方向の周期境界（ ``VCategory=0`` ）も１体の バーテックスとして扱っているがユーザが定義する必要はない。）
  サブ要素として ``VType``, ``VCategory``, ``NBody``, ``NumberOfInitialConfigurations``, ``InitialConfiguration`` がある。 ::

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

``VType /Vertex/Algorithm``
  バーテックス型の識別番号。 バーテックス型の定義ごとに異なる番号でなければならない。

``VCategory /Vertex/Algorithm``
  1 ... ワームテール。2 ... 相互作用。

``NBody /Vertex/Algorithm``
  相互作用に関与するサイトの個数。
  テールの場合には1。

``NumberOfInitialConfigurations /Vertex/Algorithm``
  バーテックスの可能な初期状態数。

``InitialConfiguration /Vertex/Algorithm``
  特定のバーテックス初期状態に対するワームの可能なアクションを定義する。
  従って、この要素は、NumberOfInitialConfigurations の値と同じ数だけ存在する必要がある。
  サブ要素として、 ``State``, ``IncomingDirection``, ``NewState``, ``NumberOfChannels``, ``Channel`` がある。 ::

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

  この例で定義されているのは、「バーテックスの左下(0)、左上(1)、右下(2)、右上(3)の脚の状態がそれぞれ1,0,0,1 であって、そこに、左下（脚0の方向）から、その脚の状態を 0 に変化させるような ワームヘッドが入射した場合」のアクションであり、
  その内容は、 「確率1で、そのワームヘッドを 脚3の方向に散乱させて、 その方向の足の状態を 0 に変更する」ことを表している。 （つまり、この散乱が起こった場合、散乱後のバーテックスの状態は 0,0,0,0 になる。）

``State /InitialConfiguration/Vertex/Algorithm``
  ワームヘッドが入ってくる前のバーテックスの初期状態を指定する。
  具体的にはバーテックスの各脚の状態を指定する。
  足の本数は、 ``NBody /Vertex`` で指定される数 (=m) の2倍なので、 2m 個数の整数値をスペースで区切ったものになる。
  その際、脚は対応するサイトの順序に並べる。
  同じサイトに対応する2本の脚については、虚数時間の小さい側を先にする。
  （サイトの並び順は任意でよいが、 格子定義ファイルの ``I`` 要素で 指定されているサイトの並び順はここで用いられた サイトの順序と整合していなければならない。）
  各整数はバーテックスの足の状態を示す 0 から n-1 までの値。 （ここで、n は対応するサイトの、 ``NumberOfStates/Site`` で指定される値。）

``IncomingDirection /InitialConfiguration/Vertex/Algorithm``
  入射するワームヘッドが入射前に乗っている脚の番号。 
  対応する足が上記 ``State`` の記述において何番目に出てくるかを 0 から 2m-1 の整数値で指定。

``NewState /InitialConfiguration/Vertex/Algorithm``
  ワームヘッドが通過したあとの ``IncomingDirection`` の足の状態。 0 から n-1 の整数値で指定。

``NumberOfChannels /InitialConfiguration/Vertex/Algorithm``
  可能な散乱チャネルの個数。

``Channel /InitialConfiguration/Vertex/Algorithm``
  散乱チャネルの定義。
  ``NumberOfChannels`` の個数だけこの要素が存在しなければならない。
  2つの整数値と1つの浮動小数点値をスペースで区切ったもので指定。

  - 第1の整数値は、散乱後のワームヘッドが乗っている足の番号を 0 から 2m-1 の値で指定したもの。
  - 第2の整数値は、ワームヘッドが飛び去ったあとのその足の状態を 0 から n-1 の値で指定したもの。
  - 第3の浮動小数点値は、そのチャネルを選ぶ確率。

  特別な場合として、ワームヘッドがテールに衝突して消滅する場合があり、この場合は 第1引数と第2引数に -1 を指定する。
