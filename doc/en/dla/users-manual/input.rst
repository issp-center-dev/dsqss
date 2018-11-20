.. highlight:: none

.. _sec_dla_input:

Input files for DSQSS/DLA
=========================

The list of input files
************************

.. csv-table::
    :header-rows: 0
    :widths: 1,4

    qmc.inp, "Parameter list for the simulation, e.g., the number of Monte Carlo sets."
    lattice.xml, "Definition of the lattice."
    algorithm.xml, "Definition of the algorithm (e.g., scattering rate of a worm)."
    sf.xml, "Indication of wave vectors for structure factors."
    cf.xml, "Indexing directions between all the sites."

Parameter file ``qmc.inp``
**********************************
The parameter file is a plain-text file with the following format,

- One line stands for one parameter by the key-value style, ``<name> = <value>``.
- Cases are insensitive except for file names.
- White spaces and blank lines are ignored.
- Parts between "#" symbol and the linebreak are ignored as comments.

The list of parameters are the following,

.. csv-table::
    :header-rows: 1
    :widths: 1,1,1,4

    name, type, default, description
    beta, double, --, "Inverse temperature. Overwrite a setting in lattice.xml."
    npre, int, 1000, "The number of Monte Carlo steps in the pre calculation phase where the number of creation trials of a pair of worms in one Monte Carlo sweep is defined."
    ntherm, int, 1000, "The number of Monte Carlo sweeps to thermalize the system."
    ndecor, int, 1000, "The number of Monte Carlo sweeps to reduce autocarrelation time between two preceding sets."
    nmcs, int, 1000, "The number of Monte Carlo sweeps to calculate mean values of observables."
    nset, int, 10, "The number of Monte Carlo sets."
    simulationtime, int,  0.0, "Simulation time in second."
    seed, int, 198212240, "The seed of the random number generator."
    nvermax, int,  10000, "The maximum number of vertices."
    nsegmax, int,  10000, "The maximum number of world-line segments."
    algfile, int,  algorithm.xml, "The filename of an algorithm file."
    latfile, string, lattice.xml, "The filename of a lattice file."
    sfinpfile, string, --,  "A structure factor file. If it is an empty string, structure factors will not be calculated."
    cfinpfile, string,  --, "A real space temperature Green's function file. If it is an empty string, real space temperature Green's functions will not be calculated."
    ckinpfile, string,  --, "A momentum space temperature Green's function file. If it is an empty string, momentum space temperature Green's functions will not be calculated."
    outfile, string, sample.log, "The name of the main result file."
    sfoutfile, string, sf.dat, "The name of the structure factor result file."
    cfoutfile, string, cf.dat, "The name of the real space temperature Green's function output file."
    ckoutfile, string, ck.dat, "The name of the momentum space temperature Green's function output file."
    runtype, int, 0, "Method. This remains for backward compatibility."

- About simulationtime

  - When simulationtime > 0.0

    - After XXX, DSQSS/DLA saves jkkjjkkjj

    - 指定秒数が経過するか, 計算が完了したとき, 途中経過をチェックポイントファイルに書き出した後, プログラムを終了します.
    - 計算開始時にチェックポイントファイルがある場合, そのファイルを読み込んだ後に計算を再開します.
    - チェックポイントファイルの名前は outfile で指定されるメイン出力ファイル名の末尾に .cjob をつけたものです.

  - simulationtime <= 0.0 のとき

    - チェックポイントファイルは無視され, 書き出しも読み込みも行われません.


格子定義ファイル ``lattice.xml``
**************************************

格子ファイルは時空間の情報, たとえばサイトの数やサイト同士のつながりかた, 逆温度の大きさなどを定義するための, 
XML ライクな形式で記述されるテキストファイルです.
これは一般に複雑になりえるので, 正方格子などのよく使われる格子については, 
格子定義ファイル作成のための補助ツール ``lattgene`` が用意されています.

格子ファイルはただ一つの要素 Lattice を持ち, すべての情報は Lattice 要素の内容として含まれます.

Lattice
  ファイル全体の要素.
  ほかのすべての要素は Lattice のサブ要素です.

Lattice/Comment
  省略可能. コメント文を示し, 計算には使用されません.

Lattice/Dimension
  格子の次元.

Lattice/Beta
  逆温度. パラメータファイルでの指定が優先されます.

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


アルゴリズム定義ファイル ``algorithm.xml``
************************************************

アルゴリズム定義ファイルは相互作用ごとのワームの散乱確率などを定義する,  
XML ライクな形式で記述されるテキストファイルです.
これは一般に複雑になりえるので,  ハイゼンベルグ模型などのよく用いられる模型については,  
アルゴリズム定義ファイル作成のための補助ツール ``hamgen_H``, ``hamgen_B`` や ``dla_alg`` が用意されています.

格子ファイルはただ一つの要素 Algorithm を持ち, すべての情報は Algorithm 要素の内容として含まれます.

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
  サブ要素として IType, VType, NBody, EBase, VertexDensity があります.
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
      </Interaction>
      ...
    </Algorithm>

Algorithm/Interaction/IType
  相互作用の型の識別番号.

Algorithm/Interaction/VType
  挿入する可能性のあるバーテックスの型の識別番号. バーテックス型の内容は Vertex/Algorithm で定義します.

Algorihtn/Interaction/NBody
  相互作用に関与するサイトの数（ゼーマン項のような1体相互作用であれば1 で, 交換相互作用のような2体相互作用であれば2. 3以上を指定することも可能）.

Algorithm/Interaction/EBase
  エネルギーオフセットの値. シミュレーション自体には影響しませんが, 最終的なエネルギーの値を出すときに使用されます.

Algorithm/Interaction/VertexDensity
  関与するサイトの状態ごとに挿入するバーテックスの密度を指定します.
  Algorithm/Interaction/NBody 個の整数値と, 1個の浮動小数点値の並びで指定.
  整数値は, 関与する各サイトの状態（順序は格子定義ファイルの I で指定するサイト番号の順序と対応します）.
  浮動小数点値は密度.

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
