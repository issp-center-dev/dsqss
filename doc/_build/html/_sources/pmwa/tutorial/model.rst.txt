.. -*- coding: utf-8 -*-
.. highlight:: none

PMWAで扱える模型
==============================
PMWAではハードコアボソン系(サイトに最大1つのボソン)とS=1/2のXXZ模型について計算可能である。ハードコアボソン系のハミルトニアンは

.. math::
   {\cal H} &= -t_{b} \sum_{\langle i, j\rangle}b_i^{\dagger} b_j + U_{BB}\sum_i n_i(n_i -1)
   +V_{B1}\sum_{\langle i, j\rangle} n_i n_j + V_{B2}\sum_{\langle\langle i, j\rangle\rangle} n_i n_j\\ 
   &+\mu\sum_i n_i-\Gamma\sum_i(b_i^{\dagger}+b_i),

で与えられる。ここで、 :math:`\langle i,j \rangle` は最近接のペア、 :math:`\langle\langle i,j \rangle\rangle` は次次近接のペアをそれぞれ表す。
S=1/2のXXZ模型は

.. math::
   {\cal H}^{XXZ} = -J_{xy} \sum_{\langle i, j\rangle}(S_i^x S_j^x + S_i^y S_j^y)-J_z\sum_{\langle i, j\rangle}S_i^zS_j^z-H \sum_{i}S_{i}^z -\Gamma \sum_i S_i^x,

で与えられる。
入力ファイルで指定するパラメータと上記式のパラメータは以下のように対応する。

  .. csv-table::
     :header-rows: 1
     :widths: 1,1,1

     Parameter, Boson, Spin
     tb, :math:`t_b`, :math:`J_{xy}` 
     UBB, :math:`U_{BB}`, `-`
     VB1, :math:`V_{B1}`, :math:`J_{z}`
     VB2, :math:`V_{B2}`, `-`
     MU, :math:`\mu`, :math:`H`
     G, :math:`\Gamma`, :math:`\Gamma/2` 
