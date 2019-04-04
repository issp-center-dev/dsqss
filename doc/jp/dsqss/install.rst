.. -*- coding: utf-8 -*-
.. highlight:: none

インストール
---------------

必要なライブラリ
********************
DSQSSの使用には以下のプログラム・ライブラリが必要です. 

- (Optional) MPI (PMWAを使用する場合には必須)
- python 2.7 or 3.x

   - numpy
   - scipy
   - toml
   - pip (make install する場合には必須)
   

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
  ├── config                 #CMake用のconfigure fileを格納したフォルダ
  │   ├── gcc.cmake
  │   └── intel.cmake
  ├── doc                    #マニュアルのソース一式を格納したフォルダ
  │   ├── en
  │   └── jp
  ├── sample                 #サンプルファイルを格納したフォルダ
  │   ├── CMakeLists.txt
  │   ├── dla
  │   └── pmwa
  ├── src                    #ソースファイル一式を格納したフォルダ
  │   ├── common
  │   ├── dla
  │   ├── pmwa
  │   └── third-party
  │       └── boost
  ├── test                   #CTest用のファイル一式を格納したフォルダ
  │   ├── CMakeLists.txt
  │   ├── dla
  │   ├── pmwa
  │   └── tool
  └── tool                  
       ├── CMakeLists.txt
       ├── setup.py          #DSQSS用のツールのセットアップ用スクリプト
       └── dsqss             #DSQSS用のツールを格納したフォルダ


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

これにより各実行ファイルが ``dsqss.build/src`` ディレクトリ以下に, 
入力ファイルの生成ツールが ``dsqss.build/tool`` ディレクトリ以下に作成されます. 
次に作成された実行ファイルが正常に動作するかテストするため, 以下のコマンドを打ちます. 

::
   
   $ make test

テストが100%通過したことが確認できた後, 以下のコマンドを入力しインストールします. 
::
   
   $ make install

実行バイナリが先に指定したインストールパスにある ``bin`` ディレクトリに,
サンプルが ``share/dsqss/VERSION/samples`` にインストールされます.
また, 補助ツールを含めたDSQSS の実行に必要な環境変数を設定するためのファイルが ``share/dsqss/dsqssvar-VERSION.sh`` に生成されます.
このファイルを ``source`` コマンドで読み込んでください. ::

   $ source share/dsqss/dsqssvar-VERSION.sh
