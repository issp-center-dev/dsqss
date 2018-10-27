# for Intel Compiler

set(CMAKE_C_COMPILER "icc" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER "icpc" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g -Wall -Wformat -Werror=format-security")
set(CMAKE_CXX_FLAGS_RELEASE "-Wno-unknown-pragmas -wd2196 -O3 -DNDEBUG -DHAVE_SSE2" CACHE STRING "" FORCE)

# for Intel MKL
set(BLA_VENDOR "Intel10_64lp" CACHE STRING "" FORCE)
