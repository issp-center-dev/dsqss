.. -*- coding: utf-8 -*-
.. highlight:: none

インストール
---------------

必要なライブラリ
********************
DSQSSの使用には以下のプログラム・ライブラリが必要です. 

- BLAS
- LAPACK
- (Optional) MPI (PMWAを使用する場合には必須)
- python 2.7 or 3.x

ダウンロード
********************
- zipファイルをダウンロードする場合
  
  DSQSSの最新版は https://github.com/issp-center-dev/dsqss/releases からダウンロードできます. 

- gitを利用する場合
  
  Gitを利用されている方は, 端末から以下のコマンドを打つことで直接ダウンロードできます. 

  ``$ git clone https://github.com/issp-center-dev/dsqss.git``

フォルダ構成
********************
DSQSSのダウンロード後にzipファイルを解凍すると, ファイルが展開されます(gitを利用された方は, cloneを行ったファイル直下のフォルダ構成になります). 
以下, 重要なファイル・フォルダについてその構成を記載します.

::
   
  ├── CMakeLists.txt
  ├── LICENSE
  ├── README.md
  ├── config
  │   ├── gcc.cmake
  │   └── intel.cmake
  ├── doc
  │   ├── en
  │   └── jp
  ├── sample
  │   ├── dla
  │   └── pmwa
  ├── src
  │   ├── common
  │   ├── dla
  │   ├── pmwa
  │   └── third-party
  │       └── boost
  ├── test
  │   ├── CMakeLists.txt
  │   ├── dla
  │   ├── pmwa
  │   └── tool
  └── tool
       └── dsqss_pre.py


インストール
********************

インストールは以下の手順で行うことが出来ます. 
以下, ダウンロードしたファイルの直下にいることを想定しています. 

::
   
   $ mkdir dsqss.build && cd dsqss.build
   $ cmake ../ -DCMAKE_INSTALL_PREFIX=/path/to/install/to 
   $ make

``/path/to/install/to`` をインストールしたい先のパスに設定してください. 
指定しなかった場合のデフォルト値は ``/usr/local`` です.
なお, cmakeがうまくいかない場合にも, コンパイラを直接指定するとうまくいくことがあります.
詳細については https://github.com/issp-center-dev/HPhi/wiki/FAQ をご覧ください. 

これにより各実行ファイルが ``dsqss.build/src`` フォルダ以下に作成されます. 
次に作成された実行ファイルが正常に動作するかテストするため, 以下のコマンドを打ちます. 

::
   
   $ make test

テストが100%通過したことが確認できた後, 以下のコマンドを入力しインストールします. 
::
   
   $ make install

実行バイナリが先に指定したインストールパスにある ``bin`` ディレクトリに,
サンプルが ``share/dsqss/VERSION/samples`` にインストールされます.
なお, インストール先のパスを変更した場合には, 変更先のパスをexportすることでその場で実行できるようになります. 
