.. -*- coding: utf-8 -*-
.. highlight:: none

インストール
---------------

必要なライブラリ
********************
DSQSSの使用には以下のプログラム・ライブラリが必要です. 

- (Optional) MPI (PMWAを使用する場合には必須)
- python 3.6+

   - numpy
   - scipy
   - toml
   

ダウンロード
********************
- アーカイブファイル (tar.gz) をダウンロードする場合
  
  DSQSSの最新版は https://github.com/issp-center-dev/dsqss/releases からダウンロードできます. 

- gitを利用する場合
  
  Gitを利用されている方は, 端末から以下のコマンドを打つことで直接ダウンロードできます. 

  ``$ git clone https://github.com/issp-center-dev/dsqss.git``

フォルダ構成
********************
DSQSSのダウンロード後にzipファイルを解凍すると, ファイルが展開されます(gitを利用された方は, cloneを行ったファイル直下のフォルダ構成になります). 
以下, 重要なファイル・フォルダについてその構成を記載します.

::

  |-- CMakeLists.txt
  |-- LICENSE
  |-- README.md
  |-- config/
  |-- doc/
  |-- sample/
  |   |-- dla/
  |   `-- pmwa/
  |-- src/
  |   |-- common/
  |   |-- dla/
  |   |-- pmwa/
  |   `-- third-party/
  |-- test/
  |   |-- dla/
  |   |-- pmwa/
  |   `-- tool/
  `-- tool/
      |-- cmake/
      |-- dsqss/
      `-- setup.py

インストール
********************

インストールは以下の手順で行うことが出来ます. 
以下, ダウンロードしたファイルの直下にいることを想定しています. 

::
   
   $ mkdir dsqss.build && cd dsqss.build
   $ cmake ../ -DCMAKE_INSTALL_PREFIX=/path/to/install/to 
   $ make

``/path/to/install/to`` をインストールしたい先のパスに設定してください（例： ``$HOME/opt/dsqss`` ）. 
指定しなかった場合のデフォルト値は ``/usr/local`` です.


.. note::

  CMake はデフォルトで ``/usr/bin/c++`` を C++ コンパイラとして使用します.
  これ以外のコンパイラ、例えば インテルコンパイラ ``icpc`` を使いたい場合は,
  ``-DCMAKE_CXX_COMPILER`` オプションを用いて明示的に指定してください::

    $ cmake ../ -DCMAKE_CXX_COMPILER=icpc

  インテルコンパイラに関しては, コンパイルオプション込みで設定するためのオプションを DSQSS 側で用意してあります::

    $ cmake ../ -DCONFIG=intel

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
DSQSS の実行前にはこのファイルを ``source`` コマンドで読み込んでください. ::

   $ source share/dsqss/dsqssvar-VERSION.sh
