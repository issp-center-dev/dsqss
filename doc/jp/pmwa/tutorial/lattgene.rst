.. -*- coding: utf-8 -*-
.. highlight:: none

格子定義ファイルの作成
==============================

PMWAでは標準的な模型に対して格子定義ファイル ``lattice.xml`` を生成するための簡易ツールとして
``lattgene_P`` を用意しています.ここでは ``lattgene_P`` の使用方法について説明します.

``lattgene_P`` ではcubic latticeに関する格子定義ファイルを作成することが出来ます.
指定するパラメータは下記の通りです.

.. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Parameter, type, 備考
     D, int, 次元数
     L, int, 格子のサイズ(各次元について連続で指定します)
     B, double, 逆温度
     NLdiv, int, Lの分割数 (各次元についてそれぞれ NLdiv分割します)
     NBdiv, int, Bの分割数
     NFIELD, int, 磁場の種類の数(基本的には0に設定)


使用例を以下に記載します.

1. 1次元8サイト, 逆温度10.0, 分割数は1の場合の格子ファイルを定義

   ``$ lattgene_P 1 8 10.0 1 1 0``

2. 2次元4*4サイト, 逆温度10.0, 分割数は1の場合の格子ファイルを定義

   ``$ lattgene_P 2 4 4 10.0 1 1 0``

