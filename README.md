# dsqss

[![Build Status](https://travis-ci.org/issp-center-dev/dsqss.svg?branch=master)](https://travis-ci.org/issp-center-dev/dsqss)


## Build

### Simple build

``` bash
rm -rf build
mkdir build
cd build
cmake ../
make
```

You can find executable files in `build/src/dla`, `build/src/dla/generators` and `build/src/pmwa/`.

### build manual

[Sphinx](http://www.sphinx-doc.org) is required.
LaTeX is required for PDF format.

``` bash
cmake -DDocument=ON ../
make doc
```

### Testing

``` bash
ctest
```

When some tests failed, you can check output of these tests by following:

``` bash
ctest -V -R "test name"
```

Test names can be specified by the regular expression.

### Install

``` bash
cmake -DCMAKE_INSTALL_PREFIX=/path/to/install/to ../
make install
```

## License
### License of DSQSS
DSQSS is distributed under the GNU GPL v3.

### License of the bundled libs
- Boost C++ library is redistributed under the Boost software license.

