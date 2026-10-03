# Script to generate a file with the version information when DSQSS is built
#
#   cmake -DDSQSS_SOURCE_DIR=... -DDSQSS_VERSION=... -DGIT_EXECUTABLE=...
#         -DTEMPLATE=... -DOUTPUT=... -P GenerateVersionFile.cmake

# A script run by "cmake -P" starts without the policies. CMake 3 then expands
# @VAR@ in quoted arguments (the old behavior of CMP0053), for example.
cmake_minimum_required(VERSION 3.1...3.20.2)

include("${CMAKE_CURRENT_LIST_DIR}/DsqssVersion.cmake")

dsqss_generate_version_file(
  "${DSQSS_SOURCE_DIR}" "${DSQSS_VERSION}" "${TEMPLATE}" "${OUTPUT}")
