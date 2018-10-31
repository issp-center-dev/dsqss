.. -*- coding: utf-8 -*-
.. highlight:: none

How to install
---------------

Requirements
********************

- BLAS
- LAPACK
- (Optional) MPI (essential for PMWA)

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

Install
********************

The installation of DSQSS can be done by the following procedures.
In the following, we assume the current directory is `dsqss.src`.

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
type the following command to install binary files:

::
   
   $ make install

If the path for installation was changed, 
it is convenient to export the path:

::
   
   $ export PATH="/path/to/install/to:$PATH"
