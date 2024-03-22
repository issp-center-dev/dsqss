.. -*- coding: utf-8 -*-
.. highlight:: none

How to install
---------------

Requirements
********************

- (Optional) MPI (essential for PMWA)
- python 3.6+

   - numpy
   - scipy
   - toml
   - typing_extensions (if python 3.6 or 3.7 is used)

Download
********************
- Download zip file
  
  The latest version of DSQSS can be obtained from https://github.com/issp-center-dev/dsqss/releases

- Use git
  
  Type the following command:

  ``$ git clone https://github.com/issp-center-dev/dsqss.git``

Directory structure
********************

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


Install
********************

The installation of DSQSS can be done by the following procedures.
In the following, we assume that you are in the root directory of dsqss.

::
   
   $ mkdir dsqss.build && cd dsqss.build
   $ cmake ../ -DCMAKE_INSTALL_PREFIX=/path/to/install/to
   $ make

Replace ``/path/to/install/to`` with the directory path where you want to install dsqss.   
It is noted that the default install directory is set as ``/usr/local/bin`` .

.. note::

  By default, CMake usually sets ``/usr/bin/c++`` as a C++ compiler for building DSQSS.
  If you want to use another C++ compiler like ``icpc`` (Intel compiler,)
  you should tell it to CMake by using ``-DCMAKE_CXX_COMPILER`` option::

    $ cmake ../ -DCMAKE_CXX_COMPILER=icpc

  For Intel compiler, DSQSS offers another option to set compiler options besides compiler itself as following::

    $ cmake ../ -DCONFIG=intel

  For details, see https://github.com/issp-center-dev/HPhi/wiki/FAQ .


.. note::

   CMake automatically search for an interpreter of Python.
   If you want to use a specific one,
   you should tell it to CMake by using ``-DPYTHON_EXECUTABLE`` option::

     $ cmake ../ -DPYTHON_EXECUTABLE=`which python3`

Each binary files for dsqss will be made in ``src`` and ``tool`` directories.
To check whether the binary files are correctly made or not,  
please type the following command:

::
   
   $ make test


After seeing that all tests are passed,
type the following command to install files:

::
   
   $ make install

This command installs executable files into the ``bin`` directory,
the sample files into the ``share/dsqss/VERSION/samples`` directory,
and the python package ``dsqss`` into the ``lib`` directory
under the install path before you set.
One configuration file for setting environment variables to perform DSQSS commands will also be installed as ``share/dsqss/dsqssvar-VERSION.sh`` .
Before invoke DSQSS commands, please load this file by ``source`` command as ::

   $ source share/dsqss/dsqssvar-VERSION.sh

In the remaining, it is assumed that DSQSS is installed and this configuration file is loaded.
