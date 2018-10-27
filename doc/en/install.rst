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
  
  The latest version of DSQSS can be obtained from  `here <https://github.com/qmc/dsqss>`_ .
  After clicking "Clone or download" button, select "Download ZIP". Then, a zip file will be downloaded.

- Use git
  
  Type the following command:

  ``$ git clone https://github.com/qmc/dsqss.git``

Directory structure
********************

::
   
  |── CMakeLists.txt
  |── LICENSE
  |── README.md
  |── config
  │   |── gcc.cmake
  │   -── intel.cmake
  |── doc
  │   |── CMakeLists.txt
  │   |── _static
  │   |── conf.py
  │   |── dla
  │   |── dsqss
  │   |── index.rst
  │   -── pmwa
  |── sample
  │   |── dla
  │   ─-─ pmwa
  |── src
  │   |── dla
  │   |── pmwa
  │   -── third-party
  │       |── boost
  |── test
  │   |── CMakeLists.txt
  │   |── dla
  │   -── pmwa
  -── tool
       |── CMakeLists.txt
       |── dsqss_pre.py


Install
********************

The installation of DSQSS can be done by the following procedures.
In the following, we assume the current directory is `dsqss.src`.

::
   
   $ mkdir dsqss.build && cd dsqss.build
   $ cmake ../ 
   $ make


It is noted that the default install directory is set as ``/usr/local/bin`` .
To change the install directory, add the following option to cmake (replace ``/path/to/install/to`` with the directory path where you want to install dsqss).

::
   
   $ cmake -DCMAKE_INSTALL_PREFIX=/path/to/install/to


Each binary files for dsqss will be made in `src` forlder.
To check whether the binary files are correctly made or not,  
please type the following command:

::
   
   $ make test


After seeing that all tests are passed,
type the following command to install binary files:

::
   
   $ make install


If the path for installation was changed, 
it is convinent to export the path:

::
   
   $ export PATH="/path/to/install/to:$PATH"
