.. -*- coding: utf-8 -*-
.. highlight:: none

計算実行
==============================
入力ファイル作成後,以下のコマンドを入力することで実行できます(入力ファイルは ``param.in`` としています).
スピン系とハードコアボソン系に応じて,

それぞれ実行ファイルが異なります.

1. スピン系の場合

  ``$pmwa_H param.in``
  
2. ハードコアボソンの場合

  ``$pmwa_B param.in``

計算終了後,結果ファイル1つとリスタート用の一時ファイル2つ(evout_sample.log, RND_evout_sample.log)が出力されます.

  
出力ファイル
==============================
ここでは結果ファイル1つとリスタート用の一時ファイル2つについて,
PMWAに特有のパラメータをそれぞれ記載します.

- 結果ファイル
  
  .. csv-table::
     :header-rows: 1
     :widths: 1,1,4

     Kind, Name, Description
     P, L, 三次元の格子情報 
     P, DOML, 並列化により分割されたドメインの大きさ
     P, DOMBETA, 並列化により分割された逆温度のドメインサイズ
     P, NDIVL, 格子の分割数 
     P, NTEST, テストをするサンプルの数(詳細はモンテカルロ計算の箇所で説明)
     R, nver, キンクとワームの数
     R, nkin, キンクの数
     R, wndx, x方向のワインディング数の二乗の期待値
     R, wndy, y方向のワインディング数の二乗の期待値
     R, wndz, z方向のワインディング数の二乗の期待値
     R, wnd2, ワインディング数の二乗の総数(wndx+wndy+wndz)
     R, bmxu, :math:`S_x` の期待値 (uniform :math:`\tau` 積分)
     R, bmpu, :math:`S_+` の期待値 (uniform :math:`\tau` 積分)
     R, bmmu, :math:`S_-` の期待値 (uniform :math:`\tau` 積分)
     R, comp, 圧縮率
     R, lxmx, 各サイトの局所的なワーム数の揺らぎ
     I, the maximum number of vertices, バーテックスの最大数
     I, the maximum number of worms, ワームの最大数

  ここで種別は出力の各行の先頭に付与される文字で, P, R, I はそれぞれParameter, Result, Informationを示します.

- リスタート用のファイル
	
  PMWAではリスタート機能が実装されており,下記の2ファイルが存在する場合には強制的にリスタートが行われます.
  以下,各ファイルの出力内容について簡単に説明します.
	
  1. evout_sample.log
    
  計算終了時のサイクル数,ワールドラインの情報,バーテックスの情報について出力したファイル.
  再計算時には読み込んだ配置を始条件として計算が行われます.
  ::
  
    26 : 計算終了時のサイクル数
    0 1 : ドメイン内のサイト0のワールドラインの情報 
    i/N beta, (i+1)/N beta 区間のワールドラインの情報 : 0: down, 1: up
    0 0 : ドメイン内のサイト1のワールドラインの情報
    1 1  :ドメイン内のサイト2のワールドラインの情報
    ...
    8 0.056997107 2 1 4 :バーテックスのラベル, tau, バーテックスの種類 , ワールドラインの本数, ボンド番号
    9 0.056997107 2 0 5
    44 0.28066013 2 1 3
  	    
  ここでバーテックスの種類については以下のものが定義されています. 

  .. csv-table::
    :header-rows: 1
    :widths: 1,4
  
    バーテックスの種類, 説明
    -5, 各ドメインにおける虚時間方向の始点及び終点.ドメイン分割しなくても有効.
    "-4(右), -2(左)", ドメインをまたぐ対角的バーテックス.ドメイン分割しなくても有効.
    "-3(右), -1(左)", ドメインをまたぐ非対角的バーテックス(キンク).ドメイン分割しなくても有効.
    0, オンサイトバーテックス(ワーム以外).
    1, 2サイトバーテックス.
    2, キンク.
    3, 2サイトバーテックス(次近接)(互換性のために残してあります).
    4, その時動いているワーム(消滅演算子).
    5, その時動いているワーム(生成演算子).
    6, "その時止まっているワーム(生成消滅関係なし),もしくは不要なバーテックス."
    7, マーカー(虚時間相関関数測定用).
  
  2. RNDevout_sample.log
	    
  乱数生成を行っているオブジェクトをバイナリ形式で出力したファイル.
  再計算時には読み込んだ乱数情報を始条件として計算が行われます.

