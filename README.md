# DSQSS (Discrete Space Quantum Systems Solver)

[![doc latest_stable en](https://img.shields.io/badge/doc--en-v2.0.4-blue.svg)](https://issp-center-dev.github.io/dsqss/manual/v2.0.4/en/index.html)
[![doc latest_stable jp](https://img.shields.io/badge/doc--jp-v2.0.4-blue.svg)](https://issp-center-dev.github.io/dsqss/manual/v2.0.4/jp/index.html)
[![doc develop](https://img.shields.io/badge/doc-dev-blue.svg)](https://issp-center-dev.github.io/dsqss/manual/develop/en/index.html)
[![doc develop jp](https://img.shields.io/badge/doc--jp-dev-blue.svg)](https://issp-center-dev.github.io/dsqss/manual/develop/jp/index.html)

DSQSS (Discrete Space Quantum Systems Solver) is a software package for calculating expectation values of several observables of quantum lattice systems in finite temperature.
DSQSS implements the path-integral Monte Carlo method with the directed loop algorithm.

## Structure of directories

- src
  - source codes of the main programs, DSQSS/DLA and DSQSS/PMWA
- tool
  - utility tools (input file generators)
- sample
  - examples of application
- test
  - test suite
- doc
  - document
- config
  - configuration files used in CMake

## Build and Install

### prerequisite

- C++ Compiler
- CMake >=2.8.12
- Python >=3.6
  - numpy
  - scipy
  - toml
  - typing_extensions (if Python < 3.8)

### Simple build

``` bash
rm -rf build
mkdir build
cd build
cmake ../
make
```

#### Notes

To change compiler, add `-DCMAKE_CXX_COMPILER` like

``` bash
cmake ../ -DCMAKE_CXX_COMPILER=`which icpc`
```

For Intel compiler, DSQSS offers another option `-DCONFIG=intel` for setting compiler and adding compiler options

```bash
cmake ../ -DCONFIG=intel
```

You can find executable files in `build/src/dla`, `build/tool` and `build/src/pmwa/`.

### build manual

[Sphinx](http://www.sphinx-doc.org) and `sphinxcontrib-spelling` are required.
LaTeX is required for PDF format.

``` bash
# in the build directory,
cmake -DDocument=ON ../
make doc
```

### Testing

``` bash
# in the build directory,
ctest
```

When some tests failed, you can check output of these tests by following:

``` bash
# in the build directory,
ctest -V -R "test name"
```

Test names can be specified by the regular expression.

### Install

The path to install is specified by `-DCMAKE_INSTALL_PREFIX` as the following,

``` bash
# in the build directory,
cmake -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR} ../
make install
```

After this process, executable files such as `dla` are installed to `${INSTALL_DIR}/bin`,
a configuration file `dsqssvars-${DSQSS_VERSION}.sh` to `${INSTALL_DIR}/share/dsqss/`,
sample files to `${INSTALL_DIR}/share/dsqss/dsqss-${DSQSS_VERSION}/sample`,
and documents (if built) to `${INSTALL_DIR}/share/dsqss/dsqss-${DSQSS_VERSION}/doc`.

### Example

``` bash
source ${INSTALL_DIR}/share/dsqss/dsqssvars-${DSQSS_VERSION}.sh
cd sample/dla/01_spindimer
dla_pre std.toml
dla param.in
cat sample.log | grep ene
```

For details, [see manual page](https://issp-center-dev.github.io/dsqss/manual/master/en/dla/tutorial/spindimer.html).

## Paper

We would appreciate if you cite the following article in your research with DSQSS,
[Y. Motoyama, K. Yoshimi, A. Masaki-Kato, T. Kato, and N. Kawashima, Comput. Phys. Commun. 264, 107944 (2021)](https://www.sciencedirect.com/science/article/pii/S0010465521000692).

The preprint is [arXiv:2007.11329](https://arxiv.org/abs/2007.11329).

## License

### License of DSQSS

DSQSS is distributed under the GNU GPL v3.

### License of the bundled libs

- [Boost C++ library](https://www.boost.org/) is redistributed under the Boost software license.
- [Plog](https://github.com/SergiusTheBest/plog) is redistributed under the Mozilla Public License 2.0
- [`FindPythonModule.cmake`](https://github.com/openturns/openturns/tree/master/cmake.FindPythonModule.cmake) is redistributed under the OSI approved BSD license.

## Acknowledgement

DSQSS v1.2 and v2.0 are developed under the support of "Project for advancement of software usability in materials science" in fiscal year 2018 by The Institute for Solid State Physics, The University of Tokyo.
