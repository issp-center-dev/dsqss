.. -*- coding: utf-8 -*-
.. highlight:: none

How to install
---------------

Requirements
********************

- (Optional) MPI (essential for PMWA)
- python 2.7 or 3.x

   - numpy
   - scipy
   - toml
   - pip (essential for ``make install``)

Download
********************
- Download zip file
  
  The latest version of DSQSS can be obtained from  `here <https://github.com/issp-center-dev/dsqss/releases>`_ .

- Use git
  
  Type the following command:

  ``$ git clone https://github.com/issp-center-dev/dsqss.git``

Directory structure
********************

::
   
  |── CMakeLists.txt
  |── LICENSE
  |── README.md
  |── config
  |   |── gcc.cmake
  |   |── intel.cmake
  |
  |── doc
  |   |── en
  |   |── jp
  |
  |── sample
  |   |── CMakeLists.txt
  |   |── dla
  |   |── pmwa
  |
  |── src
  |   |── common
  |   |── dla
  |   |── pmwa
  |   |── third-party
  |       |── boost
  |
  |── test
  |   |── CMakeLists.txt
  |   |── dla
  |   |── pmwa
  |   |── tool
  |
  |── tool
      |── CMakeLists.txt
      |── dsqss_pre.py

Install
********************

The installation of DSQSS can be done by the following procedures.
In the following, we assume the current directory is ``dsqss.src``.

::
   
   $ mkdir dsqss.build && cd dsqss.build
   $ cmake ../ -DCMAKE_INSTALL_PREFIX=/path/to/install/to
   $ make

Replace ``/path/to/install/to`` with the directory path where you want to install dsqss.   
It is noted that the default install directory is set as ``/usr/local/bin`` .
When the build procedure failed, please try directly to set the compilers.
For details, see https://github.com/issp-center-dev/HPhi/wiki/FAQ .

Each binary files for dsqss will be made in `src` folder.
To check whether the binary files are correctly made or not,  
please type the following command:

::
   
   $ make test


After seeing that all tests are passed,
type the following command to install files:

::
   
   $ make install


This installs the executable files into ``bin`` directory,
the samples into ``share/dsqss/VERSION/samples`` directory,
and a configure file ``dsqssvars-VERSION.sh`` into ``share/dsqss`` directory.
Before executing utility tools and solvers, ``source`` the configure file in order to set the environment variables properly.
::

   $ source share/dsqss/dsqssvar-VERSION.sh
