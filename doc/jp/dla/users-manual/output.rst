.. -*- coding: utf-8 -*-
.. highlight:: none

DLA の出力ファイル
==============================

フォーマット
*****************************
DLA は計算結果を行区切りのプレーンテキストファイルで出力します.
行頭の文字はその行の意味を表します.

``P <name> = <value>``
  入力パラメータファイルと格子ファイルから読み取ったパラメータ.
``R <name> = <mean> <error>``
  計算で求められた物理量. ``<mean>`` は平均値を,  ``<error>`` は標準誤差を示します.
``I <text> = <value>``
  その他計算中に得られた情報.
``C <text>``
  コメント.

以下にサンプル（反強磁性ハイゼンベルグ鎖）を示します.
::
  
  C This is DSQSS ver.v2.0.0

  I N_PROC = 1
  P D       =            1
  P L       =            8
  P BETA    =      10.0000000000000000
  P NSET    =           10
  P NPRE    =         1000
  P NTHERM  =         1000
  P NDECOR  =         1000
  P NMCS    =         1000
  P SEED    =    198212240
  P NSEGMAX =        10000
  P NVERMAX =        10000
  P BETA    = 10.000000000000
  P NTAU    =           10
  P NCYC    =            7
  P ALGFILE = algorithm.xml
  P LATFILE = lattice.xml
  P WVFILE = wv.xml
  P DISPFILE  = disp.xml
  P OUTFILE    = sample.log
  P CFOUTFILE  = cf.dat
  P SFOUTFILE  = sf.dat
  P CKOUTFILE  = ck.dat
  P SIMULATIONTIME   =     0.000000
  R sign = 1.00000000e+00 0.00000000e+00
  R anv = 3.03805000e+00 1.25395375e-02
  R ene = -4.55991910e-01 1.20267537e-03
  R spe = -1.76672204e-02 4.09064489e-02
  R som = -1.76672204e-01 4.09064489e-01
  R len = 1.20014021e+01 4.78403202e-02
  R xmx = 3.00035053e-01 1.19600800e-03
  R amzu = -2.00000000e-04 1.08972474e-04
  R bmzu = -2.00000000e-04 1.08972474e-04
  R smzu = 1.32382500e-03 1.40792745e-04
  R xmzu = 1.32382500e-02 1.40792745e-03
  R ds1 = -1.32954309e-03 7.87178338e-04
  R wi2 = 2.31040000e+01 3.83762890e-01
  R rhos = 1.44400000e-01 2.39851806e-03
  R rhof = inf nan
  R comp = 2.43165481e+35 1.71412709e+35
  R amzs0 = -2.00000000e-04 1.08972474e-04
  R bmzs0 = 1.65625000e-04 1.76161818e-05
  R smzs0 = 1.32382500e-03 1.40792745e-04
  R xmzs0 = 1.32382500e-02 1.40792745e-03
  R amzs1 = -9.25000000e-04 4.02247160e-03
  R bmzs1 = 1.09209375e-01 1.12051866e-03
  R smzs1 = 8.72503175e-01 8.93939492e-03
  R xmzs1 = 3.00500011e+00 2.99056535e-02
  R time = 1.03679300e-07 1.22794234e-09
  I [the maximum number of segments]          = 123
  I [the maximum number of vertices]          = 66
  I [the maximum number of reg. vertex info.] = 3


以下の物理量定義に現れる記号の意味を示します.

:math:`N_s`
  サイト数.

:math:`Q(\vec{k})`
  格子点 :math:`i` 上で定義される任意の演算子 :math:`Q_i` のフーリエ変換.

  :math:`\displaystyle Q(\vec{k}) \equiv \frac{1}{\sqrt{N_s}} \sum_i^{N_s} Q_i e^{-i\vec{r}_i\cdot\vec{k}}`

:math:`Q(\tau)`
  虚時間 :math:`\tau` における演算子.

  :math:`\displaystyle Q(\tau) \equiv \exp\left[\tau \mathcal{H}\right] Q(\tau=0) \exp\left[-\tau \mathcal{H}\right]`

:math:`\tilde{Q}`
  任意の演算子 :math:`Q` について, 虚時間方向の平均 :math:`\displaystyle \frac{1}{\beta}\int_0^\beta \! \mathrm{d} \tau Q(\tau)`

:math:`M^z`
  局所自由度の量子化軸方向成分.
  たとえばスピン系では局在スピン演算子の :math:`z` 成分 :math:`S^z` で, 
  ボース粒子系では数演算子 :math:`n` です.

:math:`M^\pm`
  :math:`M^z` の昇降演算子.
  スピン系では :math:`M^{\pm} \equiv S^\pm` , 
  ボース粒子系では生成消滅演算子 :math:`M^+ \equiv b^\dagger` および :math:`M^- \equiv b` .

:math:`M^x`
  非対角秩序変数.
  スピン系では :math:`M^x \equiv (S^+ + S^-)/2` , 
  ボース粒子系では :math:`M^x \equiv (b + b^\dagger)` .

:math:`T`
  温度.

:math:`\beta`
  逆温度.

:math:`E_0`
  非摂動ハミルトニアンの期待値の虚時間平均
  :math:`\displaystyle \frac{1}{\beta}\int d\tau \langle \phi(\tau)|\mathcal{H}_0|\phi(\tau)\rangle`.

:math:`N_v`
  バーテックスの数, すなわち摂動の次数.

:math:`h`
  :math:`M^z` に共役な外場.
  スピン系では縦磁場, ボース粒子系では化学ポテンシャル.

:math:`\left\langle Q \right\rangle`
  任意の演算子 :math:`Q` のグランドカノニカル平均.

メイン出力
*****************
メイン出力ファイルは, 入力パラメータファイルの ``outfile`` キーワードで指定した名前で出力されます.

``sign``
  重みの符号.

  :math:`\frac{\sum_i W_i }{ \sum_i |W_i| }`, ここで :math:`i` はモンテカルロサンプルの番号.

``anv``
  サイトあたりの平均バーテックス数.

  :math:`\displaystyle \frac{\langle N_v \rangle}{N_s}`
``ene``
  エネルギー密度.

  :math:`\displaystyle \epsilon \equiv \frac{1}{N_s}\left(\langle E_0 \rangle - T\langle N_v\rangle\right)`
``spe``
  比熱.

  :math:`\displaystyle C_V \equiv \frac{\partial \epsilon}{\partial T} = \frac{1}{N_s T^2} \left[\left\langle\left(E_0 - TN_v\right)^2\right\rangle - \left\langle\left(E_0 - TN_v\right)\right\rangle - T^2\left\langle N_v \right\rangle\right]`

  NOTICE: 量子モンテカルロ法において, 比熱の計算は他の物理量と比べて精度が悪く, サンプル数 :math:`N` (``nmcs``)に対して :math:`1/N` の系統誤差も現れます.
  特に, エネルギーギャップ以下の極低温領域など, 比熱の値が非常に小さくなるような場合には, 計算結果が負になることがあります.

``som``
  比熱と温度の比. 

  :math:`\displaystyle \gamma \equiv \frac{C_V}{T} = \beta C_V`

``len``
  平均ワーム長さ.

``xmx``
  横感受率.

``amzu``
  「磁化」(uniform, :math:`\tau=0`).

  :math:`\displaystyle m^z \equiv \frac{1}{N_s} \sum_i^{N_s} M^z_i` としたときの
  :math:`\left\langle m^z \right\rangle` .

``bmzu``
  「磁化」(uniform, :math:`\tau` 平均). :math:`\left\langle \tilde{m}^z \right\rangle` .

``smzu``
  構造因子(uniform).

  :math:`\displaystyle S^{zz}(\vec{k}=0) \equiv \frac{1}{N_s}
  \sum_{i, j} e^{i \vec{k}\cdot(\vec{r}_i-\vec{r}_j)} \left[
  \left\langle M^z_i M^z_j\right\rangle
  - \left\langle M_i^z \right\rangle \left\langle M_j^z \right\rangle 
  \right] \Bigg|_{\vec{k}=0}
  =
  N_s \left[ \left\langle (m^z)^2\right\rangle
  - \left\langle m^z\right\rangle^2 \right]`

``xmzu``
  縦感受率(uniform).

  :math:`\displaystyle \chi^{zz}(\vec{k}=0,  \omega=0) \equiv
  \frac{\partial \left\langle \tilde{m}^z \right\rangle}{\partial h} =
  \beta N_s\left[ \left\langle \left(\tilde{m}^z\right)^2\right\rangle
  - \left\langle \tilde{m}^z\right\rangle^2 \right]`

``amzsK``
  「磁化」("staggered", :math:`\tau=0`)

  :math:`\displaystyle m_K^z \equiv \frac{1}{N_s} \sum_i^{N_s} M_i^z \cos\left( \vec{k}\cdot\vec{r_i} \right)` 
  としたときの :math:`\left\langle m_s^z \right\rangle` .
  :math:`K` は波数ベクトルXMLファイルで定義された波数 :math:`k` のインデックス.

``bmzuK``
  「磁化」("staggered", :math:`\tau` 平均). :math:`\left\langle \tilde{m}_K^z \right\rangle` .

``smzsK``
  構造因子 ("staggered").

  :math:`\displaystyle S^{zz}(\vec{k}) = N_s \left[ \left\langle (m_K^z)^2 \right\rangle - \left\langle m_K^z \right\rangle^2 \right]`

``xmzsK``
  縦感受率 ("staggered").

  :math:`\displaystyle \chi^{zz}(\vec{k},  \omega=0) 
  = \beta N_s \left[\left\langle (\tilde{m}_K^z)^2 \right\rangle - \left\langle \tilde{m}_K^z \right\rangle^2 \right]`

``wi2``
  ワインディングナンバー.

  :math:`\displaystyle W^2 = \sum_{d=1}^D L_d^2 \left\langle W_d^2 \right\rangle`

``rhos``
  超流動密度.

  :math:`\displaystyle \rho_s = \frac{W^2}{2DV\beta}`


``rhof``
  超流動比 (superfluid fraction).

  :math:`\displaystyle \frac{\rho_s}{\left\langle m^z \right\rangle}`


``comp``
  圧縮率 (compressibility).

  :math:`\displaystyle \frac{\chi^{zz}(\vec{k}=0,\omega=0)} {\left\langle \tilde{m}^z \right\rangle^2}`


``time``
  モンテカルロスイープあたりの計算時間（秒）.


構造因子出力ファイル
****************************************
構造因子出力ファイルは, 入力パラメータファイルの ``sfoutfile`` キーワードで指定した名前で出力されます.
このファイルには虚時間構造因子

.. math::
  S^{zz}(\vec{k}, \tau) \equiv
  \left\langle M^z(\vec{k},  \tau) M^z(-\vec{k},  0) \right\rangle
  - \left\langle M^z(\vec{k},  \tau)\right\rangle \left\langle M^z(-\vec{k},  0)\right\rangle

が出力されます. 
波数 :math:`\vec{k}` や虚時間 :math:`\tau` の値は, 物理量名を用いて
::

  R C0t0 = 1.32500000e-03 1.40929454e-04
  R C0t1 = 1.32500000e-03 1.40929454e-04
  R C1t0 = 7.35281032e-02 3.18028565e-04

のように ``C<k>t<t>`` という形で区別されます.
ここで ``<k>`` は波数ベクトル XMLファイルの ``kindex`` (``KR`` タグの最終要素) で指定される波数のインデックスで, 
``<t>`` は離散化した虚時間のインデックス.

実空間表示温度グリーン関数出力ファイル
****************************************
実空間表示温度グリーン関数出力ファイルは, 入力パラメータファイルの ``cfoutfile`` キーワードで指定した名前で出力されます.
このファイルには温度グリーン関数

.. math::
  G(\vec{r}_{ij}, \tau) \equiv \left\langle M_i^+(\tau) M_j^- \right\rangle

が出力されます. 変位 :math:`\vec{r}_{ij}` や虚時間 :math:`\tau` の値は構造因子と同様に, 
``C<k>t<t>`` という形で物理量名によって区別されます.
ここで ``<k>`` は変位XML ファイルの ``kind`` (``R`` タグの第一要素) で指定される変位のインデックスで, 
``<t>`` は離散化した虚時間のインデックス.

NOTICE:
現在のバージョンでは :math:`S=1/2` のモデル以外はうまく計算できません.

波数表示温度グリーン関数出力ファイル
****************************************
波数表示温度グリーン関数出力ファイルは, 入力パラメータファイルの ``ckoutfile`` キーワードで指定した名前で出力されます.
このファイルには温度グリーン関数

.. math::
  G(\vec{k}, \tau) \equiv \left\langle M^+(\vec{k},  \tau) M^-(-\vec{k}, 0) \right\rangle

が出力されます.
波数 :math:`\vec{k}` や虚時間 :math:`\tau` の値は構造因子と同様に, 
``C<k>t<t>`` という形で物理量名によって区別されます.
ここで ``<k>`` は波数ベクトルXMLファイルの ``kindex`` (``RK`` タグの最終要素) で指定される波数のインデックスで, 
``<t>`` は離散化した虚時間のインデックス.

NOTICE:
現在のバージョンでは :math:`S=1/2` のモデル以外はうまく計算できません.
