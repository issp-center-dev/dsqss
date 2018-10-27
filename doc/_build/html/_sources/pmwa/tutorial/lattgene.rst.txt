.. -*- coding: utf-8 -*-
.. highlight:: none

格子生成ツール：lattgene
==============================

DSQSSでは標準的な模型に対して格子定義ファイル ``lattice.xml`` を生成するための簡易ツールとして ``lattgene`` を用意してあります。ここでは ``lattgene`` の使用方法について説明します。

``lattgene`` ではcubic latticeに関する格子定義ファイルを作成することが出来ます。
指定するパラメータは下記の通りです。

.. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Parameter, type, 備考
     D, int, 次元数
     L, int, 格子のサイズ(各次元について連続で指定します)
     B, double, 逆温度
     NLdiv, int, Lの分割数 (各次元についてNLdivで等分割します)
     NBdiv, int, Bの分割数
     NFIELD, int, 磁場の種類の数(デフォルト値は0)


分割数はMPIで使用するプロセス数に対応します(要確認)。
使用例を以下に記載します。

1. 1次元8サイト, 逆温度10.0, 分割数は1の場合の格子ファイルを定義

   ``$ lattgene 1 8 10.0 1 1``

2. 2次元4*4サイト, 逆温度10.0, 分割数は1の場合の格子ファイルを定義

   ``$ lattgene 2 4 4 10.0 1 1``

