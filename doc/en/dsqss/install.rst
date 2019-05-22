.. -*- coding: utf-8 -*-
.. highlight:: none

How to install
---------------

Requirements
********************

- (Optional) MPI (essential for PMWA)
- python 2.7 or 3.4+

   - numpy
   - scipy
   - toml

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
When the build procedure failed, please try directly to set the compilers.
For details, see https://github.com/issp-center-dev/HPhi/wiki/FAQ .

Each binary files for dsqss will be made in `src` directory.
To check whether the binary files are correctly made or not,  
please type the following command:

::
   
   $ make test


After seeing that all tests are passed,
type the following command :

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
